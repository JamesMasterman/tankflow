#ifndef FLOW_LOGGER
#define FLOW_LOGGER

#include <stdint.h>
#include "application.h"
#include <PubSubClient.h>

#define LED             7


class FlowLogger
{

public:
  FlowLogger();
  ~FlowLogger();


public:
  void Setup(const char* server, int port, const char* attributes, const char* token);
  void Send(double rate, double volume);

private:
  void SendData(double rate, double volume);
  void ServerConnect();
  void BlinkLED();

private:
  char m_mqttPub[128];
  char* m_Token;
  TCPClient pubsub;
  PubSubClient* psClient;

};

#endif
