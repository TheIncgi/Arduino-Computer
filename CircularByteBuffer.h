#ifndef CircularByteBuffer_H
#define CircularByteBuffer_H
#include <Arduino.h>

class CircularByteBuffer {
  public:
    CircularByteBuffer(short s);
    ~CircularByteBuffer();
    byte peek();
    byte poll();
    void offer( byte b );
    short available();
    
  private:  
    byte* data;
    short cursor;
    short used;
    short limit;
};

#endif
