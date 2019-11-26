#include "CircularByteBuffer.h"

CircularByteBuffer::CircularByteBuffer( short size = 0 ){
  this->data = new byte[ size ];
  this->cursor = 0;
  this->limit = size;
  this->used = 0;
}

CircularByteBuffer::~CircularByteBuffer(){
  delete data;
}

byte CircularByteBuffer::peek(){
  if(this->used == 0) return 0;
  return this->data[ this->cursor ];
}
byte CircularByteBuffer::poll(){
  if(this->used == 0) return 0;
  byte temp = this->peek();
  this->cursor = (this->cursor+1) % this->limit;
  this->used--;
  return temp;
}
void CircularByteBuffer::offer( byte b ){
  this->used = min( this->limit, this->used + 1 );
  short writePos = (this->cursor + this->used - 1) % this->limit;
  this->data[ writePos ] = b;
}
short CircularByteBuffer::available(){
  return this->used;
}



//void PS2Keyboard::debug(){
//  Serial.print("Queue: ");
//  Serial.print(eventBuffer.size);
//  Serial.print(" {");
//  for(byte i = 0; i< eventBuffer.limit; i++){
//    byte j = (i+eventBuffer.cursor) % eventBuffer.limit;
//    if( j == (eventBuffer.cursor + eventBuffer.size) % eventBuffer.limit )
//      Serial.print(" || ");
//    Serial.print( eventBuffer.buffer[j], HEX);
//    Serial.print( ", ");
//  }
//  Serial.println();
//}
