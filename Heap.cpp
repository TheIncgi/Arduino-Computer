#include "Heap.h"
#include "Blocks.h"
#include "MemChannel.h"
#include "Value.h"
#include "HashMap.h"
namespace Heap {

  
  MemChannel::Address allocate(MemChannel::ChannelID heapID, unsigned int nBytes, char type) {
    unsigned long out = findN( heapID, nBytes+5 );
    if(out==0) return 0;
    MemChannel::writeULong( heapID, out, nBytes ); //size
    MemChannel::writeULong( heapID, out+4, 0); //default is 0 usages, deref-ing checks if at 1 or 0
    MemChannel::writeChar( heapID, out+8, type ); //type
    return out;
  }

  //with initialization of variable
  MemChannel::Address allocate(MemChannel::ChannelID heapID, unsigned int nBytes, char type, byte* bArr ) {
    unsigned long out = allocate(heapID, nBytes, type );
    if( out == 0 ) return;
    MemChannel::writeBytes( heapID, out + HEAP_OBJ_HEADER_SIZE, bArr, 0, nBytes );
    return out;
  }

  
  void dealloc(MemChannel::ChannelID heapID, MemChannel::Address heapPtr) {
    unsigned long s = MemChannel::readULong( heapID, heapPtr );
    MemChannel::Address e = heapPtr + HEAP_OBJ_HEADER_SIZE + s;
    byte empty[24];
    for(; heapPtr < e; heapPtr+=24){
      MemChannel::writeBytes( heapID, heapPtr, empty, 0, min(24, s));
      s-=24;
    }
  }

  //ref & deref increment and decrement reference counters on each element, if it reaches 0 the element is deallocated
  //for any map's or structs, their deconstructor should be called
  //these are STRONG references only, or references from a stack to be more exact
  void reference(MemChannel::ChannelID heapID, MemChannel::Address heapPtr){
    MemChannel::writeULong( heapID, heapPtr + 4, getReferenceCount( heapID, heapPtr) + 1);
  }
  void dereference(MemChannel::ChannelID heapID, MemChannel::Address heapPtr){
    unsigned long refs = getStrongReferenceCount(heapID, heapPtr);
    MemChannel::ChannelID nextChannel = heapID;
    MemChannel::Address nextAddr = heapPtr;

    /*
     * TODO
     * Method proposals
     *  1. Recursion
     *     Unknown stack limitiations, unknown behavior on stack crash
     *  2. Deallocation queue in external ram
     *     Requires allocation of memory to deallocate. (but this memory doesn't have any reference chains)
     *  3. Follow reference to end, de-allocate, the go back to start and repeat
     *     Trades time away to save on space. Slow on large depths. Not sure about cycles rn.
     */

  
    while(nextChannel!=0){
      if(refs <= 1){
        char type = typeOf(heapID, heapPtr);
        switch(type){
          case 'm':{
            HashMap::del(heapID, heapPtr);
          }break;
          case '*':{
            MemChannel::ChannelID ptrChannel = MemChannel::readULong(heapID, heapPtr + HEAP_OBJ_HEADER_SIZE);
            MemChannel::ChannelID ptrAddress = MemChannel::readULong(heapID, heapPtr + HEAP_OBJ_HEADER_SIZE + 4);
            dereference();
          }break;
          case 'a':{
          }break;
        }
        
        dealloc( heapID, heapPtr );
      }else{
        MemChannel::writeULong( heapID, heapPtr + 4, refs-1);
      }
    }
  }
  unsigned long getStrongReferenceCount(MemChannel::ChannelID heapID, MemChannel::Address heapPtr){
    return MemChannel::readULong( heapID, heapPtr + 4);
  }
  
  char typeOf(MemChannel::ChannelID heapID, MemChannel::Address heapPtr) {
    return MemChannel::readChar( heapID, heapPtr + 8 );
  }

  unsigned long findN(MemChannel::ChannelID heapID, unsigned int nBytes ) {
    unsigned long limit = MemChannel::getChannelLength(heapID);
    unsigned int contig = 0;
    byte buffSize = 24;
    byte scanBuffer[buffSize];
    MemChannel::Address ptr = 1;
    MemChannel::Address scan = 1;
    Value v;
    while(contig < nBytes){ //while still need more
      if(limit-scan < 4){   //near end
        if(MemChannel::grow(heapID)){  //add space
          limit = MemChannel::getChannelLength(heapID);
        }else{ //oops, cant, ng
          return 0;
        }
      }
      byte usedBuffer = min(buffSize, limit-scan);
      MemChannel::readBytes(heapID, scan, scanBuffer, usedBuffer); 
      for(byte x=0; x < usedBuffer-3; x++){ //-3 to read the full size of a heap object
        MemChannel::Address here = scan + x;
        if(here < ptr) continue;
        if(scanBuffer[x]==0){
          contig++;
          if(contig >= nBytes) return ptr;
        }else{
          contig = 0;
          v.bArr4[0] = scanBuffer[x];
          v.bArr4[1] = scanBuffer[x+1];
          v.bArr4[1] = scanBuffer[x+2];
          v.bArr4[1] = scanBuffer[x+3]; //TODO check endian
          ptr = here + HEAP_OBJ_HEADER_SIZE + v.ul; //past current object
        }
      }
      scan += buffSize-1;
    }
    return 0;
  }
}
