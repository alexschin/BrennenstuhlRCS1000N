/*
  RF-Switch Brennenstuhl RCS 1000 N
  Arduino library for remote control Brennenstuhl switches.
  
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

#if not defined(__BrennenstuhlRCS1000N_cpp__)
#define __BrennenstuhlRCS1000N_cpp__

#include <Arduino.h>
#include "BrennenstuhlRCS1000N.h"


// *** PINS ***
static int8_t _pinSND = BrennenstuhlRCS1000N::NO_PIN;
static int8_t _pinRCV = BrennenstuhlRCS1000N::NO_PIN;


// *** PULSE WIDTH ***
#define PULSE_WIDTH_US       300
#define PULSE_WIDTH_US_DELTA (PULSE_WIDTH_US / 4)

static inline bool isPulseShort(uint16_t us) __attribute__((always_inline));
static inline bool isPulseShort(uint16_t us) {
  return (us >= 1 * (PULSE_WIDTH_US - PULSE_WIDTH_US_DELTA)) && (us <= 1 * (PULSE_WIDTH_US + PULSE_WIDTH_US_DELTA));
}

static inline bool isPulseLong(uint16_t us) __attribute__((always_inline));
static inline bool isPulseLong(uint16_t us) {
  return (us >= 3 * (PULSE_WIDTH_US - PULSE_WIDTH_US_DELTA)) && (us <= 3 * (PULSE_WIDTH_US + PULSE_WIDTH_US_DELTA));
}


// *** TIMINGS BUFFER ***
#define TIMINGS_MAX ((12/*code length*/ * 4/*pulses per pit*/) + 2/*extra space*/)
static volatile int16_t timings[TIMINGS_MAX];
static volatile int8_t  timings_position = 0;

static inline void timings_push(int v) __attribute__((always_inline));
static inline void timings_push(int v) {
  timings[timings_position++] = v;
  if (timings_position >= TIMINGS_MAX) timings_position = 0;
}

static inline int timings_pop() __attribute__((always_inline));
static inline int timings_pop() {
  timings_position = (timings_position != 0) ? (timings_position - 1) : (TIMINGS_MAX - 1);
  return timings[timings_position];
}

static inline int timings_clear() __attribute__((always_inline));
static inline int timings_clear() {
  timings_position = 0;
  memset((void*)timings, 0, sizeof(timings));
}


// *** SEND BUFFER ***
#define SEND_BUFFER_SIZE  15
static struct SEND_BUFFER_ITEM {
  unsigned int code;
  unsigned int repeat;
  unsigned int preDelay;
} _send_buffer[SEND_BUFFER_SIZE];


// *** RECIEVE BUFFER ***
#define RECV_BUFFER_SIZE 5
static struct RECV_BUFFER_ITEM {
  unsigned int code;
  unsigned int repeat;
} _recv_buffer[RECV_BUFFER_SIZE];


#define RECV_STATE_DISABLED  0x01
#define RECV_STATE_NEWCODE   0x02

static volatile uint16_t _rcv_newcode = 0;
static volatile uint8_t  _rcv_state   = 0;
static volatile int      _rcv_pulsewidthUS = 0;

int BrennenstuhlRCS1000N::getRecvPulseWidthUS() { return _rcv_pulsewidthUS; };

static void rcv_handleInterrupt() {
  static volatile uint32_t lastMicros = 0;

  if (!_rcv_state) {
    uint32_t nowMicros = micros();
    uint32_t duration = nowMicros - lastMicros;

    if (duration > 5000) {
      int pulsewidth = timings_pop();
      if (isPulseShort(pulsewidth)) {
        _rcv_pulsewidthUS = pulsewidth;
        _rcv_state |= RECV_STATE_NEWCODE;
      } else {
        timings_push(0);
      }
    } else {
      timings_push(duration);
    }
    
    lastMicros = nowMicros;
  }
}

static void rcv_enable() {
  _rcv_state &= ~RECV_STATE_DISABLED;
  if (_pinRCV != BrennenstuhlRCS1000N::NO_PIN) {
    attachInterrupt(digitalPinToInterrupt(_pinRCV), rcv_handleInterrupt, CHANGE);
  }
}

static void rcv_disable() {
  _rcv_state |= RECV_STATE_DISABLED;
  if (_pinRCV != BrennenstuhlRCS1000N::NO_PIN) {
    detachInterrupt(digitalPinToInterrupt(_pinRCV));
  }
}


static bool rcv_getReceivedValue(unsigned int* value) {
  if (!(_rcv_state & RECV_STATE_NEWCODE))
    return false;

  rcv_disable();
  unsigned int code = 0;
  
  for (unsigned char i = 0; i < 12; i++) {
    int p3 = timings_pop();
    int p2 = timings_pop();
    int p1 = timings_pop();
    int p0 = timings_pop();

    code >>= 1;
    if (isPulseShort(p0) && isPulseLong(p1)) {
      if (isPulseShort(p2)) {
        code |= 2048;
      } else if (!isPulseLong(p2)) {
        code = 0;
        break;
      }
    } else {
      code = 0;
      break;
    }
  }
  
  *value = code;  
  timings_push(0);
  
  _rcv_state &= ~RECV_STATE_NEWCODE;
  rcv_enable();

  return code != 0;
}


static inline void _send_on() __attribute__((always_inline));
static inline void _send_on() {
  digitalWrite(_pinSND, HIGH);
  delayMicroseconds(PULSE_WIDTH_US * 1);
  digitalWrite(_pinSND, LOW);
  delayMicroseconds(PULSE_WIDTH_US * 3);
}

static inline void _send_off() __attribute__((always_inline));
static inline void _send_off() {
  digitalWrite(_pinSND, HIGH);
  delayMicroseconds(PULSE_WIDTH_US * 3);
  digitalWrite(_pinSND, LOW);
  delayMicroseconds(PULSE_WIDTH_US * 1);
}

static inline void _send_sync() __attribute__((always_inline));
static inline void _send_sync() {
  digitalWrite(_pinSND, HIGH);
  delayMicroseconds(PULSE_WIDTH_US * 1);
  digitalWrite(_pinSND, LOW);
  delayMicroseconds(PULSE_WIDTH_US * 31);
}

static void _send(uint16_t code) {
  if (_pinSND != BrennenstuhlRCS1000N::NO_PIN) {
    rcv_disable();
    _send_on(); if (code & 2048) { _send_on(); } else { _send_off(); }
    _send_on(); if (code & 1024) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &  512) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &  256) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &  128) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &   64) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &   32) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &   16) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &    8) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &    4) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &    2) { _send_on(); } else { _send_off(); }
    _send_on(); if (code &    1) { _send_on(); } else { _send_off(); }
    _send_sync();
    rcv_enable();
  }
}





BrennenstuhlRCS1000N::BrennenstuhlRCS1000N(int pinSND, int pinRCV) {
  _pinSND = pinSND;
  _pinRCV = pinRCV;
}

void BrennenstuhlRCS1000N::begin() {
  memset(_recv_buffer, 0, sizeof(_recv_buffer));
  memset(_send_buffer, 0, sizeof(_send_buffer));

  timings_position = 0;
  memset((void*)timings, 0, sizeof(timings));
  
  if (_pinSND != NO_PIN) {
    pinMode(_pinSND, OUTPUT);
    digitalWrite(_pinSND, LOW);
  }

  if (_pinRCV != NO_PIN) {
    pinMode(_pinRCV, INPUT_PULLUP);
    rcv_enable();
  }
}

void BrennenstuhlRCS1000N::end() {
  if (_pinRCV != NO_PIN) {
    rcv_disable();
    pinMode(_pinRCV, INPUT);
    _pinRCV = NO_PIN;
  }
  memset(_send_buffer, 0, sizeof(_send_buffer));
  
  if (_pinSND != NO_PIN) {
    digitalWrite(_pinSND, LOW);
    pinMode(_pinSND, INPUT);
    _pinSND = NO_PIN;
  }
  memset(_recv_buffer, 0, sizeof(_recv_buffer));
}


static uint16_t _last_recv_code = 0;
static uint16_t _last_recv_code_repeat = 0;
static uint16_t _last_recv_code_delay  = 0;


void BrennenstuhlRCS1000N::loop() {
  static unsigned long _last_millis = 0;

  if (_pinSND != NO_PIN) {
	  if (_send_buffer[0].repeat != 0) {
      if (_last_millis != millis()) {
        
        if (_send_buffer[0].preDelay != 0) {
          _send_buffer[0].preDelay--;
        } else {
          _send(_send_buffer[0].code);
        
          if ((--_send_buffer[0].repeat) == 0) {            
            for (uint8_t i = 1; i < SEND_BUFFER_SIZE; i++) {
              _send_buffer[i - 1] = _send_buffer[i];
              if (_send_buffer[i].repeat == 0)
                break;
              _send_buffer[i].repeat = 0;
            }        
          }
        }

        _last_millis = millis();
      }
      return;
	  }
  }

  if (_pinRCV != NO_PIN) {
	  unsigned int _recv_code;
	  
	  if (rcv_getReceivedValue(&_recv_code)) {
      if ((_recv_code & 0b111110000000) != 0 && (_recv_code & 0b000001111100) != 0 && (_recv_code & 0b000000000011) != 0) {
        if (_last_recv_code != _recv_code) {
          _last_recv_code = _recv_code;
          _last_recv_code_repeat = 1;
        } else {
          _last_recv_code_repeat += 1;
        }
      }
      _last_recv_code_delay = _recv_timeout;
      return;
	  } 
	  
	  if (_last_millis != millis()) {
      if (_last_recv_code_delay != 0 && (--_last_recv_code_delay) == 0) {
        if (_last_recv_code != 0 && _last_recv_code_repeat >= 1) {
          for (uint8_t i = 0; i < RECV_BUFFER_SIZE; i++) {
            struct RECV_BUFFER_ITEM* itm = &(_recv_buffer[i]);
            if (itm->code == 0) {
              itm->code   = _last_recv_code;
              itm->repeat = _last_recv_code_repeat;
              break;
            }
          }
          
          _last_recv_code = _last_recv_code_repeat = 0;
        }
      }
      
      _last_millis = millis();
      return;
	  }
  }
}


bool BrennenstuhlRCS1000N::send(int code, int repeat, int pre_delay) {
  for (uint8_t i = 0; i < SEND_BUFFER_SIZE; i++) {
    struct SEND_BUFFER_ITEM* itm = &(_send_buffer[i]);
    
    if (itm->code == code)
      return true;
    else if (itm->repeat == 0) {
      itm->code     = code;
      itm->repeat   = repeat;
      itm->preDelay = pre_delay;
      return true;
    }
  }
  
  return false;
}



bool BrennenstuhlRCS1000N::recv(int* code_value, int* code_repeat) {
  if (_recv_buffer[0].code == 0)
    return false;
  
  if (code_value != NULL)  *code_value  = _recv_buffer[0].code;
  if (code_repeat != NULL) *code_repeat = _recv_buffer[0].repeat;

  for (uint8_t i = 1; i < RECV_BUFFER_SIZE; i++) {
    _recv_buffer[i - 1] = _recv_buffer[i];
    if (_recv_buffer[i].code == 0)
      break;
    _recv_buffer[i].code = 0;
  }

  return true;
}

#endif
