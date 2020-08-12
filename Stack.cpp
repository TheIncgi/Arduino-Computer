#include "Stack.h"
#include "MemChannel.h"
#include "OpCodes.h"

namespace Stack{
  //Frame header:
  //  L Active Frame
  //  L Program Counter
  //Frame:
  //  0 or return address (next instruction to execute)
  //  Address - Previous frame start
  //  Address - Current Frame End position (exclusive)
  //  named vars.... | type, namePtr, data
  
  void pushFrame(MemChannel::ChannelID id){
    MemChannel::Address previousFrameStart = getActiveFrame(id);
    MemChannel::Address endOfPrevious = getEndOfFrame(id);
    MemChannel::writeULong(id, endOfPrevious, 0); //no return, scope increase
    MemChannel::writeULong(id, endOfPrevious + 4, previousFrameStart); //prior frame
    MemChannel::writeULong(id, endOfPrevious + 8, endOfPrevious + STACK_FRAME_HEADER_SIZE);  //end of frame
    MemChannel::writeULong(id, endOfPrevious + 12, 0); //Error handler not set
  }
  void pushReturnFrame(MemChannel::ChannelID id, MemChannel::Address rAdd){
    MemChannel::Address previousFrameStart = getActiveFrame(id);
    MemChannel::Address endOfPrevious = getEndOfFrame(id);
    MemChannel::writeULong(id, endOfPrevious, rAdd + Op::INSTRUCTION_SIZE); //no return, scope increase
    MemChannel::writeULong(id, endOfPrevious + 4, previousFrameStart); //prior frame
    MemChannel::writeULong(id, endOfPrevious + 8, endOfPrevious + STACK_FRAME_HEADER_SIZE);  //end of frame
    MemChannel::writeULong(id, endOfPrevious + 12, 0); //Error handler not set
  }
  byte pushVariable(MemChannel::ChannelID id, char type){
    byte varIndex = countVars( id );
    byte vsize = Op::typeSize(type);
    byte buf[vsize];
    MemChannel::Address eof = getEndOfFrame( id );
    MemChannel::writeChar( id, eof, type );
    MemChannel::writeBytes( id, eof + 1, buf, vsize);
    setEndOfFrame( id, eof + 1 + vsize );
    return varIndex;
  }
  byte pushVariable(MemChannel::ChannelID id, char type, byte* data){
    byte varIndex = countVars( id );
    byte vsize = Op::typeSize(type);
    MemChannel::Address eof = getEndOfFrame( id );
    MemChannel::writeChar( id, eof, type );
    MemChannel::writeBytes( id, eof + 1, data, vsize);
    setEndOfFrame( id, eof + 1 + vsize );
    return varIndex;
  }
  byte pushArray(MemChannel::ChannelID id, unsigned int elements, char type){ //a3i123 array, 3 elements, int, data
    byte varIndex = countVars( id );
    byte tsize = Op::typeSize( type );
    byte buf[tsize];
    MemChannel::Address eof = getEndOfFrame( id );
    MemChannel::writeChar( id, eof, 'a' );
    MemChannel::writeUInt( id, eof+1, elements );
    MemChannel::writeChar( id, eof+3, type );
    for(unsigned int i = 0; i<elements; i++)
      MemChannel::writeBytes( id, eof + 4 + i*tsize, buf, tsize);
    setEndOfFrame( id, eof + 4 + tsize*elements );
    return varIndex;
  }
  MemChannel::Address popFrame(MemChannel::ChannelID id){
    MemChannel::Address activeFrame = getActiveFrame(id);
    MemChannel::Address rAdd = MemChannel::readULong(id, activeFrame );
    MemChannel::Address pFrame = MemChannel::readULong(id, activeFrame + 4);
    setActiveFrame( id, pFrame );
    setEndOfFrame( id, activeFrame );
  }
  bool hasPriorFrame(MemChannel::ChannelID id){
    return getActiveFrame > STACK_HEADER_SIZE;
  }
  
  void setActiveFrame(MemChannel::ChannelID id, MemChannel::Address address){
    MemChannel::writeULong(id, address - STACK_HEADER_SIZE, address);
  }
  MemChannel::Address getActiveFrame(MemChannel::ChannelID id){
    return MemChannel::readULong(id, 0);
  }
  
  //on top frame only
  byte countVars(MemChannel::ChannelID id){
    byte out = 0;
    MemChannel::Address c = getActiveFrame( id ) + STACK_FRAME_HEADER_SIZE;
    MemChannel::Address eof = getEndOfFrame( id );
    while(c < eof){
      out++;
      if(out==255){
        Serial.println("Warning: Oversized stack frame");
        return 255;
      }

      c += sizeOfVar( id, c );
    }
    return out;
  }
  void setEndOfFrame(MemChannel::ChannelID id, MemChannel::Address address){
    MemChannel::writeULong(id, getActiveFrame(id)+8, address );
  }
  MemChannel::Address getEndOfFrame(MemChannel::ChannelID id){
    return MemChannel::readULong(id, getActiveFrame(id)+8);
  }
  
  MemChannel::Address getVarAddress(MemChannel::ChannelID id, byte varID){
    byte n = 0;
    MemChannel::Address c = getActiveFrame( id ) + STACK_FRAME_HEADER_SIZE;
    MemChannel::Address eof = getEndOfFrame( id );
    while((n < varID) && (c < eof)){
      n++;
      if(n==255){
        Serial.println("Warning: Oversized stack frame");
        return 0;
      }

      c += sizeOfVar( id, c );
      
    }
    return c;
  }
  byte sizeOfVar(MemChannel::ChannelID id, MemChannel::Address addrs){
    char vType = MemChannel::readChar( id, addrs );
    byte s = Op::typeSize(vType);
    if(s==0){
      return MemChannel::readUInt(id, addrs+1) + ( vType=='a'? 2 : 1 );
    }else{
      return s + 1;
    }
  }
  MemChannel::Address getProgramCounter(MemChannel::ChannelID id){
    return MemChannel::readULong( id, 4);
  }
  void stepProgramCounter(MemChannel::ChannelID id){
    jumpProgramCounter( id, getProgramCounter(id) + 1 );
  }
  /**
   * Jump target is the position of the next instruction to be executed divided by OpCodes::INSTRUCTION_SIZE
   * This function just writes the PC directly
   */
  void jumpProgramCounter(MemChannel::ChannelID id, MemChannel::Address jumpTarget){
    MemChannel::writeULong( id, 4, jumpTarget );
  }
  MemChannel::Address getErrorHandler(MemChannel::ChannelID id){
    return MemChannel::readULong( id, getActiveFrame(id)+12 );
  }
  void setErrorHandler(MemChannel::ChannelID id, MemChannel::Address fAddress){
     MemChannel::writeULong(id, getActiveFrame(id) + 12, fAddress);
  }
}
