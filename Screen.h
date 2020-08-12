#ifndef Theincgi_Screen
#define Theincgi_Screen

#include <Arduino.h>
  #define GREEN 0x0400
  #define RED   0xFA49
  #define BLUE  0x4C5F
  #define WHITE 0xFFFF
  #define LIGHT_GRAY 0xA534
namespace Screen {
  void cls();
  void sprint(String s);
  unsigned short fcol(unsigned short col);
  void cursor(unsigned short x, unsigned short y);
  unsigned short peekW(unsigned short addr);
  bool ack();
  unsigned short getCursorX();
  unsigned short getCursorY();
  void serialWait();
  void serialWait(unsigned short m);
  void serialEmpty();
}
#endif
