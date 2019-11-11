
#include "SerialCommunicator.h"
#include "checksum.h"
#include "FlowPacket.h"
#include "AckPacket.h"
#include "../RadioHead/RH_RF95.h"

#define START_PACKET 254


#define INIT 0
#define GOT_INIT 1
#define GOT_ID 2
#define GOT_TYPE 3
#define GOT_STATION 4
#define GOT_LENGTH 5
#define GOT_STRING 6
#define GOT_CRC 7
#define DONE 8

const long TIMEOUT = 2000;
const uint8_t MAX_RESENDS = 3;

SerialCommunicator::SerialCommunicator(RH_RF95& rf95)
{
  mPacketID = 0;
  mState = INIT;
  mLastAckPacketID = 0;
  mLastSendPacketID = 0;
  mLastSendTime = 0;
  mRF95 = rf95;
}

SerialCommunicator::~SerialCommunicator()
{

}

bool SerialCommunicator::publish(uint8_t station, uint8_t packetType, char* data, uint8_t dataLen){
  bool result = false;
  uint8_t resends = 0;

  //build packet
  packet.type = packetType;
  packet.station = station;
  memset(packet.data, 0, DATA_BUFFER_SIZE);
  memcpy(packet.data, data, dataLen);
  packet.dataLen = dataLen;

  while(!result){
    //Send packet if last one was acked or timed out
    if(mLastSendPacketID == mLastAckPacketID){
      mLastSendPacketID = send(packet);
      mLastSendTime = millis();
    }else{
      //check for ack instead
      if(recvAck() == mLastSendPacketID){
        mLastAckPacketID = mLastSendPacketID;
        result = true;
      }else{
        //Timeout waiting for ack, resend
        if((millis() - mLastSendTime) > TIMEOUT){
          mLastSendPacketID = mLastAckPacketID; //Trigger a resend
          resends++;
        }

        if(resends > MAX_RESENDS){
          break; //give up
        }
      }
    }
  }

  return result;
}

/**
 * Serialise a weather packet into a buffer ready to send
 */
void SerialCommunicator::serialise(FlowPacket& packet, uint8_t* buffer){
  //Very simple packet format
  //- start packet
  //- packet number
  //- length of packet
  //- packet data
  //- checksum

  if(mPacketID > 253){
    mPacketID = 1;
  }else{
    mPacketID++;
  }

  packet.init = START_PACKET;
  packet.packetID = mPacketID;

  uint8_t packetSize = sizeof(FlowPacket);
  memcpy(buffer, &packet, packetSize);

  uint16_t checkSum = crc_calculate(buffer, packetSize);
  buffer[packetSize] = (uint8_t)(checkSum & 0xFF);
  buffer[packetSize+1] = (uint8_t)(checkSum >> 8);
}

bool SerialCommunicator::recv(FlowPacket& packet)
{
  bool success = false;
  if (mRF95.waitAvailableTimeout(TIMEOUT))
  {
      // Should be a message for us now
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (mRF95.recv(buf, &len))
      {
        int c;
        int index = 0;

        // Read data from serial.
        //Response will be packet number and current time
        while(index < len && mState != DONE){
            c = buf[index];
            switch(mState){
              case INIT:
                if(c != START_PACKET){
                  mState = INIT;
                  return success;
                }
                packet.init = c;
                //clear out the state, new packet arriving
                mState = GOT_INIT;
                mIndex = 0;
                mCalcCRC = 0;
                mRecvCRC = 0;
                memset(packet.data, '\0', DATA_BUFFER_SIZE);

                //Begin initialising the CRC
                crc_init(&mCalcCRC);
                crc_accumulate((uint8_t)c, &mCalcCRC);
                break;
              case GOT_INIT:
                packet.packetID = c;
                mState = GOT_ID;
                crc_accumulate((uint8_t)c, &mCalcCRC);
                break;
              case GOT_ID:
          packet.type = c;
          mState = GOT_TYPE;
          crc_accumulate((uint8_t)c, &mCalcCRC);
          break;
        case GOT_TYPE:
                packet.station = c;
                mState = GOT_STATION;
                crc_accumulate((uint8_t)c, &mCalcCRC);
                break;
              case GOT_STATION:
                packet.dataLen = c;
                mState = GOT_LENGTH;
                crc_accumulate((uint8_t)c, &mCalcCRC);
                break;
              case GOT_LENGTH:
                packet.data[mIndex] = c;
                mIndex++;
                crc_accumulate((uint8_t)c, &mCalcCRC);

                if(mIndex >= DATA_BUFFER_SIZE){
                  mState = GOT_STRING;
                }
                break;
              case GOT_STRING:
                mRecvCRC = c;
                mState = GOT_CRC;
                break;
              case GOT_CRC:
                mRecvCRC += (c << 8);
                mState = DONE;
                break;
            }
            i++;

            if(mState == DONE){
              if(mRecvCRC == mCalcCRC){
      sendAck(packet.packetID);
                success = true;
              }else{
                success = false;
              }

              mState = INIT;
            }
          }
        }
      }
    }

    return success;
}

int SerialCommunicator::send(FlowPacket& packet){
  const int buffLen = sizeof(FlowPacket) + 2; //2 extra bytes for the CRC
  uint8_t sendBuffer[buffLen];
  int c;

  //Very simple packet format
  //- start packet
  //- packet number
  //- length of packet
  //- packet data
  //- checksum

  memset(sendBuffer, 0, buffLen);
  serialise(packet, sendBuffer);

   // Send a reply
  mRF95.send(sendBuffer, buffLen);
  mRF95.waitPacketSent();
   
  return packet.packetID;
}

int SerialCommunicator::recvAck(){
    int packetID = -1;
    if(mRF95.available())
    {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      if (mRF95.recv(buf, &len))
      {
        int c;
        int index = 0;
        // Read data from serial.
        //Response will be packet number and current time
        while(index < len && mState != DONE) 
        {
          c = buf[index];
          switch(mState)
          {
            case INIT:
              if(c != START_PACKET)
              {
                mState = INIT;
                return packetID;
              }
              else
              {
                mState = GOT_INIT;
              }
              break;
            case GOT_INIT:
              packetID = c;
              mState = DONE;
              break;
          }
          index++;
        }
      }
    }

    if(mState == DONE){
      mState = INIT; //ready for next ack
    }

    return packetID;
}



void SerialCommunicator::sendAck(uint8_t packetID)
{
  bool result = false;
  AckPacket packet;
  const uint8_t packetSize = sizeof(AckPacket);
  uint8_t buffer[packetSize];

  memset(buffer, 0, packetSize);
  packet.packetID = packetID;
  packet.init = START_PACKET;
  memcpy(buffer, &packet, packetSize);

  mRF95.send(buffer, packetSize);
  mRF95.waitPacketSent();
}
