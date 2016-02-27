# BrennenstuhlRCS1000N
A small Arduino library for Brennenstuhl RCS 1000N RF-switches and compatible devices.
Works for me with Arduino and ESP8266.


### Quick Start

```cpp
#include <BrennenstuhlRCS1000N.h>

#define RFSWITCH_A      0b10000
#define RFSWITCH_D      0b00010

#if defined(ESP8266)
  #define SENDER_PIN    D4
  #define RECEIVER_PIN  D2
#else
  #define SENDER_PIN    4
  #define RECEIVER_PIN  2
#endif

// connect rf-switch
BrennenstuhlRCS1000N rfSwitch(SENDER_PIN, RECEIVER_PIN); // use BrennenstuhlRCS1000N::NO_PIN
                                                         // for no pin usage

void setup() {
  #if defined(ESP8266)
    Serial.begin(115200); delay(250); Serial.println();
  #else
    Serial.begin(9600);
  #endif
  
  // initialize library...
  rfSwitch.begin();
}

void loop() {
  int code, repeat;
  if (rfSwitch.recv(&code, &repeat)) {
    if (rfSwitch.getRecvDeviceId(code) == RFSWITCH_D) {
      switch (rfSwitch.getRecvCommand(code)) {
        case BrennenstuhlRCS1000N::SWITCH_ON:  // ON
          Serial.println(F("ON"));
          rfSwitch.sendSwitchOn(rfSwitch.getRecvSenderId(code), RFSWITCH_A); 
          break;
        case BrennenstuhlRCS1000N::SWITCH_OFF: // OFF
          Serial.println(F("OFF"));
          rfSwitch.sendSwitchOff(rfSwitch.getRecvSenderId(code), RFSWITCH_A); 
          break;
      }
    }
    
  }
  rfSwitch.loop();
}
```

###API - BrennenstuhlRCS1000N

