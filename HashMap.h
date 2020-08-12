#ifndef Theincgi_HASHMAP_H
#define Theincgi_HASHMAP_H
#include "MemChannel.h"
#include "Heap.h"

namespace HashMap {
  const byte HAHSMAP_DEFAULT_SIZE = 4;
  
  /*
   * create KEYS and VALUES arrays.
   *                            |  4      |     4       |    4       |
   * Creates heap object (9, 'm', usedSize, keysAddress, valuesAddress) 
   */
  MemChannel::Address create(MemChannel::ChannelID id);
  /**
   * Attempt to resize
   */
  bool resize(MemChannel::ChannelID id, MemChannel::Address mapPtr);
  void del(MemChannel::ChannelID id, MemChannel::Address mapPtr);
  unsigned long hashCode( byte* data, unsigned int len);
  unsigned long hashCode( MemChannel::ChannelID id, MemChannel::Address objPtr );
  MemChannel::Address getKeysAddress(MemChannel::ChannelID id, MemChannel::Address objPtr);
  MemChannel::Address getValuesAddress(MemChannel::ChannelID id, MemChannel::Address objPtr);
  unsigned long getSize( MemChannel::ChannelID id, MemChannel::Address objPtr);
  
}

#endif
