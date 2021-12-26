#ifndef FLOW_LOGGER
#define FLOW_LOGGER

#include <stdint.h>
#include "application.h"
#include "PubSubClient.h"
#include "mqttlogger.h"

class FlowLogger: public MQTTLogger
{

public:
  FlowLogger();
  ~FlowLogger();

public:
  void Send(double rate, double volume);

private:
  bool SendData(double rate, double volume);
};

#endif
