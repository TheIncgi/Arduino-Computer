#ifndef Theincgi_MEMCHANNEL_H
#define Theincgi_MEMCHANNEL_H
#include <Arduino.h>
#include "Blocks.h"


namespace MemChannel {
  //id is based on first block number
  typedef unsigned long ChannelID;
  //address within the channel
  typedef unsigned long Address;

  const byte CHANNEL_HEADER_SIZE = 4;

  extern Address usedChannels;
  

  //returning 0 means no channel could be created
  ChannelID createChannel();

  bool grow(ChannelID);

  ChannelID getChannelCount();
  unsigned long getBlockCount(ChannelID id);
  unsigned long getChannelLength(ChannelID id);

  bool writeByte  (ChannelID id, Address address, byte value);
  bool writeChar  (ChannelID id, Address address, char value);
  bool writeInt   (ChannelID id, Address address, int value);
  bool writeUInt  (ChannelID id, Address address, unsigned int value);
  bool writeLong  (ChannelID id, Address address, long value);
  bool writeULong (ChannelID id, Address address, unsigned long value);
  bool writeBytes (ChannelID id, Address address, byte* bytes, unsigned int len);
  bool writeBytes (ChannelID id, Address address, byte* bytes, unsigned int offset, unsigned int len);
  bool writeStr   (ChannelID id, Address address, String str);

  byte           readByte(ChannelID id, Address address);
  char           readChar(ChannelID id, Address address);
  int            readInt(ChannelID id, Address address);
  unsigned int   readUInt (ChannelID id, Address address);
  long           readLong (ChannelID id, Address address);
  unsigned long  readULong(ChannelID id, Address address);
  String         readStr(ChannelID id, Address address);
  void           readBytes(ChannelID id, Address address, byte* bytes, unsigned int len);
  void           readBytes(ChannelID id, Address address, byte* bytes, unsigned int offset, unsigned int len);
  
}

#endif
