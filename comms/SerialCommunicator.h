#pragma once
#include "application.h"
#include "FlowPacket.h"
#include "../RadioHead/RH_RF95.h"

class SerialCommunicator
{
public:
  SerialCommunicator(RH_RF95& rf95);
  ~SerialCommunicator();


public:
  bool publish(uint8_t station, uint8_t packetType, char* data, uint8_t dataLen);

public:
  int  send(FlowPacket& packet);
  bool recv(FlowPacket& packet);
  int recvAck();
  void sendAck(uint8_t packetID);

private:
  void serialise(FlowPacket& packet, uint8_t* buffer);

private:
  
  uint8_t mPacketID;
  uint8_t mState;
  uint16_t mCalcCRC=0;
  uint16_t mRecvCRC=0;
  uint16_t mIndex;

  uint8_t mLastSendPacketID;
  uint8_t mLastAckPacketID;
  long mLastSendTime;

  FlowPacket packet;

  RH_RF95& mRF95;


};
