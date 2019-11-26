#include "Blocks.h"
#include "Ram.h"

namespace Blocks{
//  unsigned long firstEmptyBlock = 1; //0 reserved by channel managment system
//  const unsigned int BLOCK_SIZE = 128;
//  const unsigned int BLOCK_HEADER_SIZE = 8; //4 for prev, 4 for next
//  const unsigned int BLOCK_PAYLOAD_SIZE = BLOCK_SIZE - BLOCK_HEADER_SIZE;
  
//marked as used if any of:
// - block is number 0
// - block's next field is not 0  (since 0 will always be the first block, nothing will point to it, so this means the block in question isn't used
//if a block does not have a next or prev, it will point to itself
  
  void readBlock(unsigned long, byte* blockBuffer){
  }
  void writeBlock(unsigned long, byte* blockBuffer){
  }
  boolean isUsed(unsigned long blockNum){ //blockNum==0 or next!=0
    if( blockNum==0 ) return true;
    return 0 < getNextBlock( blockNum );
  }
  boolean hasNext(unsigned long blockNum){
    return Blocks::getNextBlock(blockNum) != blockNum;
  }
  boolean hasPrev(unsigned long blockNum){
    return Blocks::getPrevBlock(blockNum) != blockNum;
  }
  unsigned long getNextBlock(unsigned long blockNum){
    unsigned long addr = Blocks::getBlockAddress( blockNum ) + 4;
    return RAM::memReadUL( addr );
  }
  unsigned long getPrevBlock(unsigned long blockNum){
    unsigned long addr = Blocks::getBlockAddress( blockNum );
    return RAM::memReadUL( addr );
  }

  unsigned long getBlockNumber(unsigned long address){
    return address / Blocks::BLOCK_SIZE;
  }
  unsigned long getBlockAddress(unsigned long number){
    return number * Blocks::BLOCK_SIZE;
  }
  
  namespace Channel{
    unsigned int read( unsigned long startingBlock, unsigned long seekAddr, byte* buf, unsigned int s, unsigned int len){
      unsigned long targetBlockInSequence = seekAddr / Blocks::BLOCK_PAYLOAD_SIZE;
      unsigned long cur = startingBlock;
      unsigned long next = Blocks::getNextBlock( cur );
      unsigned int nRead = 0;
      while(next!=cur && cur != targetBlockInSequence){
        cur = next;
        next = Blocks::getNextBlock( cur );
      }
      if(cur != targetBlockInSequence) return 0;
      while( len > 0 ){
        unsigned byte toRead = min(len, Blocks::BLOCK_PAYLOAD_SIZE);
        RAM::memRead(Blocks::getBlockAddress(cur)+8, buf, s, toRead); //addr, buf, start, len
        s+=toRead;
        len-=toRead;
        nRead += toRead;
        if(len > 0 && next==cur) return nRead;
        cur = next;
        next = Blocks::getNextBlock( cur );
      }
      return nRead;
    }
    void write(unsigned long startingBlock, unsigned long seekAddr, byte* buf, unsigned int len){}

  }
}
