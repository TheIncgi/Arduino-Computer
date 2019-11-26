#include "ThePS2Keyboard.h"

volatile int8_t ThePS2Keyboard::interruptCounter = -1;
volatile byte ThePS2Keyboard::interruptBuffer = 0;
volatile bool ThePS2Keyboard::parity = false;
volatile bool ThePS2Keyboard::isSending = false;
byte ThePS2Keyboard::clockPin = -1;
byte ThePS2Keyboard::dataPin = -1;
volatile byte ThePS2Keyboard::tx = 0;
volatile bool ThePS2Keyboard::comError = false;
volatile long ThePS2Keyboard::timeout = -1;
CircularByteBuffer ThePS2Keyboard::rx(32);
volatile ThePS2Keyboard::Status ThePS2Keyboard::status = ThePS2Keyboard::Status::IDLE;
bool ThePS2Keyboard::capsLock = false;
bool ThePS2Keyboard::numLock = false;
bool ThePS2Keyboard::scrollLock = false;


void ThePS2Keyboard::begin(byte clockPin, byte dataPin) {
  ThePS2Keyboard::clockPin = clockPin;
  ThePS2Keyboard::dataPin = dataPin;

  pinMode(clockPin, INPUT_PULLUP);
  pinMode(dataPin, INPUT_PULLUP);

  interruptCounter = -1;
  isSending = false;
  comError = false;
  timeout = millis() - 1;
  status = ThePS2Keyboard::Status::IDLE;
  tx = 0;
  //    for(byte b = 0; b<eventBuffer.limit; b++){
  //      eventBuffer.buffer[b] = 0;
  //    }
  capsLock = false;
  numLock = false;
  scrollLock = false;
  
  attachInterrupt( digitalPinToInterrupt( clockPin ), onInterrupt, FALLING );
}

void ThePS2Keyboard::end() {
  detachInterrupt( digitalPinToInterrupt( clockPin ) );
}

void ThePS2Keyboard::send(byte b) {
  Serial.println("Waiting for idle state");
  while(status != Status::IDLE)
    delayMicroseconds(50);
  Serial.println("sending to keyboard...");
  tx = b;
  interruptCounter = -1;
  parity = 1;
  comError = false;
  timeout = millis()+250;
  status = Status::TRANSMITTING;
  pinMode(clockPin, OUTPUT);
  digitalWrite(clockPin, LOW); //pulls clock low, also triggers interrupt
}

bool ThePS2Keyboard::isCapsLock(){
  return capsLock;
}
bool ThePS2Keyboard::isNumLock(){
  return numLock;  
}
bool ThePS2Keyboard::isScrollLock(){
  return scrollLock;  
}

void ThePS2Keyboard::setCapsLock(bool state){
  capsLock = state;
  updateLEDs();
}
void ThePS2Keyboard::setNumLock(bool state){
  numLock = state;
  updateLEDs();
}
void ThePS2Keyboard::setScrollLock(bool state){
  scrollLock = state;
  updateLEDs();
}

void ThePS2Keyboard::updateLEDs(){
  send(0xED);
  send( (capsLock << 2) | (numLock << 1) | (scrollLock) );
}

byte ThePS2Keyboard::read() {
  return rx.poll();
}
byte ThePS2Keyboard::peek() {
  return rx.peek();
}
short ThePS2Keyboard::available() {
  return rx.available();
}



//on falling edge, because:
//Device writing occurs when the clock is low
//Keyboard writing occurs while the clock is high (dev read low)
void ThePS2Keyboard::onInterrupt() {
  //boolean dataState = digitalRead( dataPin );
  //Serial.print(dataState);
  if(status == Status::IDLE || timeout < millis()){
    timeout = millis()+250;
    interruptCounter = -1;
    interruptBuffer = 0;
    parity = 0;
    comError = false;
    status = Status::IDLE;
  }
  if(status == Status::IDLE){
    status = Status::RECEIVING;
  }
  if(status == Status::RECEIVING){
    boolean dataState = digitalRead( dataPin );
    switch(interruptCounter){
      
      case -1:{ //start bit (always 0)
        if(dataState!=0)
          comError = true;
        else{
          interruptCounter = -1;
          interruptBuffer = 0;
          parity = 0;
        }
      } break;
      
      case 0: //data
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:{
         interruptBuffer |= dataState << (interruptCounter);
         parity ^= dataState;
        
      } break;
      
      case 8:{ //parity (odd)
        parity ^= dataState;
        if( parity != 1 )
          comError = true;
      } break;
      
      case 9:{ //stop (this is bit #11) (always 1)
        if( dataState != 1 )
          comError = true;
          
        if(!comError){
          rx.offer( interruptBuffer );
          Serial.println();
          Serial.print(">>");
          Serial.println(interruptBuffer,HEX);
        }
        comError = false;
        interruptCounter = -1;
        status = Status::IDLE;
        return;
      }
      
      default:
        break;
    }
  }else if(status == Status::TRANSMITTING){
   // Serial.println(interruptCounter);
    switch(interruptCounter){
      case -1: //inital clock low by us
        delayMicroseconds(120); //must be kept low for atleast 100 uS
        pinMode( dataPin, OUTPUT);
        digitalWrite(dataPin, LOW); //data brought low via current sink
        delayMicroseconds(12);
        pinMode(clockPin, INPUT_PULLUP); //clock released
        //keyboard now gens clock signals
        break;
      case 0: //data
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:{ //lsb -> msb
        bool b = (tx >> interruptCounter) & 1;
        digitalWrite(dataPin, b);
        parity ^= b;
        break;
      }
      case 8: //parity
        digitalWrite(dataPin, parity);
        break;
      case 9: //stop
        pinMode(dataPin, INPUT_PULLUP); //release for stop bit of 1
        break;
      case 10: //ack
        interruptCounter = -1;
        status = Status::IDLE;
        
       // Serial.println("Transmission complete");
        return;
      default:
        break;
    }  
  }
  interruptCounter++;
}

//77  76 6e
//77 77 77 ee 77 77 ee 77
