#ifndef Theincgi_HEAP_H
#define Theincgi_HEAP_H
#include <Arduino.h>
#include "MemChannel.h"

namespace Heap {
  //returning an address of 0 indicates an error (Out of memory)
  const byte HEAP_OBJ_HEADER_SIZE = 9;
  MemChannel::Address allocate(MemChannel::ChannelID heapID, unsigned int nBytes, char type);
  MemChannel::Address allocate(MemChannel::ChannelID heapID, unsigned int nBytes, char type, byte* bArr);

  //ref & deref increment and decrement reference counters on each element, if it reaches 0 the element is deallocated
  //for any map's or structs, their deconstructor should be called
  void reference(MemChannel::ChannelID heapID, MemChannel::Address heapPtr);
  void dereference(MemChannel::ChannelID heapID, MemChannel::Address heapPtr);
  
  void dealloc(MemChannel::ChannelID heapID, MemChannel::Address heapPtr);
  char typeOf(MemChannel::Address heapPtr);
  unsigned long findN(MemChannel::ChannelID heapID, unsigned int nBytes);
  unsigned long getStrongReferenceCount(MemChannel::ChannelID, MemChannel::Address heapPtr);
}

#endif
