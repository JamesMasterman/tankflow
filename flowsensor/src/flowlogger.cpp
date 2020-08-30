#include "flowlogger.h"


FlowLogger::FlowLogger()
{
  psClient = new PubSubClient((Client &) pubsub);
}

FlowLogger::~FlowLogger()
{

}

void FlowLogger::Send(double rate, double volume)
{
    if (!psClient->connected())
    {
        ServerConnect();
    }

    SendData(rate, volume);
    psClient->loop();
}

void FlowLogger::SendData(double rate, double volume)
{
    // Prepare JSON payload
    snprintf(m_mqttPub, sizeof(m_mqttPub), "{\"flowrate\":%.1f,\"totalvolume\":%.0f}", rate, volume);
    Serial.println(m_mqttPub);

    // Send payload
    psClient->publish("v1/devices/me/telemetry", m_mqttPub);               // Topic, payload
    BlinkLED();
}

void FlowLogger::Setup(const char* server, int port, const char* attributes, const char* token)
{
   pinMode(LED, OUTPUT);
   psClient->setServer(server, port);
   m_Token = (char*)token;

   //Connect
   ServerConnect();

   //Send device attributes
   snprintf(m_mqttPub, sizeof(m_mqttPub), "%s", attributes);
   Serial.println(m_mqttPub);
   psClient->publish("v1/devices/me/attributes", m_mqttPub);
}

void FlowLogger::ServerConnect()
{
   while (!psClient->connected())
   {
       Serial.println("serverConnect(): Attempting to connect to Thingsboard server");
       if (psClient->connect("Photon", m_Token, NULL))                    // ClientID, User, PW
       {
           Serial.println("serverConnect(): Connected");
           return;
       }
       else
           Serial.println("serverConnect(): Connection failed, retry in 3 seconds");
       delay(3000);
   }
 }

void FlowLogger::BlinkLED()
{
    digitalWrite(LED,HIGH);
    delay(5);
    digitalWrite(LED,LOW);
    delay(5);
}
