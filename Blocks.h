#ifndef Blocks_h
#define Blocks_h
#include <Arduino.h>


//block format
//previous block number,
//next block number,
//data

//32-bit addressing (unsigned long) limits this to 33,554,432 blocks at max
//For a block size of 128...
//each chip provides 131,072 bytes or 1024 blocks
//a full board provides 8,192‬ blocks
//with 32-bit addressing there can be up to 262,144‬ blocks max (arduino int is only 0 to 65,535
namespace Blocks{
  //TODO optimize empty block finder: unsigned long firstEmptyBlock = 1; //0 reserved by channel managment system
  extern unsigned long blocksUsed;
  const unsigned int BLOCK_SIZE = 128; //TODO EEPROM config
  const unsigned int BLOCK_HEADER_SIZE = 8; //4 for prev, 4 for next block number
  const unsigned int BLOCK_PAYLOAD_SIZE = BLOCK_SIZE - BLOCK_HEADER_SIZE;

  typedef unsigned long BlockID;
  
//marked as used if any of:
// - block is number 0
// - block's next field is not 0  (since 0 will always be the first block, nothing will point to it, so this means the block in question isn't used
//if a block does not have a next or prev, it will point to itself

  /**Must have buffer size of atleast BLOCK_PAYLOAD_SIZE*/
  boolean isUsed(BlockID blockNum); //blockNum==0 or next!=0
  boolean hasNext(BlockID blockNum);
  boolean hasPrev(BlockID blockNum);
  BlockID getNextBlock(BlockID blockNum);
  BlockID getPrevBlock(BlockID blockNum);
  BlockID getFirst(BlockID blockNum);
  BlockID getLast(BlockID blockNum);
  BlockID getNth(BlockID start, BlockID offset);

  BlockID getBlockNumber(unsigned long address);
  unsigned long getBlockAddress(BlockID number);

  /**Returns block number of new block*/
  BlockID allocate();       //New block for sequence
  BlockID allocate(BlockID previous);       //previous block number to point to
  void deallocate(BlockID toDeallocate); //deallocates this block, and all after
  BlockID locateUnused();
  BlockID maxBlocks();
  

}

#endif
