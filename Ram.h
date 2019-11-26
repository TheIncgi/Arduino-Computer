#ifndef Theincgi_RAM_H
#define Theincgi_RAM_H
#include <Arduino.h>

//long supports  4,294,967,296 addresses (32-bit addressing)
namespace RAM{
  #define PIN_MEM_SBIT 6 //Shift reg data bit //chip select is low for select, high for disable
  #define PIN_MEM_SLCH 7 //Shift reg latch
  #define PIN_MEM_SCLK 8 //shift reg clock
  #define PIN_MEM_MSO 9  //mem chip serial out, we read data here
  #define PIN_MEM_MHD 10 //mem chip hold, suspends transmission to avoid retransmitting full sequence again, held high when not in use
  #define PIN_MEM_MCL 11 //mem chip clock
  #define PIN_MEM_MSI 12 //mem chip serial in
  const unsigned long MAX_CHIP_ADDR = 0x1FFFF; //131,071 inclusive
  
  extern int memoryUnits;
  extern double memSize; //total all addresses, like array len for memory
  void countMemUnits();
  
  //chooses which memory unit to read from
  //used by memWrite/memRead
  void memCS(unsigned int i);
  
  void memSendInstr(byte instr, unsigned long addr);
  
  void memWrite(unsigned long addr, byte val) ;
  void memWrite(unsigned long addr, byte* vals, unsigned int s, unsigned int len);
  void memWriteFill(unsigned long addr, byte value, unsigned long len);
  
  byte memRead(unsigned long addr);
  void memRead(unsigned long addr, byte* buf, unsigned int len);
  void memRead(unsigned long addr, byte* buf, unsigned int s, unsigned int len);
  

  unsigned long memReadUL(unsigned long addr);
  void memWriteUL(unsigned long addr, unsigned long value);
  
  
  //wipe everything
  void memClear();
}

#endif
