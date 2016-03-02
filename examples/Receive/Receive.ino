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

#define RFSWITCH_A      0b10000
#define RFSWITCH_B      0b01000
#define RFSWITCH_C      0b00100
#define RFSWITCH_D      0b00010
#define RFSWITCH_E      0b00001

#if defined(ESP8266)
  #define PIN_RFSWITCH_SEND  D4  // BrennenstuhlRCS1000N::NO_PIN
  #define PIN_RFSWITCH_RECV  D2
#else
  #define PIN_RFSWITCH_SEND   4  // BrennenstuhlRCS1000N::NO_PIN
  #define PIN_RFSWITCH_RECV   2
#endif

BrennenstuhlRCS1000N rfSwitch(PIN_RFSWITCH_SEND, PIN_RFSWITCH_RECV);


char* toBIN(uint32_t value, uint8_t length);


void setup() {
  pinMode(PIN_RFSWITCH_SEND, OUTPUT); digitalWrite(PIN_RFSWITCH_SEND, LOW);
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
    int senderId = rfSwitch.getRecvSenderId(code);  // sender id
    int deviceId = rfSwitch.getRecvDeviceId(code);  // device/switch id
    int command  = rfSwitch.getRecvCommand(code);   // command on, off
    
    Serial.println(F("## ---"));
    Serial.print(F("## code = 0b")); Serial.print(code, BIN); Serial.print(F(", repeat = ")); Serial.print(repeat, DEC); Serial.println();
    Serial.print(F("## senderId = 0b")); Serial.print(toBIN(senderId, 5)); 
    Serial.print(F(", deviceId = 0b"));  Serial.print(toBIN(deviceId, 5));
    Serial.print(F(", command = 0b"));   Serial.print(toBIN(command , 2));
    switch(command){ case BrennenstuhlRCS1000N::SWITCH_ON:{Serial.print(F(" (ON)"));} break; case BrennenstuhlRCS1000N::SWITCH_OFF:{Serial.print(F(" (OFF)"));} break; }
    Serial.print(F(", pulse width = ")); Serial.print(rfSwitch.getRecvPulseWidthUS(), DEC); Serial.print(F(" us"));
    Serial.println();
  }

  rfSwitch.loop();
}


char* toBIN(uint32_t value, uint8_t length) {
  static char s[(8 * sizeof(value)) + 1];
  int i = 8 * sizeof(value); 
  
  if (length > i ) length = i;

  s[i] = 0;
  while (length--) {
    s[--i] = (value & 1) ? '1' : '0';
    value >>= 1;
  }
  
  return &s[i];
}