#ifndef Blocks_h
#define Blocks_h

#include "Ram.h"

//block format
//previous block number,
//next block number,
//data

//32-bit addressing (unsigned long) limits this to 33,554,432 blocks at max
//For a block size of 128...
//each chip provides 131,072 bytes or 1024 blocks
//a full board provides 8,192‬ blocks
//with 32-bit addressing there can be up to 262,144‬ blocks max
namespace Blocks{
  unsigned long firstEmptyBlock = 1; //0 reserved by channel managment system
  const unsigned int BLOCK_SIZE = 128;
  const unsigned int BLOCK_HEADER_SIZE = 8; //4 for prev, 4 for next
  const unsigned int BLOCK_PAYLOAD_SIZE = BLOCK_SIZE - BLOCK_HEADER_SIZE;
  
//marked as used if any of:
// - block is number 0
// - block's next field is not 0  (since 0 will always be the first block, nothing will point to it, so this means the block in question isn't used
//if a block does not have a next or prev, it will point to itself
  
  void readBlock(unsigned long, byte* blockBuffer);
  void writeBlock(unsigned long, byte* blockBuffer);
  boolean isUsed(unsigned long blockNum); //blockNum==0 or next!=0
  boolean hasNext(unsigned long blockNum);
  boolean hasPrev(unsigned long blockNum);
  unsigned long getNextBlock(unsigned long blockNum);
  unsigned long getPrevBlock(unsigned long blockNum);
  
  namespace Channel{
    boolean isExists(String pID);
    void make(String pID); //should err if name exists
    void destroy(String pID);
    unsigned long getBlock(String pID, unsigned long blockNum);
    void read( unsigned long seek, byte* buf, unsigned int s, unsigned int len);
    void write(unsigned long seek, byte* buf, unsigned int len);
  }
}

#endif
