#include <SD.h>
#include "ThePS2Keyboard.h"
#include "Ram.h"
#include "Blocks.h"
#include "Value.h"

//out means the keyboard outputs to the board
#define PIN_KEYBOARD_CLOCK 3
#define PIN_KEYBOARD_DATA 5

#define PIN_SD_MISO 50
#define PIN_SD_MOSI 51
#define PIN_SD_CLK 52
#define PIN_SD_CS 53







ThePS2Keyboard keyboard;


#define DISPLAY_BAUD 115200


void setup() {
  delay(3000);
  Serial.begin(2000000);
  Serial3.begin(115200);
  // put your setup code here, to run once:  attachInterrupt( digitalPinToInterrupt(PIN_KEYBOARD_CLOCK_OUT), onKeyboardInterrupt, FALLING);
  
  keyboard.begin( PIN_KEYBOARD_CLOCK, PIN_KEYBOARD_DATA );
  
  delay(2000);
  cls();
  sprint("Startup\n");
  Serial.println("Console: Startup");
  if (!SD.begin(53)) {
    sprint("SD CARD FAIL'D");
    return;
  }
  sprint("SD CARD OK\n");
  //sprint("Message 2");

  pinMode(PIN_MEM_SBIT, OUTPUT);
  pinMode(PIN_MEM_SLCH, OUTPUT);
  pinMode(PIN_MEM_SCLK, OUTPUT);
  
  pinMode(PIN_MEM_MSI, OUTPUT);
  pinMode(PIN_MEM_MHD, OUTPUT);
  pinMode(PIN_MEM_MCL, OUTPUT);
  pinMode(PIN_MEM_MSO, INPUT);

  digitalWrite(PIN_MEM_MHD, HIGH); //high when not in use

  RAM::countMemUnits();
  Serial.print("Memsize: "); Serial.println( RAM::memSize );
  sprint("Memory: ");
  sprint(  String( ((int)(  RAM::memSize*100/1048576.0   ))/100.0 )      );
  sprint("MiB (");
  sprint( String(RAM::memoryUnits) );
  sprint(" memory units)\n");

  sprint("Clearing ram...\n");
  RAM::memClear();
  
  testMemoryCh();
  Serial.print("TEST COMPLETE");
}

void loop() {
  delay(500);
  if(keyboard.available() > 0){
    while(keyboard.available() > 0){
      byte b = keyboard.read();
      sprint("key ");
      Serial.println(b, HEX);
      if(b == 0xF0 && keyboard.peek() == 0x24){
        Serial.println("Echo test:");
        keyboard.send(0xEE); //echo test
      }
      else if(b == 0xF0 && keyboard.peek() == 0x58){
        Serial.println("Caps lock:");
        keyboard.setCapsLock( !keyboard.isCapsLock() );
      }
    }
    Serial.println("");
  }
  //Serial.println("Sending Echo (0xEE)");
  //keyboard.send(0xEE);
  
}


//TODO read/write

void testMemoryCh(){
  {
    for(int i = 0; i<10; i++)
      Serial.println(String("Block ")+i+( Blocks::isUsed(i)? " USED" : " FREE" ));

    
    Serial.println("Preforming Block system test");
    unsigned long addr = Blocks::allocate(); //some new block
    String x = "ExamplePID";
    int len = x.length()+5;
    
    sprint("Created block: "+String( addr ) );
    Value v;
    unsigned long cur = 0;
    cur = Blocks::Channel::readUnsignedLong(0, cur);
    cur = cur==0? 4 : cur;
    Serial.print("Registering block in system channel pos ");
    Serial.println(cur);
    Blocks::Channel::writeUnsignedLong(0, cur, addr);
    Blocks::Channel::writeString( x );
  }
  {
    unsigned long cur = 4;
    unsinged long pid = 0;
    do{
      unsigned long block = Blocks::Channel::readUnsignedLong(0, cur);
      if(block==0) break;
      
    }while(true);
  }

}

void cls(){
  Serial3.write(0xFF);
  Serial3.write(0xCD);
  Serial3.flush();
}
void sprint(String s){
  Serial3.write(0x00);
  Serial3.write(0x18);
  Serial3.print(s);
  Serial3.write(0x00);//null terminated string
  Serial3.flush();
}
