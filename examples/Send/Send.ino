/*
  BLINK EXAMPLE
  
  RF-Switch Brennenstuhl RCS 1000 N 
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

#define RFSENDER_ID     0b11111

#define RFSWITCH_A      0b10000
#define RFSWITCH_B      0b01000
#define RFSWITCH_C      0b00100
#define RFSWITCH_D      0b00010
#define RFSWITCH_E      0b00001

#if defined(ESP8266)
  #define PIN_RFSWITCH_SEND  D4
#else
  #define PIN_RFSWITCH_SEND  4
#endif

#include <BrennenstuhlRCS1000N.h>

BrennenstuhlRCS1000N rfSwitch(PIN_RFSWITCH_SEND, BrennenstuhlRCS1000N::NO_PIN);


void setup() {
  Serial.begin(115200); 
  #if defined(ESP8266)
	delay(100); Serial.println();
  #endif
  
  Serial.println(F("## BrennenstuhlRCS1000N"));
  Serial.print(F("## PIN_RFSWITCH_SEND = ")); Serial.println(PIN_RFSWITCH_SEND, DEC);

  rfSwitch.begin();
  /* rfSwitch.setSendRepeat(5);    */ Serial.print(F("## rfSwitch.setSendRepeat()   = ")); Serial.println(rfSwitch.getSendRepeat(), DEC);
  /* rfSwitch.setSendPreDelay(10); */ Serial.print(F("## rfSwitch.setSendPreDelay() = ")); Serial.println(rfSwitch.getSendPreDelay(), DEC);
}


void loop() {
  static unsigned long _last_millis = 0;
  static unsigned long _ms_counter  = 0;
  static bool          switch_state = true;
  
  if (_last_millis != millis()) {
    _last_millis = millis();

	/* DELAY 5 SEC. */
    if (_ms_counter == 0) {
        _ms_counter = 5 * 1000L;
        
        Serial.println( (switch_state) ? F("ON") : F("OFF") );
        switch (switch_state) {
          case true:
            rfSwitch.sendSwitchOn(RFSENDER_ID, RFSWITCH_A);
            rfSwitch.sendSwitchOn(RFSENDER_ID, RFSWITCH_B);
            rfSwitch.sendSwitchOn(RFSENDER_ID, RFSWITCH_C);
            rfSwitch.sendSwitchOn(RFSENDER_ID, RFSWITCH_D);
            break;
          case false:
            rfSwitch.sendSwitchOff(RFSENDER_ID, RFSWITCH_A);
            rfSwitch.sendSwitchOff(RFSENDER_ID, RFSWITCH_B);
            rfSwitch.sendSwitchOff(RFSENDER_ID, RFSWITCH_C);
            rfSwitch.sendSwitchOff(RFSENDER_ID, RFSWITCH_D);
            break;
        }
        switch_state = !!!switch_state;
    } else
      _ms_counter--;
  }

  rfSwitch.loop();
}