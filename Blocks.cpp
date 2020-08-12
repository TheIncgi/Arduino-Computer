#include "Blocks.h"
#include "Ram.h"
#include "Value.h"

namespace Blocks{
//  unsigned long firstEmptyBlock = 1; //0 reserved by channel managment system
 unsigned long blocksUsed = 1;
//  const unsigned int BLOCK_SIZE = 128;
//  const unsigned int BLOCK_HEADER_SIZE = 8; //4 for prev, 4 for next
//  const unsigned int BLOCK_PAYLOAD_SIZE = BLOCK_SIZE - BLOCK_HEADER_SIZE;
  
//marked as used if any of:
// - block is number 0
// - block's next field is not 0  (since 0 will always be the first block, nothing will point to it, so this means the block in question isn't used
//if a block does not have a next or prev, it will point to itself
  
  
  boolean isUsed(BlockID blockNum){ //blockNum==0 or next!=0
    if( blockNum==0 ) return true;
    return 0 < getNextBlock( blockNum );
  }
  boolean hasNext(BlockID blockNum){
    return Blocks::getNextBlock(blockNum) != blockNum;
  }
  boolean hasPrev(BlockID blockNum){
    return Blocks::getPrevBlock(blockNum) != blockNum;
  }
  BlockID getNextBlock(BlockID blockNum){
    unsigned long addr = Blocks::getBlockAddress( blockNum ) + 4;
    //Serial.println(String("Next for ")+blockNum+" is "+RAM::memReadUL(addr));
    return RAM::memReadUL( addr );
  }
  BlockID getPrevBlock(BlockID blockNum){ //<67108867 4294967295>
    unsigned long addr = Blocks::getBlockAddress( blockNum );
    return RAM::memReadUL( addr );
  }
  BlockID getFirst(BlockID blockNum){
    while(hasPrev(blockNum)) blockNum = getPrevBlock(blockNum);
    return blockNum;
  }
  BlockID getLast(BlockID blockNum){
    while(hasNext(blockNum)) blockNum = getNextBlock(blockNum);
    return blockNum;
  }
  
  BlockID getBlockNumber(unsigned long address){ //inverse of getBlockAddress, doesn't use payload size
    return address / Blocks::BLOCK_SIZE;
  }
  unsigned long getBlockAddress(BlockID block){
    return block * Blocks::BLOCK_SIZE;
  }

  BlockID allocate(){
    BlockID block2 = Blocks::locateUnused();
    if(block2 == 0)
      return 0; //none
      
    unsigned long addr2 = Blocks::getBlockAddress( block2 );
    
    RAM::memWriteUL(addr2   , block2);   //new.prev = old
    RAM::memWriteUL(addr2 +4, block2); //new.next = new //no next, points to self
    Blocks::blocksUsed++;
    return block2;
  }

  BlockID allocate(BlockID previous){
    BlockID block2 = Blocks::locateUnused();
    if(block2 == 0)
      return 0; //none

    previous = getLast(previous);
      
    unsigned long addr1 = Blocks::getBlockAddress( previous );
    unsigned long addr2 = Blocks::getBlockAddress( block2 );

    RAM::memWriteUL(addr1 +4, block2); //old.next = new
    
    RAM::memWriteUL(addr2   , previous);   //new.prev = old
    RAM::memWriteUL(addr2 +4, block2); //new.next = new //no next, points to self
    Blocks::blocksUsed++;
    return block2;
  }
  void deallocate(BlockID toDeallocate){//putting the first block in a sequence will erase the whole sequence
    while(true){
      unsigned long addr = Blocks::getBlockAddress(toDeallocate);
      BlockID       next = Blocks::getNextBlock(toDeallocate);
      RAM::memWriteFill(addr, 0, Blocks::BLOCK_SIZE); //clean up
      Blocks::blocksUsed--;
      if(next == toDeallocate) break;
      if(Blocks::blocksUsed==0) return; //throw "EXCESSIVE DEALLOCATION";//block 0 is always claimed
      toDeallocate = next;
    }
  }
  BlockID locateUnused(){ 
    Serial.println(String("Looking for block in range 1 to ")+maxBlocks() );
    if(blocksUsed == maxBlocks()){
      Serial.println("OUT OF MEMORY");
      return 0;
    }
    
    for(BlockID blockN = 1; blockN < maxBlocks(); blockN++){
      if( ! Blocks::isUsed(blockN) ) return blockN;
    }
    Serial.println("OUT OF MEMORY");
    return 0; //none, out of memory
  }

  BlockID maxBlocks(){
    return ((RAM::MAX_CHIP_ADDR+1) / Blocks::BLOCK_SIZE) * RAM::memoryUnits;
  }

  BlockID getNth(BlockID start, BlockID offset){
    for(BlockID x = 0; x<offset; x++)
      start = getNextBlock(start);
    return start;
  }
  

}
