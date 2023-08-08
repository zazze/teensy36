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

#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi01(myusb);
MIDIDevice midi02(myusb);
MIDIDevice * midilist[2] = {&midi01, &midi02};

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;


void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT); // LED pin
  digitalWrite(13, LOW);
  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  Serial.println("2 USB Midi Example");
  delay(10);
  myusb.begin();
}


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
    byte cable = usbMIDI.getCable();

    // forward this message to 1 of the 3 Serial MIDI OUT ports
    if (type != usbMIDI.SystemExclusive) {

      // Then simply give the data to the MIDI library send()
      midilist[cable]->send(type, data1, data2, channel);

    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      midilist[cable]->sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
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
