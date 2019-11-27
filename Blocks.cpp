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

  unsigned long allocate(){
    unsigned long block2 = Blocks::locateUnused();
    if(block2 == 0)
      return 0; //none
      
    unsigned long addr2 = Blocks::getBlockAddress( block2 );
    
    RAM::memWriteUL(addr2   , block2);   //new.prev = old
    RAM::memWriteUL(addr2 +4, block2); //new.next = new //no next, points to self
    Blocks::blocksUsed++;
    return block2;
  }

  unsigned long allocate(unsigned long previous){
    while(Blocks::hasNext(previous)) previous = Blocks::getNextBlock(previous);
    unsigned long block2 = Blocks::locateUnused();
    if(block2 == 0)
      return 0; //none
      
    unsigned long addr1 = Blocks::getBlockAddress( previous );
    unsigned long addr2 = Blocks::getBlockAddress( block2 );

    RAM::memWriteUL(addr1 +4, block2); //old.next = new
    
    RAM::memWriteUL(addr2   , previous);   //new.prev = old
    RAM::memWriteUL(addr2 +4, block2); //new.next = new //no next, points to self
    Blocks::blocksUsed++;
    return block2;
  }
  void deallocate(unsigned long toDeallocate){//putting the first block in a sequence will erase the whole sequence
    while(true){
      unsigned long addr = Blocks::getBlockAddress(toDeallocate);
      unsigned long next = Blocks::getNextBlock(toDeallocate);
      RAM::memWriteFill(addr, 0, Blocks::BLOCK_SIZE); //clean up
      Blocks::blocksUsed--;
      if(next == toDeallocate) break;
      if(Blocks::blocksUsed==0) return; //throw "EXCESSIVE DEALLOCATION";//block 0 is always claimed
      toDeallocate = next;
    }
  }
  unsigned long locateUnused(){ 
    for(unsigned long blockN = 1; blockN < maxBlocks(); blockN++){
      if( ! Blocks::isUsed(blockN) ) return blockN;
    }
    return 0; //none, out of memory
  }

  unsigned long maxBlocks(){
    return (RAM::MAX_CHIP_ADDR+1 / Blocks::BLOCK_SIZE) * RAM::memoryUnits;
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
        unsigned int toRead = min(len, Blocks::BLOCK_PAYLOAD_SIZE);
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
    unsigned int write(unsigned long startingBlock, unsigned long seekAddr, byte* buf, unsigned int s, unsigned int len){
      unsigned long targetBlockInSequence = seekAddr / Blocks::BLOCK_PAYLOAD_SIZE;
      unsigned long cur = startingBlock;
      unsigned long next = Blocks::getNextBlock( cur );
      unsigned int nWrote = 0;
      while(next!=cur && cur != targetBlockInSequence){
        cur = next;
        next = Blocks::getNextBlock( cur );
      }
      while(cur < targetBlockInSequence){
        cur = Blocks::allocate(cur);
      }
      unsigned int pos = s;
      while(len > 0){
        if(cur == 0) break; //no addr
        unsigned int toWrite = min(len, Blocks::BLOCK_PAYLOAD_SIZE);
        RAM::memWrite(Blocks::getBlockAddress(cur)+8, buf, s, toWrite);
        nWrote += toWrite;
        s      += toWrite;
        len    -= toWrite;
        if(len <= 0) return nWrote;
        
        if(!Blocks::hasNext(cur)) //allocate or get next block
          cur = Blocks::allocate(cur);
        else
          cur = Blocks::getNextBlock(cur);
      }
      return nWrote;
    }

  }
}
