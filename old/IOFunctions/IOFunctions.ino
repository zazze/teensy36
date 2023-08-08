/* Receive Incoming USB Host MIDI using functions.  As usbMIDI
   reads incoming messages, handler functions are run.
   See the InputRead example for the non-function alterative.

   This very long example demonstrates all possible handler
   functions.  Most applications need only some of these.
   This example is meant to allow easy copy-and-paste of the
   desired functions.

   Use the Arduino Serial Monitor to view the messages
   as Teensy receives them by USB MIDI

   You must select MIDI from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

/*
 * For Ableton Push, cable 0 is the Live port and cable 1 is the User port
 */

#include <USBHost_t36.h>

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

// Transition mode
#define STOP 0
#define RAMP 0
#define PULSE 1
#define BLINK 2
// Transition speed
#define VERYFAST 1
#define FAST 2
#define MODERATE 3
#define SLOW 4
#define VERYSLOW 5

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);


void setup() {
  Serial.begin(115200);

  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  Serial.println("USB Host Input and Output Functions example");
  delay(10);
  myusb.begin();

  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandleNoteOff(myNoteOff);
  midi1.setHandleAfterTouchPoly(myAfterTouchPoly);
  midi1.setHandleControlChange(myControlChange);
  midi1.setHandleProgramChange(myProgramChange);
  midi1.setHandleAfterTouchChannel(myAfterTouchChannel);
  midi1.setHandlePitchChange(myPitchChange);
  // Only one of these System Exclusive handlers will actually be
  // used.  See the comments below for the difference between them.
  midi1.setHandleSystemExclusive(mySystemExclusiveChunk);
  midi1.setHandleSystemExclusive(mySystemExclusive); 
  midi1.setHandleTimeCodeQuarterFrame(myTimeCodeQuarterFrame);
  midi1.setHandleSongPosition(mySongPosition);
  midi1.setHandleSongSelect(mySongSelect);
  midi1.setHandleTuneRequest(myTuneRequest);
  midi1.setHandleClock(myClock);
  midi1.setHandleStart(myStart);
  midi1.setHandleContinue(myContinue);
  midi1.setHandleStop(myStop);
  midi1.setHandleActiveSensing(myActiveSensing);
  midi1.setHandleSystemReset(mySystemReset);
  // This generic System Real Time handler is only used if the
  // more specific ones are not set.
  midi1.setHandleRealTimeSystem(myRealTimeSystem);
}

void loop() {
  // The handler functions are called when midi1 reads data.  They
  // will not be called automatically.  You must call midi1.read()
  // regularly from loop() for midi1 to actually read incoming
  // data and run the handler functions as messages arrive.
  myusb.Task();
  midi1.read();
}


void setLED(int row, int column, 
            int color, int shade, 
            int transition = STOP, int trdur = STOP) {
    midi1.sendNoteOn(27 + (row*8) + column, 
                      color + shade,
                      transition*5 + trdur + 1,
                      1);
}

// This 3-input System Exclusive function is more complex, but allows you to
// process very large messages which do not fully fit within the midi1's
// internal buffer.  Large messages are given to you in chunks, with the
// 3rd parameter to tell you which is the last chunk.  This function is
// a Teensy extension, not available in the Arduino MIDI library.
//
void setLCDline(int row, const char * txt) {
  
  int ssx[77] = {240, 71, 127 , 21 , 24 , 0 , 69 , 0, [76] = 247};
  int i;
   for(i = 0; i < 68; i++)
   {
       ssx[8+i] = txt[i];
   }
  
}
  mySystemExclusiveChunk(const byte *data, uint16_t length, bool last) {
  Serial.print("SysEx Message: ");
  printBytes(data, length);
  if (last) {
    Serial.println(" (end)");
  } else {
    Serial.println(" (to be continued)");
  }
}

void setup() {
  char input[] = "005000720065010D006F00200074006F0020006E0065006601480075006A006701610069006A0065";
  char output[sizeof(input) / SUBSTRINGSPLIT + 1];   // Don't forget NULL at end of string
  char temp[5];
  int length;
  int index = 0;
  int i;

  Serial.begin(9600);
  
  length = strlen(input);
  for (i = 0; i < length; i += SUBSTRINGSPLIT, index++) {
    strncpy(temp, &input[i], SUBSTRINGSPLIT);
    Serial.print(temp);
    output[index] = atoi(temp);
    Serial.print("   ");
    Serial.println((char) output[index]);
  }
}

void myNoteOn(byte channel, byte note, byte velocity) {
  // When a USB device with multiple virtual cables is used,
  // midi1.getCable() can be used to read which of the virtual
  // MIDI cables received this message.
  int cableno = midi1.getCable();
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
  Serial.print(", cable=");
  Serial.println(cableno, DEC);

//  setLED(3,1,BLUE,BRIGHT,STOP, STOP);
//  setLED(3,2,BLUE,NORMAL,STOP, STOP);
//  setLED(3,3,BLUE,DIM,STOP, STOP);
//  setLED(3,4,BLUE,MIX,STOP, STOP);
//  setLED(3,5,BLUE,MIX);
//  setLED(3,6,ORCHID,BRIGHT);
//  setLED(3,7,ORCHID,NORMAL);
//  setLED(3,8,ORCHID,DIM);
//
//  setLED(4,1,BLUE,BRIGHT,PULSE, VERYSLOW);
//  setLED(4,2,BLUE,NORMAL,PULSE, SLOW);
//  setLED(4,3,BLUE,DIM,PULSE, MODERATE);
//  setLED(4,4,BLUE,MIX,PULSE, FAST);
//  setLED(4,5,BLUE,MIX, PULSE, VERYFAST);
//  setLED(4,6,ORCHID,BRIGHT,PULSE, VERYSLOW);
//  setLED(4,7,ORCHID,NORMAL,PULSE, VERYSLOW);
//  setLED(4,8,ORCHID,DIM,PULSE, VERYSLOW);
//
//  setLED(5,1,BLUE,BRIGHT,BLINK, VERYSLOW);
//  setLED(5,2,BLUE,NORMAL,BLINK, SLOW);
//  setLED(5,3,BLUE,DIM,BLINK, MODERATE);
//  setLED(5,4,BLUE,MIX,BLINK, FAST);
//  setLED(5,5,BLUE,MIX, BLINK, VERYFAST);
//  setLED(5,6,ORCHID,BRIGHT,BLINK, VERYSLOW);
//  setLED(5,7,ORCHID,NORMAL,BLINK, VERYSLOW);
//  setLED(5,8,ORCHID,DIM,BLINK, VERYSLOW);

}

void myNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void myAfterTouchPoly(byte channel, byte note, byte velocity) {
  Serial.print("AfterTouch Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void myControlChange(byte channel, byte control, byte value) {
  int cableno = midi1.getCable();
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.println(value, DEC);
  Serial.print(", cable=");
  Serial.println(cableno, DEC);

}

void myProgramChange(byte channel, byte program) {
  Serial.print("Program Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", program=");
  Serial.println(program, DEC);
}

void myAfterTouchChannel(byte channel, byte pressure) {
  Serial.print("After Touch, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pressure=");
  Serial.println(pressure, DEC);
}

void myPitchChange(byte channel, int pitch) {
  Serial.print("Pitch Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", pitch=");
  Serial.println(pitch, DEC);
}


// This 3-input System Exclusive function is more complex, but allows you to
// process very large messages which do not fully fit within the midi1's
// internal buffer.  Large messages are given to you in chunks, with the
// 3rd parameter to tell you which is the last chunk.  This function is
// a Teensy extension, not available in the Arduino MIDI library.
//
void mySystemExclusiveChunk(const byte *data, uint16_t length, bool last) {
  Serial.print("SysEx Message: ");
  printBytes(data, length);
  if (last) {
    Serial.println(" (end)");
  } else {
    Serial.println(" (to be continued)");
  }
}

// This simpler 2-input System Exclusive function can only receive messages
// up to the size of the internal buffer.  Larger messages are truncated, with
// no way to receive the data which did not fit in the buffer.  If both types
// of SysEx functions are set, the 3-input version will be called by midi1.
//
void mySystemExclusive(byte *data, unsigned int length) {
  Serial.print("SysEx Message: ");
  printBytes(data, length);
  Serial.println();
}

void myTimeCodeQuarterFrame(byte data) {
  static char SMPTE[8]={'0','0','0','0','0','0','0','0'};
  static byte fps=0;
  byte index = data >> 4;
  byte number = data & 15;
  if (index == 7) {
    fps = (number >> 1) & 3;
    number = number & 1;
  }
  if (index < 8 || number < 10) {
    SMPTE[index] = number + '0';
    Serial.print("TimeCode: ");  // perhaps only print when index == 7
    Serial.print(SMPTE[7]);
    Serial.print(SMPTE[6]);
    Serial.print(':');
    Serial.print(SMPTE[5]);
    Serial.print(SMPTE[4]);
    Serial.print(':');
    Serial.print(SMPTE[3]);
    Serial.print(SMPTE[2]);
    Serial.print('.');
    Serial.print(SMPTE[1]);  // perhaps add 2 to compensate for MIDI latency?
    Serial.print(SMPTE[0]);
    switch (fps) {
      case 0: Serial.println(" 24 fps"); break;
      case 1: Serial.println(" 25 fps"); break;
      case 2: Serial.println(" 29.97 fps"); break;
      case 3: Serial.println(" 30 fps"); break;
    }
  } else {
    Serial.print("TimeCode: invalid data = ");
    Serial.println(data, HEX);
  }
}

void mySongPosition(uint16_t beats) {
  Serial.print("Song Position, beat=");
  Serial.println(beats);
}

void mySongSelect(byte songNumber) {
  Serial.print("Song Select, song=");
  Serial.println(songNumber, DEC);
}

void myTuneRequest() {
  Serial.println("Tune Request");
}

void myClock() {
  Serial.println("Clock");
}

void myStart() {
  Serial.println("Start");
}

void myContinue() {
  Serial.println("Continue");
}

void myStop() {
  Serial.println("Stop");
}

void myActiveSensing() {
  Serial.println("Actvice Sensing");
}

void mySystemReset() {
  Serial.println("System Reset");
}

void myRealTimeSystem(uint8_t realtimebyte) {
  Serial.print("Real Time Message, code=");
  Serial.println(realtimebyte, HEX);
}



void printBytes(const byte *data, unsigned int size) {
  while (size > 0) {
    byte b = *data++;
    if (b < 16) Serial.print('0');
    Serial.print(b, HEX);
    if (size > 1) Serial.print(' ');
    size = size - 1;
  }
}
