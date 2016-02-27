# BrennenstuhlRCS1000N
A small Arduino library for Brennenstuhl RCS 1000N RF-switches and compatible devices.
Works for me with Arduino and ESP8266.


### Quick Start

```cpp
#include <BrennenstuhlRCS1000N.h>

#if defined(ESP8266)
  #define PIN_RFSWITCH_RECV  D2
  #define PIN_RFSWITCH_SEND  D4
#else
  #define PIN_RFSWITCH_RECV  2
  #define PIN_RFSWITCH_SEND  4
#endif

// connect rf-switch
BrennenstuhlRCS1000N rfSwitch(PIN_RFSWITCH_SEND, PIN_RFSWITCH_RECV); // or use BrennenstuhlRCS1000N::NO_PIN 

void setup() {
  rfSwitch.begin();
}

void loop() {
}
```

###API - BrennenstuhlRCS1000N

