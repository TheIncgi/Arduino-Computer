#include "Screen.h"

void Screen::cls(){
  Screen::serialEmpty();
  do{
    Serial3.write(0xFF);
    Serial3.write(0xCD);
    Serial3.flush();
  }while(!ack());
}
void Screen::sprint(String s){
  Screen::serialEmpty();
  unsigned short len = 0;
  unsigned short cx = Screen::getCursorX();
  unsigned short cy = Screen::getCursorY();
  do{
    if(len > 0)
      Screen::cursor(cx, cy);
    do{
      Serial3.write(0x00);
      Serial3.write(0x18);
      Serial3.print(s);
      Serial3.write(0x00);//null terminated string
      Serial3.flush();
    }while(!ack());
    Screen::serialWait(2);
    len = (Serial3.read() << 8) | Serial3.read();
  }while(len!=s.length());
}
unsigned short Screen::fcol(unsigned short col){
  Screen::serialEmpty();
  do{
    Serial3.write(0xFF);
    Serial3.write(0xE7);
    Serial3.write(col>>8);
    Serial3.write(col & 0xFF);
    Serial3.flush();
  }while(!ack());
  Screen::serialWait(2);
  return (Serial3.read() << 8) | Serial3.read();
}
void Screen::cursor(unsigned short x, unsigned short y){
  Screen::serialEmpty();
  do{
    Serial3.write(0xFF);
    Serial3.write(0xE9);
    Serial3.write(y>>8);
    Serial3.write(y&0xFF);
    Serial3.write(x>>8);
    Serial3.write(x&0xFF);
    Serial3.flush();
  }while(!ack());
}
//https://4dsystems.com.au/mwdownloads/download/link/id/127/
unsigned short Screen::peekW(unsigned short addr){
  Screen::serialEmpty();
  do{
    Serial3.write(0x00);
    Serial3.write(0x27);
    Serial3.write(addr>>8);
    Serial3.write(addr&0xFF);
    Serial3.flush();
  }while(!ack());
  return (Serial3.read() << 8) | Serial3.read();
}

bool Screen::ack(){
  Screen::serialWait();
  byte b = Serial3.read();
  if(b == 6){
    return true;
  }
  Screen::serialEmpty();
  return false;
}
unsigned short Screen::getCursorX(){
  return Screen::peekW(0x5F) / Screen::peekW(0x65); //pixel x / font width
}
unsigned short Screen::getCursorY(){
  return Screen::peekW(0x60) / Screen::peekW(0x66); //pixel y / font height
}
void Screen::serialWait(){
  Screen::serialWait(1);
}
void Screen::serialWait(unsigned short m){
  while(Serial3.available()<m)
    delay(10);
}
void Screen::serialEmpty(){
  while(Serial3.available()>0) Serial3.read();
}
