//Very simple packet format
//- start packet
//- packet number
//- length of packet
//- packet data
//- checksum

#define ACK_PACKET_SIZE 2

typedef struct AckPacket_t{
  /**
   * intialiser for packet start
   */
  uint8_t init;

  /**
   * id of packet being acked
   */
  uint8_t packetID;

}  AckPacket;
