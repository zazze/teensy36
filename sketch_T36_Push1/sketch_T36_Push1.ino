/* Create a "class compliant " USB to 2 more USB connected devices. Admittedly, you could just plug
   those 2 devices directly into your computer, but this example is meant
   to show how to forward any MIDI message between the 3 different MIDI
   libraries.  A "real" application might do something more interesting,
   like translate or modify the MIDI messages....

   A USB host cable
   is needed on Teensy 3.6's second USB port, and obviously USB hubs
   are needed to connect up to 2 USB MIDI devices. 
   This is also a way to test multiport USB-midi devices like Ableton Push.

   You must select MIDIx16 from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

// Updated 20210701 by Johan Zetterqvist
// Updated 20210725 by Johan Zetterqvist
// Updated 20210726 by Johan Zetterqvist
// Updated 20210727 by Johan Zetterqvist
// Updated 20210729 by Johan Zetterqvist
// Updated 20210822 by Johan Zetterqvist
// Updated 20210823 by Johan Zetterqvist
// Updated 20210824 by Johan Zetterqvist
// Updated 20210825 by Johan Zetterqvist

#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)

// Color definitions
#define BW 0
#define RED 5
#define AMBER 9
#define YELLOW 13
#define LIME 17
#define GREEN 21
#define SPRING 25
#define TURQUOISE 29
#define CYAN 33
#define SKY 37
#define OCEAN 41
#define BLUE 45
#define ORCHID 49
#define MAGENTA 53
#define PINK 57

// Shade for RGB
#define BRIGHT 0
#define NORMAL 1
#define DIM 2
#define MIX 3 //Mix with next color

// Shade for Black and white
#define BLACK 0 
#define DARK_GREY 1
#define GREY 2
#define WHITE 3

// Transition type
#define STOP 0
#define ONESHOT 1
#define PULSE 6
#define BLINK 11

// Transition speed
#define VERYFAST 0
#define FAST 1
#define MODERATE 2
#define SLOW 3
#define VERYSLOW 4

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi01(myusb);
MIDIDevice midi02(myusb);
MIDIDevice * midilist[2] = {&midi01, &midi02};

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;


void setPadLED(int row, int column, 
            int color = BW, int shade = NORMAL, 
            int transition = STOP, int trspeed = MODERATE) {
              
    midi01.sendNoteOn(27 + (row*8) + column, // MIDI-note 
                      color + shade, // MIDI-velocity
                      transition*5 + trspeed + 1, // MIDI-channel
                      1 // MIDI Virtual port
                      );
    
}

// Standard scale on Push - major and minor scales with
// increasing rows shifted a fourth. 29 notes spanning 4 octaves
void set8Scale4ths(uint8_t degree = 1) {
  delay(500);
    // Loop over rows counting from the bottom and up
    for (int i = 1; i <= 8; i++) {
      // Loop over columns counting from left to right
      for (int j = 1; j <= 8; j++) {
        if (degree == 1) {
          setPadLED(i, j, BLUE, NORMAL, STOP, MODERATE);
        } else {
          setPadLED(i, j, BW, GREY, STOP, MODERATE);
          }
        degree++;
        degree = degree % 7;
        delay(1);
      }
      degree = (degree+2) % 7;
    }
}

// // Translate degree to integer to add to get midi note number;
// uint8_t degree2MIDIAdd(uint8_t degree, uint8_t scale) {
//   if (degree == 0) {
//     return degree;
//   } else {
//     if (scale == 1) {
//       if (degree < 3) {
//         return degree*2
//       } else {
//         return degree*2 - 1; 
//       }
//     }
//   }
// }

// // Translate pressed note to a note in the selected scale and octave
// void create8Scale4thsNotes(uint8_t midival, uint8_t root = 0, int8_t octave = 5, uint8_t scale = 1) {
//   uint8_t newnote;
//   newnote = 12*octave + root + degree2MIDIAdd(degree, scale);
//   , uint8_t root = 1, uint8_t root = 1, ;
// }

void clearLCDRow(byte row) {
  byte rowid=27+row;
  const byte sedat[] = {240, 71, 127, 21, rowid, 0, 0, 247};
  midi01.sendSysEx(8, sedat, true);
}

void setLCDChar(uint8_t row = 1, uint8_t col = 1, uint8_t startpos = 1, byte ch_b = 'x') {
  byte row_b = 23 + row, startpos_b = (col-1)*17 + startpos - 1;
  byte sedat[] = {240, 71, 127, 21, row_b, 0, 2, startpos_b, ch_b, 247};
  midi01.sendSysEx(9+1, sedat, true);
}

void writeLCDText4x4(uint8_t row = 1, uint8_t col = 1, String str = "default") {
  uint8_t nchars, i = 0;
  nchars = min(str.length(), (uint8_t)17);
  byte sxchars[26] = {180};
  sxchars[0] = 240;
  sxchars[1] = 71;
  sxchars[2] = 127;
  sxchars[3] = 21;
  sxchars[4] = row+23;
  sxchars[5] = 0;
  sxchars[6] = 18;
  sxchars[7] = 17*(col-1);
  sxchars[25] = 247;

  while(i <  nchars) {
    sxchars[8+i] = str[i];
    i++;
  }
  while(i < 17) {
    sxchars[8+i] = ' ';
    i++;
  }
  
  midi01.sendSysEx(26, sxchars, true);
}

void writeLCDText4x8(uint8_t row = 1, uint8_t col = 1, String str = "default") {
  
  uint8_t nchars, i = 0, block, blockpos, startpos;
  nchars = min(str.length(), (uint8_t)8);
  block = col / 2; // Which of the four blocks?
  blockpos = 1 - (col % 2) ; // Which part of the block? The 1st (0) or 2nd (1)?
  startpos = 17*(block-1) + 9*blockpos;
  byte sxchars[17] = {180};
  sxchars[0] = 240;
  sxchars[1] = 71;
  sxchars[2] = 127;
  sxchars[3] = 21;
  sxchars[4] = row+23;
  sxchars[5] = 0;
  sxchars[6] = 9;
  sxchars[7] = startpos;
  sxchars[16] = 247;

  while(i <  nchars) {
    sxchars[8+i] = str[i];
    i++;
  }
  while(i < 8) {
    sxchars[8+i] = ' ';
    i++;
  }
  
  midi01.sendSysEx(17, sxchars, true);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void myNoteOn(byte channel, byte note, byte velocity) {
//      midi01.sendNoteOn(note, // MIDI-note 
//                      21 + 1, // MIDI-velocity
//                      0*5 + 0 + 1,// MIDI-channel
//                      1 // MIDI virtual cable
//                      );
//
//  
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void sendToComputer(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray, byte cable)
{
    usbMIDI.send(type, data1, data2, channel, cable);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
//  pinMode(13, OUTPUT); // LED pin
//  digitalWrite(13, LOW);
  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  Serial.println("Push 1 Example");
  delay(10);

  myusb.begin();

  midi01.setHandleNoteOn(myNoteOn);

  delay(500);
  set8Scale4ths();
  delay(100);
  clearLCDRow(1);
  clearLCDRow(2);
  clearLCDRow(3);
  clearLCDRow(4);

//  setLCDChar(1, 2, 2, '8');
//  setLCDChar(2, 3, 4, '2');
//
//  writeLCDText4x4(2, 2, "12345678911234567");
//  writeLCDText4x4();
//
//  writeLCDText4x8(3,7,"Hej!");
//  writeLCDText4x8(4,6,"Hejda!");

  writeLCDText4x4(2,2,"Teensy 3.6 & Push"); 
  writeLCDText4x4(2,3,"   by Johan Z");
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//bool firstrun = true;

void loop() {
  
  bool activity = false;

  // Next read messages arriving from the (up to) 2 USB devices plugged into the USB Host port
  for (int port=0; port < 2; port++) {
    if (midilist[port]->read()) {
      uint8_t type =       midilist[port]->getType();
      uint8_t data1 =      midilist[port]->getData1();
      uint8_t data2 =      midilist[port]->getData2();
      uint8_t channel =    midilist[port]->getChannel();
      const uint8_t *sys = midilist[port]->getSysExArray();
      sendToComputer(type, data1, data2, channel, sys, port);
      activity = true;
    }
  }

  // Finally, read any messages the PC sends to Teensy, and forward them
  // to USB devices on the USB host port.
  if (usbMIDI.read()) {
    // get the USB MIDI message, defined by these 5 numbers (except SysEX)
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
//    byte cable = usbMIDI.getCable();

    // forward this message to 1 of the 3 Serial MIDI OUT ports
    if (type != usbMIDI.SystemExclusive) {

      // Then simply give the data to the MIDI library send()
//      midilist[cable]->send(type, data1, data2, channel);
      midi01.send(type, data1, data2, channel);
      

    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
//      midilist[cable]->sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
      midi01.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
    }
    activity = true;
  }

  // blink the LED when any activity has happened
  if (activity) {
    digitalWriteFast(13, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15) {
    digitalWriteFast(13, LOW);  // LED off
  }
  
}
