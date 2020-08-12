#include "OpCodes.h"
#include <Arduino.h>

namespace Op{
  byte typeSize(char c){
    switch(c){
      case 'z': return 1; //boolean
      case 'b': return 1; //byte
      case 'c': return 1; //char
      case 'i': return 2; //int
      case 'I': return 2; //unsigned int
      case 'l': return 4; //long
      case 'L': return 4; //unsigned long
      case '*': return 8; //pointer (channel, address)
//      case '%': return 8; //internal pointer
      case 'a': return 0; //array
      case 's': return 0; //string
      case 'f': return 4; //float
      case 'm': return 12; //map (size, keys heap ptr, values heap ptr)
    }
    Serial.print("Op::typeSize - Missing case ");
    Serial.println(c);
    return 0;
  }
}
