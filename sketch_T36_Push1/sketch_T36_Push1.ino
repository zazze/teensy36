/* Create a "class compliant " USB to 2 more USB connected devices. Admittedly, you could just plug
   those 2 devices directly into your computer, but this example is meant
   to show how to forward any MIDI message between the 3 different MIDI
   libraries.  A "real" application might do something more interesting,
   like translate or modify the MIDI messages....

   A USB host cable
   is needed on Teensy 3.6's second USB port, and obviously USB hubs
   are needed to connect up to 2 USB MIDI devices. 
   This is also a way to test multiporlt USB-midi devices like Ableton Push.

   You must select MIDIx16 from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

// Updated 20210701 by Johan Zetterqvist
// Updated 20210725 by Johan Zetterqvist
// Updated 20210726 by Johan Zetterqvist
// Updated 20210727 by Johan Zetterqvist
// Updated 20210729 by Johan Zetterqvist

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

// Code 

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi01(myusb);
MIDIDevice midi02(myusb);
MIDIDevice * midilist[2] = {&midi01, &midi02};

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void setPadLED(int row, int column, 
            int color = BW, int shade = NORMAL, 
            int transition = STOP, int trspeed = MODERATE) {
              
//    midi01.sendNoteOn(27 + (row*8) + column, // MIDI-note 
//                      color + shade, // MIDI-velocity
//                      transition*5 + trspeed + 1, // MIDI-channel
//                      1); // MIDI-port

    midi01.sendNoteOn(27 + (row*8) + column, // MIDI-note 
                      color + shade, // MIDI-velocity
                      transition*5 + trspeed + 1, // MIDI-channel
                      1 // MIDI Virtual port
                      );

    Serial.print("row=");
    Serial.print(row, DEC);
    Serial.print(", column=");
    Serial.print(column, DEC);
    Serial.print(", color=");
    Serial.print(color, DEC);
    Serial.print(", shade=");
    Serial.print(shade, DEC);
    Serial.print(" and velocity=");
    Serial.print(color, DEC);
    Serial.println(".");
    
}

//  // Loop over rows counting from the bottom and up
//  for (int i = 0; i < 8; i++) {
//    // Looper over columns counting from left to right
//    for (int j = 0; j < 8; j++) {
//      if ((i == 0 || i == 7) && (j == 0 || j == 7)) {
//        
//      }
//      }
//  }


// Standard scale on Push - major and minor scales with
// increasing rows shifted a fourth. 29 notes spanning 4 octaves
void set8Scale4ths() {
  int degree = 1;
  delay(1500);
//  int degcol[8] = {5, 9, 13, 17, 21, 25, 
    // Loop over rows counting from the bottom and up
    for (int i = 1; i <= 8; i++) {
      // Looper over columns counting from left to right
      for (int j = 1; j <= 8; j++) {
        if (degree == 1) {
          setPadLED(i, j, BLUE, NORMAL, STOP, MODERATE);
//          setPadLED(i, j, 5 + 4*degree, BRIGHT, STOP, MODERATE);
        } else {
          setPadLED(i, j, BW, GREY, STOP, MODERATE);
//          setPadLED(i, j,  5 + 4*degree, NORMAL, STOP, MODERATE);
        }
        degree++;
        degree = degree % 7;
        delay(50);
      }
//      degree = (degree+1) % 7;
      degree = (degree+2) % 7;
    }
}

//void setLCD(int row, int column, 
//            int color = BW, int shade = NORMAL, 
//            int transition = STOP, int trspeed = MODERATE) {

//  byte sedat[] = {240, 71, 127, 21, 27+1, 0, 0, 247}; // Clear 1st line of LCD
//  byte sedat[] = {240, 71, 127, 21, 27+2, 0, 0, 247}; // Clear 2nd line of LCD
//  byte sedat[] = {240, 71, 127, 21, 27+3, 0, 0, 247}; // Clear 3rd line of LCD
//  midi01.sendSysEx(8, sedat, true);

void clearLCDRow(byte row) {
  byte rowid=27+row;
  const byte sedat[] = {240, 71, 127, 21, rowid, 0, 0, 247};
  midi01.sendSysEx(8, sedat, true);
}



void writeLCDText2Row(uint8_t row, uint8_t startpos, String str) {
  int nchars = str.length();
  byte row_b = 23+row, nchars_b = nchars+1, byte1_b, byte2_b, startpos_b;
  char chbuf[nchars] ;
  int asciibuf[nchars];  
  str.toCharArray(chbuf, nchars);
  byte1_b = chbuf[0];
  byte2_b = chbuf[1];
  byte sedat[] = {240, 71, 127, 21, row_b, 0, nchars_b, startpos_b, byte1_b, byte2_b, 247};

//  for (i = 0; i < nchars; i++)
//  asciibuf = (int)chbuf; 
  Serial.print("str: ");
  Serial.println(str);
  Serial.print("nchars: ");
  Serial.println(nchars);  
  Serial.println("chbuf positions 1-2:");
  Serial.println(chbuf[1]); 
  Serial.println(chbuf[2]); 
  
//  byte sedat[] = {240, 71, 127, 21, 23+row, 0, nchars+1, startpos, (int*) chbuf, 247};
  Serial.println("sedat positions 0-10:");
  Serial.println(sedat[0]);
  Serial.println(sedat[1]);
  Serial.println(sedat[2]);
  Serial.println(sedat[3]);
  Serial.println(sedat[4]);
  Serial.println(sedat[5]);
  Serial.println(sedat[6]);
  Serial.println(sedat[7]);
  Serial.println(sedat[8]);
  Serial.println(sedat[9]);
  Serial.println(sedat[10]);
  
  midi01.sendSysEx(11, sedat, true);
  
}

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
//  MIDI.begin();

  midi01.setHandleNoteOn(myNoteOn);

//    midi01.sendNoteOn(27 + 8*3 + 2, // MIDI-note 
//                      125, // MIDI-velocity
//                      6// MIDI-channel
//                      );

//setPadLED(1, 1, BW, WHITE, STOP, MODERATE);
//setPadLED(2, 3, CYAN, NORMAL, PULSE, MODERATE);
//setPadLED(6, 3, CYAN, NORMAL, BLINK, MODERATE);
//setPadLED(7, 5, OCEAN, BRIGHT, BLINK, MODERATE);
//setPadLED(6, 3, RED, NORMAL, STOP, MODERATE);
//setPadLED(6, 4, CYAN, NORMAL, BLINK, MODERATE);
  delay(2000);
  set8Scale4ths();
  delay(500);
  clearLCDRow(3);
//  writeLCDText2Row(4, 2, "123456789112345678921234567893");
  byte ch1 = 'c';
    byte sedat[] = {240, 71, 127, 21, 27, 0, 2, 0, ch1, 247};
  
  midi01.sendSysEx(9+1, sedat, true);

  writeLCDText2Row(2, 3, "12"); 

}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void myNoteOn(byte channel, byte note, byte velocity) {
      midi01.sendNoteOn(note, // MIDI-note 
                      21 + 1, // MIDI-velocity
                      0*5 + 0 + 1,// MIDI-channel
                      1 // MIDI virtual cable
                      );

  
}

//void setPad(byte pad, byte row, byte col, byte color)
//{
//      // Then simply give the data to the MIDI library send()
//      midilist[cable]->send(type, data1, data2, channel);
//  
//}

//void createGrid(int typebyte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray, byte cable)
//{
//    usbMIDI.send(type, data1, data2, channel, cable);
//}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void sendToComputer(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray, byte cable)
{
    usbMIDI.send(type, data1, data2, channel, cable);
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

//  midi01.setHandleNoteOn(myNoteOn);
//setPadLED(6, 3, RED, NORMAL, STOP, MODERATE);
//setPadLED(6, 4, CYAN, NORMAL, BLINK, MODERATE);
//setPadLED(7, 5, OCEAN, BRIGHT, BLINK, MODERATE);
//setPadLED(1, 1, BW, WHITE, STOP, MODERATE);
//setPadLED(8, 8, BW, WHITE, BLINK, MODERATE);

//  midi01.sendNoteOn(27 + 8*2 + 2, // MIDI-note 
//                    125, // MIDI-velocity
//                    6,1// MIDI-channel
//                   );

//  midi01.sendNoteOn(27 + 8*2 + 2, // MIDI-note 
//                    125, // MIDI-velocity
//                    6// MIDI-channel
//                   );

//void setPadLED(int row, int column, 
//            int color = BW, int shade = NORMAL, 
//            int transition = STOP, int trspeed = MODERATE) {
//    midi01.sendNoteOn(27 + (row*8) + column, // MIDI-note 
//                      color + shade, // MIDI-velocity
//                      transition*5 + trspeed + 1// MIDI-channel
//                      );

//    midi01.sendNoteOn(27 + (5*8) + 7, // MIDI-note 
//                      5 + 1, // MIDI-velocity
//                      6*5 + 0 + 1,// MIDI-channel
//                      1 // MIDI virtual cable
//                      );
//    setPadLED(4,6,5,1,1,0);

//  if(firstrun) {
//    setPadLED(7,7,5,1,1,0);
////    set8Scale4ths();
//    firstrun = false;
//  }
  
}
