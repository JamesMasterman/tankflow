#ifndef MQTT_LOGGER
#define MQTT_LOGGER

#include <stdint.h>
#include "application.h"
#include <PubSubClient.h>

#define LED             7

class MQTTLogger
{

public:
  MQTTLogger();
  ~MQTTLogger();

public:
  void Setup(const char* server, int port, const char* attributes, const char* token);
  void Heartbeat();

protected:
  bool ServerConnect();
  void BlinkLED();
  bool EnsureConnected();

protected:
  char m_mqttPub[128];
  char* m_Token;
  TCPClient pubsub;
  PubSubClient* psClient;
};

#endif
