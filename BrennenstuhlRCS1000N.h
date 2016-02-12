/*
  Brennenstuhl RCS 1000 N RF Switch
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

#if not defined(__BrennenstuhlRCS1000N_h__)
#define __BrennenstuhlRCS1000N_h__

#include <Arduino.h>

#define BrennenstuhlRCS1000N_CODE(senderId, deviceId, command) \
          (((((int)senderId) & 0b11111) << 7) | ((((int)deviceId) & 0b11111) << 2) | (((int)command) & 0b11))
          
#define BrennenstuhlRCS1000N_CODE_ON(senderId, deviceId) \
          BrennenstuhlRCS1000N_CODE(senderId, deviceId, BrennenstuhlRCS1000N::SWITCH_ON)

#define BrennenstuhlRCS1000N_CODE_OFF(senderId, deviceId) \
          BrennenstuhlRCS1000N_CODE(senderId, deviceId, BrennenstuhlRCS1000N::SWITCH_OFF)


class BrennenstuhlRCS1000N {
  public:
    BrennenstuhlRCS1000N(int pinSND, int pinRCV);


  public:
    void begin();
    void end();
    void loop();


  public:
    bool send(int code, int repeat, int pre_delay);
    inline bool sendSwitchOn(int senderId, int deviceId) __attribute__((always_inline)) {
      return send(BrennenstuhlRCS1000N_CODE_ON(senderId, deviceId), _send_repeat, _send_pre_delay);
    };
    inline bool sendSwitchOff(int senderId, int deviceId) __attribute__((always_inline)) {
      return send(BrennenstuhlRCS1000N_CODE_OFF(senderId, deviceId), _send_repeat, _send_pre_delay);
    };


  private:
    int _send_repeat = 6; // *** resend 6 times ***
  public:
    inline int getSendRepeat() const __attribute__((always_inline)) { return _send_repeat; };
    inline int setSendRepeat(int repeat) __attribute__((always_inline)) { _send_repeat = repeat; };
    
  private:
    int _send_pre_delay = 10; // *** 10 ms ***
  public:
    inline int getSendPreDelay() const __attribute__((always_inline)) { return _send_pre_delay; };
    inline int setSendPreDelay(int pre_delay) __attribute__((always_inline)) { _send_pre_delay = pre_delay; };


  public:
    bool recv(int* code_value, int* code_repeat = NULL);
    inline int getRecvSenderId(int code_value) __attribute__((always_inline)) { return ((code_value >> 7) & 0b11111); };
    inline int getRecvDeviceId(int code_value) __attribute__((always_inline)) { return ((code_value >> 2) & 0b11111); };
    inline int getRecvCommand(int code_value)  __attribute__((always_inline)) { return (code_value & 0b11); };

    int getRecvPulseWidthUS();
    
  private:
    int _recv_timeout = 250;
  public:
    inline int getRecvTimeout() const __attribute__((always_inline))           { return _recv_timeout; };
    inline int setRecvTimeout(int recv_timeout) __attribute__((always_inline)) { _recv_timeout = recv_timeout; };


  public:
    static const int NO_PIN     = 0xff;
    static const int SWITCH_ON  = 0b10;
    static const int SWITCH_OFF = 0b01;
};

#endif
