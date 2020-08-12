#ifndef Theincgi_OpCodes_H
#define Theincgi_OpCodes_H
#include <Arduino.h>

namespace Op{
  const byte INSTRUCTION_SIZE = 4; //bytes
  const byte
    MOVE = 0,
    LOADK = 1,
    LOADBOOL = 2,
    LOADNIL = 3;

   //return of 0 indicates variable width
   byte typeSize(char c);
}

#endif
