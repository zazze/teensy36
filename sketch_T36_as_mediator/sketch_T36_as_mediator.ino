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

#include <MIDI.h>
#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)

// Interval time in ???
const int interval_time = 90;

//const byte Black = 0;  //black
//const byte White = 122; //white
//const byte LightGray = 123; //light gray
//const byte DarkGray = 124; //dark gray
//const byte Blue = 125; //blue
//const byte Green = 126; //green
const byte Black = 0;
const byte Red = 5;
const byte Orange = 9;
const byte Yellow = 13;
const byte Green = 21; //or 17
const byte Magenta = 29;
const byte Blue  = 45; //or 41 or 37
const byte LPurple = 48;
const byte Pink = 53;
const byte Cerise = 57;
const byte Peach = 60;
const byte Lemon = 62;
const byte Lime = 63;
const byte Purple = 69;
const byte White = 70;

const byte O1 = 111;
const byte O2 = 112;
const byte O3 = 113;
const byte O4 = 114;
const byte O5 = 115;

const byte YG = 116;

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)
USBHost push1host;
USBHub hub1(&push1host);
USBHub hub2(&push1host);
MIDIDevice push1midiuser(push1host);
MIDIDevice push1midilive(push1host);

byte pattern[128];

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;

elapsedMillis clock_count;
byte step_count;
byte round_count;

void setup() {
    pinMode(13, OUTPUT); // LED pin
    digitalWrite(13, LOW);
    
    for(int i = 0; i < 8; i ++) {
      pinMode(i, OUTPUT); 
      digitalWrite(i, LOW);
    }

  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  push1host.begin();
  push1midiuser.sendNoteOn(27 + (5*8) + 6, Red, 1,1);
//  push1hostmidi.setHandleNoteOn(myNoteOn);
//  push1hostmidi.setHandleNoteOff(myNoteOff);
//  push1hostmidi.setHandleControlChange(myControlChange);

}


void loop() {

      // Then simply give the data to the MIDI library send()
//  push1hostmidi.send(type, data1, data2, channel);

  push1midiuser.sendNoteOn(27 + 5*8 + 6, Red, 0);
  push1midilive.sendNoteOn(27 + (5*8) + 6, Red, 0);
  push1midiuser.sendControlChange(59, 5, 0);
  push1midilive.sendControlChange(59, 4, 0);
//  setGrid(byte col, byte row, byte colour) {
//  push1hostmidi.sendNoteOn(27 + row*8 + col, colour, 1);
//}

//  push1host.Task();
//  push1midiuser.read();

//  // Sequencer
//  if (clock_count >= interval_time) {
//    doStep();
//    clock_count = 0;
//  }

  delay(1);
  
  bool activity = false;

//  // Next read messages arriving from the USB device plugged into the USB Host port
//    
//    if (push1midiuser.read()) {
//      uint8_t type =       push1midiuser.getType();
//      uint8_t data1 =      push1midiuser.getData1();
//      uint8_t data2 =      push1midiuser.getData2();
//      uint8_t channel =    push1midiuser.getChannel();
//      const uint8_t *sys = push1midiuser.getSysExArray();
//      sendToComputer(type, data1, data2, channel, sys, 0);
//      activity = true;
//    }
//
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
      push1midiuser.send(type, data1, data2, channel);

    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      push1midiuser.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
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


void sendToComputer(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray, byte cable)
{
    usbMIDI.send(type, data1, data2, channel, cable);
}

//Adapted from Little-scale
void myNoteOn(byte channel, byte note, byte velocity) {
  if (note % 16 < 8) {
    pattern[note] = (pattern[note] + 1) % 4;

    if (pattern[note] == 1) {
      setGrid(note % 16, note / 16, Red);
    } else {
      setGrid(note % 16, note / 16, Black);
    }

  }

}

void myNoteOff(byte channel, byte note, byte velocity) {
  // midi1.sendNoteOff(note, velocity, channel);
}

void myControlChange(byte channel, byte control, byte value) {
  push1midiuser.sendControlChange(control, value, channel);
}


void doStep() {
  for (int i = 0; i < 8; i ++) {
    if (pattern[(i * 16) + step_count] == 1) {
      setGrid(step_count, i, Red);
    }
    else if (pattern[(i * 16) + step_count] == 2 && round_count == 0) {
      setGrid(step_count, i, Red);
    }

    else if (pattern[(i * 16) + step_count] == 3 && round_count == 1) {
      setGrid(step_count, i, Red);
    }
    else {
      setGrid(step_count, i, Black);
    }
  }

  step_count ++;
  
  if (step_count > 7) {
    step_count = 0;
    round_count = 1 - round_count;
  }

//  for (int i = 0; i < 8; i ++) {
//    if (pattern[(i * 16) + step_count] == 1) {
//      setGrid(step_count, i, O4);
//      push1midiuser.sendNoteOn((i * 16) + 15, 127, 1);
//      digitalWrite(7 - i , HIGH); 
//    }
//    else if (pattern[(i * 16) + step_count] == 2 && round_count == 0) {
//      setGrid(step_count, i, O4);
//      push1midiuser.sendNoteOn((i * 16) + 15, 127, 1);
//      digitalWrite(7 - i , HIGH);
//    }
//
//    else if (pattern[(i * 16) + step_count] == 3 && round_count == 1) {
//      setGrid(step_count, i, O4);
//      midi1.sendNoteOn((i * 16) + 15, 127, 1);
//      digitalWrite(7 - i , HIGH);
//    }
//
//    else {
//      setGrid(step_count, i, Green);
//      midi1.sendNoteOn((i * 16) + 15, 0, 1);
//      digitalWrite(7 - i , LOW);
//    }
//  }

  delay(3); 

  for (int i = 0; i < 8; i ++) {
    digitalWrite(7 - i , LOW);
  }


}

void setGrid(byte col, byte row, byte colour) {
  push1midiuser.sendNoteOn(27 + row*8 + col, colour, 1);
}
