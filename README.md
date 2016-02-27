# BrennenstuhlRCS1000N
A small Arduino library for Brennenstuhl RCS 1000N RF-switches and compatible devices.
Works for me with Arduino and ESP8266.


### Quick Start

```cpp
#include <BrennenstuhlRCS1000N.h>

#if defined(ESP8266)
  #define SENDER_PIN    D2
  #define RECEIVER_PIN  D4
#else
  #define SENDER_PIN    2
  #define RECEIVER_PIN  4
#endif

// connect rf-switch
BrennenstuhlRCS1000N rfSwitch(SENDER_PIN, RECEIVER_PIN); // use BrennenstuhlRCS1000N::NO_PIN
                                                         // for no pin usage

void setup() {
  rfSwitch.begin();
}

void loop() {
}
```

###API - BrennenstuhlRCS1000N

