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
  #define PIN_RFSWITCH_RECV  D2
#else
  #define PIN_RFSWITCH_RECV  2
#endif

BrennenstuhlRCS1000N rfSwitch(BrennenstuhlRCS1000N::NO_PIN, PIN_RFSWITCH_RECV);


void setup() {
  ::pinMode(D4, OUTPUT); ::digitalWrite(D4, LOW);
  
  Serial.begin(115200); 
  #if defined(ESP8266)
  delay(100); Serial.println();
  #endif
  
  Serial.println(F("## BrennenstuhlRCS1000N"));
  Serial.print(F("## PIN_RFSWITCH_RECV = ")); Serial.println(PIN_RFSWITCH_RECV, DEC);

  rfSwitch.begin();
  /* rfSwitch.setRecvTimeout(250); */ Serial.print(F("## rfSwitch.getRecvTimeout() = ")); Serial.println(rfSwitch.getRecvTimeout(), DEC);
}


void loop() {
  int code, repeat;

  if (rfSwitch.recv(&code, &repeat)) {
    Serial.println(F("## ---"));
    Serial.print(F("## code = 0b")); Serial.print(code, BIN); Serial.print(F(", repeat = ")); Serial.print(repeat, DEC); Serial.println();
    Serial.print(F("## senderId = 0b")); Serial.print(rfSwitch.getRecvSenderId(code), BIN); Serial.print(F(", deviceId = 0b")); Serial.print(rfSwitch.getRecvDeviceId(code), BIN); Serial.print(F(", command = 0b")); Serial.print(rfSwitch.getRecvCommand(code), BIN); Serial.println();
    Serial.print(F("## getRecvPulseWidthUS = ")); Serial.println(rfSwitch.getRecvPulseWidthUS(), DEC);
  }

  rfSwitch.loop();
}
