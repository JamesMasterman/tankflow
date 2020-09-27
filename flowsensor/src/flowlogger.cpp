#include "flowlogger.h"


FlowLogger::FlowLogger()
{

}

FlowLogger::~FlowLogger()
{

}

void FlowLogger::Send(double rate, double volume)
{
    if(!SendData(rate, volume))
    {
      psClient->loop();
      EnsureConnected();
      SendData(rate, volume);
    }
    psClient->loop();
}

bool FlowLogger::SendData(double rate, double volume)
{
    // Prepare JSON payload
    snprintf(m_mqttPub, sizeof(m_mqttPub), "{\"flowrate\":%.1f,\"totalvolume\":%.1f}", rate, volume);
    //Serial.println(m_mqttPub);

    // Send payload
    bool result = psClient->publish("v1/devices/me/telemetry", m_mqttPub);            // Topic, payload
    BlinkLED();
    return result;
}
