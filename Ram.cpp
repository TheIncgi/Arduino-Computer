#include "Ram.h"

int RAM::memoryUnits = 255;
double RAM::memSize = 0;

void RAM::countMemUnits(){ //could have it read first and restore the value if ram could be added at runtime, but not likely to occur
  long addr = 0;
  byte magic = -74;
  int units = 0;
  while(true){
    Serial.print("CHECK [0x");
    Serial.print(addr, HEX);
    
    memWrite(addr, magic);  //attempt to write a value
    if(memRead(addr) != magic) //check if the value was set
      break;
    memWrite(addr, 0); //reset value to 0
    units++;
    addr += MAX_CHIP_ADDR + 1;
  }
  memoryUnits = units; //global variables
  memSize = ((double)MAX_CHIP_ADDR+1)*memoryUnits;
}

//chooses which memory unit to read from
//used by memWrite/memRead
void RAM::memCS(unsigned int i){
  int boardNum = i >> 3; //bits after first 3 LSB happen to be the 
  int chipNum  = i  & 7; //3 right most bits happen to be the chip num
  for(int b = ceil(memoryUnits/8.0)-1; b>=0; b--){
    digitalWrite(PIN_MEM_SLCH, LOW);
    int j = 0xFF;
    if(boardNum == b && i>=0){
      j = 0xFF ^ (1<<chipNum); //turn off the single bit for this board
    }else if(i==-2){
      j = 0; //select all
    }
    shiftOut(PIN_MEM_SBIT, PIN_MEM_SCLK, MSBFIRST, j);
    digitalWrite(PIN_MEM_SLCH, HIGH);
  }
}
void RAM::memSendInstr(byte instr, unsigned long addr){
  digitalWrite(PIN_MEM_MCL, LOW);
  shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, instr);
  shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, (addr >> 16) & 0xFF);
  shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, (addr >>  8) & 0xFF);
  shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, (addr      ) & 0xFF);
}

void RAM::memWrite(unsigned long addr, byte val) {
  int chipNum = addr/(MAX_CHIP_ADDR+1);
  memCS( chipNum );
  addr = addr % (MAX_CHIP_ADDR+1);
  memSendInstr( 0x02, addr ); //write
  shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, val);
  memCS(-1); //TODO optimize by a rapid pulse on the shift reg clock then latch
}
void RAM::memWrite(unsigned long addr, byte* vals, unsigned int s, unsigned int len){
  int chipNum = addr/(MAX_CHIP_ADDR+1);
  memCS( chipNum );
  addr = addr % (MAX_CHIP_ADDR+1);
  memSendInstr( 0x02, addr); //write
  for( int i = s; i<s+len; i++ ){
    shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, vals[i]);
    addr++;
    if(addr % (MAX_CHIP_ADDR+1) == 0 ){ //reached end of chip, move to next
      memCS( ++chipNum );
      memSendInstr( 0x02, 0 );
    }
  }
  memCS(-1); //TODO optimize by a rapid pulse on the shift reg clock then latch
}
void RAM::memWriteFill(unsigned long addr, byte value, unsigned long len){ 
  int chipNum = addr/(MAX_CHIP_ADDR+1);
  memCS( chipNum );
  addr = addr % (MAX_CHIP_ADDR+1);
  memSendInstr( 0x02, addr); //write
  for( int i = 0; i<len; i++ ){
    shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, value);
    addr++;
    if(addr % (MAX_CHIP_ADDR+1) == 0 ){ //reached end of chip, move to next
      memCS( ++chipNum );
      memSendInstr( 0x02, 0 );
    }
  }
  memCS(-1); //TODO optimize by a rapid pulse on the shift reg clock then latch
}

byte RAM::memRead(unsigned long addr){
  byte tmp;
  memRead(addr, &tmp, 1);
  return tmp;
}
void RAM::memRead(unsigned long addr, byte* buf, unsigned int len){
  memRead( addr, buf, 0, len);
}
void RAM::memRead(unsigned long addr, byte* buf, unsigned int s, unsigned int len){
  int chipNum = addr/(MAX_CHIP_ADDR+1);
  memCS( chipNum );
  addr = addr % (MAX_CHIP_ADDR+1);
  memSendInstr( 0x03, addr ); //read
  
  for(int i = s; i<s+len; i++){
    buf[i] = shiftIn(PIN_MEM_MSO, PIN_MEM_MCL, MSBFIRST);
    addr++;
    if(addr % (MAX_CHIP_ADDR+1) == 0 ){ //reached end of chip, move to next
      memCS( ++chipNum );
      memSendInstr( 0x03, 0 );
    }
  }
  memCS(-1);
}

unsigned long RAM::memReadUL(unsigned long addr){
  byte buf[4];
  RAM::memRead( addr, buf, 4);
  unsigned long out = 0;
  for(byte i = 0; i<4; i++){
    out =  out << 8;
    out |= buf[i];
  }
  return out;
}
void RAM::memWriteUL(unsigned long addr, unsigned long value){
  byte buf[4];
  for(byte i = 0; i<4; i++){
    buf[i] = (value >> ((3-i)*8)) & 0xFF;
  }
  RAM::memWrite(addr, buf, 0, 4);
}

void RAM::memClear() {
  memCS( -2 ); //all
  memSendInstr( 0x02, 0 ); //write
  for( long l = 0; l <= MAX_CHIP_ADDR; l++){
     shiftOut(PIN_MEM_MSI, PIN_MEM_MCL, MSBFIRST, 0);
  }
}
