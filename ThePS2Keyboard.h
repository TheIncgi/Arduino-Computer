#ifndef Theincgi_ThePS2Keyboard_H
#define Theincgi_ThePS2Keyboard_H

#include <Arduino.h>
#include "CircularByteBuffer.h"

class ThePS2Keyboard {
  private:
    enum Status { RECEIVING, TRANSMITTING, DISABLE, IDLE };
  public:
    /**Clock out must be an interrupt pin*/
    static void begin(byte clockPin, byte dataPin);
    static void end();
    static void send(byte b);

    static short available();
    static byte read();
    static byte peek();
    static bool isCapsLock();
    static bool isNumLock();
    static bool isScrollLock();
    void setCapsLock(bool state);
    void setNumLock(bool state);
    void setScrollLock(bool state);
    void updateLEDs();
  private:
    static byte clockPin, dataPin;
    volatile static int8_t interruptCounter;
    volatile static byte interruptBuffer;
    static void onInterrupt();
    volatile static bool parity;
    volatile static bool comError;
    static CircularByteBuffer rx;
    volatile static byte tx;
    //if the board is sending data to the keyboard
    //disables action on key Interrupt
    volatile static bool isSending;
    volatile static Status status;
    volatile static long timeout;
    static bool numLock, capsLock, scrollLock;
};

#endif
