#ifndef Theincgi_Stack_H
#define Theincgi_Stack_H

#include "MemChannel.h"

namespace Stack {
  const byte STACK_HEADER_SIZE = 8;
  const byte STACK_FRAME_HEADER_SIZE = 16;

  //create a new frame, used for scope level
  void pushFrame(MemChannel::ChannelID id);
  
  byte pushVariable(MemChannel::ChannelID id, char type);
  byte pushVariable(MemChannel::ChannelID id, char type, byte* data);
  byte pushArray(MemChannel::ChannelID id, unsigned int elements, char type);
  MemChannel::Address popFrame(MemChannel::ChannelID id); //returns 0 or next instruction to execute (should be immediatly after a label or function call, so it can't be 0 if it does have a return/loop)
  bool hasPriorFrame(MemChannel::ChannelID id);
  void pushReturnFrame(MemChannel::ChannelID id, MemChannel::Address rAdd);
  void setActiveFrame(MemChannel::ChannelID id, MemChannel::Address address);
  MemChannel::Address getActiveFrame(MemChannel::ChannelID id);
  MemChannel::Address getEndOfFrame(MemChannel::ChannelID id);
  void setEndOfFrame(MemChannel::ChannelID id, MemChannel::Address);
  MemChannel::Address getVarAddress(MemChannel::ChannelID channelID, byte varID);
  byte sizeOfVar(MemChannel::ChannelID id, MemChannel::Address addrs);
  MemChannel::Address getProgramCounter(MemChannel::ChannelID id);
  void stepProgramCounter(MemChannel::ChannelID id);
  byte countVars(MemChannel::ChannelID id); //on top frame only
  MemChannel::Address getErrorHandler(MemChannel::ChannelID id);
  void setErrorHandler(MemChannel::ChannelID id, MemChannel::Address);
  /**
   * Jump target is the position of the next instruction to be executed divided by OpCodes::INSTRUCTION_SIZE
   * PC is written directly, indicates instruction number, not address
   */
  void jumpProgramCounter(MemChannel::ChannelID id, MemChannel::Address jumpTarget);
}

#endif
