#include "MemChannel.h"
#include <Arduino.h>
#include "Value.h"
#include "Blocks.h"
#include "Ram.h"

namespace MemChannel {
  Address usedChannels = 0;


  /**
   * returning 0 means no channel could be created
   */
  ChannelID createChannel(){
    Blocks::BlockID id = Blocks::allocate();
    if(id==0) return 0;
    usedChannels++;

    unsigned long addr = Blocks::getBlockAddress(id);
    addr += Blocks::BLOCK_HEADER_SIZE;
    
    RAM::memWriteUL( addr, 1); //blocks used in this channel
    return id;
  }

  bool grow(ChannelID id){
    Blocks::allocate( id );
    RAM::memWriteUL( Blocks::getBlockAddress(id) + Blocks::BLOCK_HEADER_SIZE, getBlockCount( id ) + 1 );
  }

  ChannelID getChannelCount(){
    return usedChannels;
  }
  
  unsigned long getBlockCount(ChannelID id){
    unsigned long addr = Blocks::getBlockAddress( Blocks::getFirst( id ) );
    return RAM::memReadUL(addr + Blocks::BLOCK_HEADER_SIZE);
  }

  unsigned long getChannelLength(ChannelID id){
    return getBlockCount(id) * Blocks::BLOCK_PAYLOAD_SIZE - CHANNEL_HEADER_SIZE;
  }

  bool writeBytes (ChannelID id, Address address, byte* bytes, unsigned int len){
    return writeBytes( id, address, bytes, 0, len);
  }  
  bool writeBytes (ChannelID id, Address address, byte* bytes, unsigned int offset, unsigned int len){
    address += CHANNEL_HEADER_SIZE;
    
    unsigned long blockCount = getBlockCount(id);
    unsigned long space = blockCount * Blocks::BLOCK_PAYLOAD_SIZE - CHANNEL_HEADER_SIZE;
    Blocks::BlockID targetBlock = address / Blocks::BLOCK_PAYLOAD_SIZE;
    Blocks::BlockID endBlock    = (address + len - 1) / Blocks::BLOCK_PAYLOAD_SIZE;
    unsigned int inBlockTarget = address % Blocks::BLOCK_PAYLOAD_SIZE;
    Blocks::BlockID blocksAdded = 0;
    if((address + len - 1) > space){
      Blocks::BlockID last = Blocks::getLast(id);
      for(unsigned long x = blockCount; x < endBlock; x++){
        last = Blocks::allocate(last); //jumps to end and adds block, returns new block location
        if(last==0) return false;
        blocksAdded++;
      }
      RAM::memWriteUL( Blocks::getBlockAddress(id) + Blocks::BLOCK_HEADER_SIZE, blockCount + blocksAdded );
    }
    Blocks::BlockID cur = Blocks::getNth(id, targetBlock);

    unsigned long writeAddr = Blocks::getBlockAddress(cur) + Blocks::BLOCK_HEADER_SIZE + inBlockTarget;
    RAM::memWrite(writeAddr, bytes, offset, len);
    return true;
  }
  bool writeByte  (ChannelID id, Address address, byte value){
    Value v;
    v.b = value;
    return writeBytes(id, address, v.bArr1, 1);
  }
  bool writeChar (ChannelID id, Address address, char value){
    Value v;
    v.c = value;
    return writeBytes(id, address, v.bArr1, 1);
  }
  bool writeInt   (ChannelID id, Address address, int value){
    Value v;
    v.i = value;
    return writeBytes(id, address, v.bArr2, 2);
  }
  bool writeUInt  (ChannelID id, Address address, unsigned int value){
    Value v;
    v.ui = value;
    return writeBytes(id, address, v.bArr2, 2);
  }
  bool writeLong  (ChannelID id, Address address, long value){
    Value v;
    v.l = value;
    return writeBytes(id, address, v.bArr4, 4);
  }
  bool writeULong (ChannelID id, Address address, unsigned long value){
    Value v;
    v.ul = value;
    return writeBytes(id, address, v.bArr4, 4);
  }
  bool writeStr   (ChannelID id, Address address, String str){
    writeUInt(id, address, str.length());
    byte buf[str.length()];
    arrayCopy(str, buf);
    writeBytes(id, address + 2, buf, str.length());
  }

  void           readBytes(ChannelID id, Address address, byte* bytes, unsigned int len){
    return readBytes(id, address, bytes, 0, len);
  }
  void           readBytes(ChannelID id, Address address, byte* bytes, unsigned int offset, unsigned int len){
    address += CHANNEL_HEADER_SIZE;
    
    unsigned long blockCount = getBlockCount(id);
    Blocks::BlockID targetBlockIndex = address / Blocks::BLOCK_PAYLOAD_SIZE;
    Blocks::BlockID endBlockIndex    = (address + len - 1) / Blocks::BLOCK_PAYLOAD_SIZE;
    unsigned int inBlockTarget = address % Blocks::BLOCK_PAYLOAD_SIZE;

    if(endBlockIndex > blockCount) return; //out of bounds
    
    Blocks::BlockID targetBlock = Blocks::getNth(id, targetBlockIndex);
    Blocks::BlockID endBlock    = Blocks::getNth(targetBlock, endBlockIndex-targetBlockIndex);

    unsigned int bufCursor = 0;
    Blocks::BlockID sBlock = targetBlock;
    while(bufCursor < len){
      unsigned long addr = Blocks::getBlockAddress(sBlock) + Blocks::BLOCK_HEADER_SIZE;
      unsigned int readLen = min(Blocks::BLOCK_PAYLOAD_SIZE, len-bufCursor);
      if(sBlock == targetBlock){
        addr += inBlockTarget;
      }
      RAM::memRead( addr, bytes, bufCursor + offset, readLen);
      bufCursor += readLen;
    }
  }
  byte           readByte(ChannelID id, Address address){
    Value v;
    readBytes(id, address, v.bArr1, 1);
    return v.b;
  }
  char           readUByte(ChannelID id, Address address){
    Value v;
    readBytes(id, address, v.bArr1, 1);
    return v.c;
  }
  int            readInt(ChannelID id, Address address){
    Value v;
    readBytes(id, address, v.bArr2, 2);
    return v.i;
  }
  unsigned int   readUInt (ChannelID id, Address address){
    Value v;
    readBytes(id, address, v.bArr2, 2);
    return v.ui;
  }
  long           readLong (ChannelID id, Address address){
    Value v;
    readBytes(id, address, v.bArr4, 4);
    return v.l;
  }
  unsigned long  readULong(ChannelID id, Address address){
    Value v;
    readBytes(id, address, v.bArr4, 4);
    return v.ul;
  }
  String         readStr(ChannelID id, Address address){
    unsigned int len = readUInt(id, address);
    byte buf[len];
    readBytes(id, address+2, buf, len);
    return (char*)buf;
  }
  
  
}
