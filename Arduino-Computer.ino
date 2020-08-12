#include <SD.h>
#include "ThePS2Keyboard.h"
#include "Ram.h"
#include "Blocks.h"
#include "Value.h"
#include "Screen.h"
#include "MemChannel.h"
#include "Heap.h"
//out means the keyboard outputs to the board
#define PIN_KEYBOARD_CLOCK 3
#define PIN_KEYBOARD_DATA 5

#define PIN_SD_MISO 50
#define PIN_SD_MOSI 51
#define PIN_SD_CLK 52
#define PIN_SD_CS 53


//TODO:
// Heap auto-deallocation




ThePS2Keyboard keyboard;


#define DISPLAY_BAUD 115200


void setup() {
  delay(3000);
  Serial.begin(2000000);
  Serial3.begin(115200);
  // put your setup code here, to run once:  attachInterrupt( digitalPinToInterrupt(PIN_KEYBOARD_CLOCK_OUT), onKeyboardInterrupt, FALLING);
  
  keyboard.begin( PIN_KEYBOARD_CLOCK, PIN_KEYBOARD_DATA );
  Serial.println("Setting up...");
  delay(2000);
  Screen::cls();
  Screen::fcol(LIGHT_GRAY);
  Screen::sprint("Startup\n");
  Serial.println("Console: Startup");
  if (!SD.begin(53)) {
    Screen::fcol(RED);
    Screen::sprint("SD CARD FAIL'D\nInsert & Reboot.");
    voidLoop();
  }
  Screen::sprint("SD CARD:");
  Screen::fcol(GREEN);
  Screen::sprint(" OK\n");
  Screen::fcol(LIGHT_GRAY);
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
  Screen::sprint("Memory: ");
  Screen::fcol(BLUE);
  Screen::sprint(  String( ((int)(  RAM::memSize*100/1048576.0   ))/100.0 )      );
  Screen::fcol(LIGHT_GRAY);
  Screen::sprint("MiB (");
  Screen::fcol(BLUE);
  Screen::sprint( String(RAM::memoryUnits) );
  Screen::fcol(LIGHT_GRAY);
  Screen::sprint(" memory units)\n");

  if(RAM::memoryUnits == 0){
    Screen::fcol(RED);
    Screen::sprint("No memory units found. Check wires and restart.");
    voidLoop();
  }

  
  RAM::memClear();
  
  setupMemoryCh();
  Screen::fcol(BLUE);
  Screen::sprint("POST TEST COMPLETE\n");
  Screen::fcol(WHITE);
}

void loop() {
  delay(500);
  if(keyboard.available() > 0){
    while(keyboard.available() > 0){
      byte b = keyboard.read();
      Screen::sprint(String(b)+" ");
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

void voidLoop() {
  while(true)
    delay(10000);
}

//TODO read/write

void setupMemoryCh(){
  RAM::memWriteUL(Blocks::BLOCK_HEADER_SIZE, 1);
  
  MemChannel::writeStr(0, 0, "Hello");
  MemChannel::ChannelID testHeap = MemChannel::createChannel();
  Value v;
  v.i = 0x4E2B;
  Heap::allocate(testHeap, 2, 'i', v.bArr2);
  RAM::memDump(0, Blocks::BLOCK_SIZE*2);
  Screen::sprint( MemChannel::readStr(0,0) + "\n" );
}

//void testMemoryCh(){
//  {
//    for(int i = 0; i<10; i++)
//      Serial.println(String("Block ")+i+( Blocks::isUsed(i)? String(" USED <")+Blocks::getPrevBlock(i)+" "+Blocks::getNextBlock(i)+">" : " FREE" ));
//    
//    Serial.println("Preforming Block system test");
//    unsigned long addr = Blocks::allocate(); //some new block
//    String x = "ExamplePID";
//    int len = x.length()+5;
//
//    for(int i = 0; i<10; i++)
//      Serial.println(String("Block ")+i+( Blocks::isUsed(i)? String(" USED <")+Blocks::getPrevBlock(i)+" "+Blocks::getNextBlock(i)+">" : " FREE" ));
//
//    
//    Screen::sprint("Created block: "+String( addr )+"\n");
//    Value v;
//    unsigned long cur = 0;
//    cur = Blocks::Channel::readUnsignedLong(0, cur); //block, addr
//    cur = cur==0? 4 : cur;
//    Serial.print("Registering block in system channel pos ");
//    Serial.println(cur);
//    Blocks::Channel::write(0, cur, addr);
//    cur-=4; Serial.print("Check: "); Serial.println(Blocks::Channel::readUnsignedLong(0,cur));
//    Blocks::Channel::write(0, cur, x );
//  }
//  {
//    unsigned long cur = 0;
//    unsigned long pid = 0;
//    do{
//      unsigned long block = Blocks::Channel::readUnsignedLong(0, cur);
//      if(block==0) break;
//      String bName = Blocks::Channel::readString(0, cur);
//      Screen::sprint("PID [");
//      Screen::sprint(String(pid++));
//      Screen::sprint("] Block ");
//      Screen::sprint(String(block));
//      Screen::sprint(" ");
//      Screen::sprint(bName);
//      Screen::sprint("\n");
//    }while(true);
//  }
//
//}
