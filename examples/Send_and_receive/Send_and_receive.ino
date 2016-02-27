/*
  RF-Switch Brennenstuhl RCS 1000 N example
  uses Arduino library for remote control Brennenstuhl switches.
  
  Copyright (c) 2016 Alexander Schindowski.  All right reserved.
  
  Project home: http://github.com/alexschin/BrennenstuhlRCS1000N

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <BrennenstuhlRCS1000N.h>

#define RFSENDER_ID     0b11111

#define RFSWITCH_A      0b10000
#define RFSWITCH_B      0b01000
#define RFSWITCH_C      0b00100
#define RFSWITCH_D      0b00010
#define RFSWITCH_E      0b00001

#if defined(ESP8266)
  #define PIN_RFSWITCH_SEND  D4  // sending pin
  #define PIN_RFSWITCH_RECV  D2  // receiving pin
#else
  #define PIN_RFSWITCH_SEND  4   // sending pin
  #define PIN_RFSWITCH_RECV  2   // receiving pin
#endif

BrennenstuhlRCS1000N rfSwitch(PIN_RFSWITCH_SEND, PIN_RFSWITCH_RECV);


void setup() {
  Serial.begin(115200); 
#if defined(ESP8266)
	delay(100); Serial.println();
#endif
  
  Serial.println(F("## BrennenstuhlRCS1000N"));
  Serial.print(F("## PIN_RFSWITCH_SEND = ")); Serial.println(PIN_RFSWITCH_SEND, DEC);
  Serial.print(F("## PIN_RFSWITCH_RECV = ")); Serial.println(PIN_RFSWITCH_RECV, DEC);

  rfSwitch.begin();
  /* rfSwitch.setSendRepeat(6);    */ Serial.print(F("## rfSwitch.getSendRepeat()   = ")); Serial.println(rfSwitch.getSendRepeat(), DEC);
  /* rfSwitch.setSendPreDelay(10); */ Serial.print(F("## rfSwitch.getSendPreDelay() = ")); Serial.println(rfSwitch.getSendPreDelay(), DEC);
  /* rfSwitch.setRecvTimeout(250); */ Serial.print(F("## rfSwitch.getRecvTimeout()  = ")); Serial.println(rfSwitch.getRecvTimeout(), DEC);
}


void loop() {
  int code, repeat;

  if (rfSwitch.recv(&code, &repeat)) {
    int senderId = rfSwitch.getRecvSenderId(code);  // sender id
    int deviceId = rfSwitch.getRecvDeviceId(code);  // device/switch id
    int command  = rfSwitch.getRecvCommand(code);   // command on, off
    
    Serial.println(F("## ---"));
    Serial.print(F("## code = 0b")); Serial.print(code, BIN); Serial.print(F(", repeat = ")); Serial.print(repeat, DEC); Serial.println();
    Serial.print(F("## senderId = 0b")); Serial.print(senderId, BIN); 
    Serial.print(F(", deviceId = 0b"));  Serial.print(deviceId, BIN); 
    Serial.print(F(", command = 0b"));   Serial.print(command , BIN); 
    Serial.println();
    Serial.print(F("## getRecvPulseWidthUS = ")); Serial.println(rfSwitch.getRecvPulseWidthUS(), DEC);

    if (deviceId == RFSWITCH_D) {
      switch (command) {
        case BrennenstuhlRCS1000N::SWITCH_ON:  // ON
          Serial.println(F("RFSWITCH_D - ON"));
          rfSwitch.sendSwitchOn(senderId, RFSWITCH_A); 
          rfSwitch.sendSwitchOn(senderId, RFSWITCH_B); 
          rfSwitch.sendSwitchOn(senderId, RFSWITCH_C); 
          break;
        case BrennenstuhlRCS1000N::SWITCH_OFF: // OFF
          Serial.println(F("RFSWITCH_D - OFF"));
          rfSwitch.sendSwitchOff(senderId, RFSWITCH_A); 
          rfSwitch.sendSwitchOff(senderId, RFSWITCH_B); 
          rfSwitch.sendSwitchOff(senderId, RFSWITCH_C); 
          break;
      }
    }

  }

  rfSwitch.loop();
}
