#include "mqttlogger.h"


MQTTLogger::MQTTLogger()
{
  psClient = new PubSubClient((Client &) pubsub);
}

MQTTLogger::~MQTTLogger()
{

}

void MQTTLogger::Setup(const char* server, int port, const char* attributes, const char* token)
{
   pinMode(LED, OUTPUT);
   psClient->setServer(server, port);
   m_Token = (char*)token;

   //Connect
   if(ServerConnect())
   {
     //Send device attributes
     snprintf(m_mqttPub, sizeof(m_mqttPub), "%s", attributes);
     psClient->publish("v1/devices/me/attributes", m_mqttPub);
     psClient->loop();
     Serial.println("Sent attributes");
  }
}

bool MQTTLogger::EnsureConnected()
{
  if (!psClient->connected())
  {
      ServerConnect();
  }

  return psClient->connected();
}

bool MQTTLogger::ServerConnect()
{
   pubsub.flush();
   psClient->disconnect();
   bool connected = psClient->connected();
   while (!connected)
   {
       if (psClient->connect("Photon", m_Token, NULL))
       {
           Particle.publish("Did connect");
           //Serial.println("serverConnect(): Connected");
           connected = true;
           psClient->loop();
           break;
       }
       else
       {
           Particle.publish("Cannot connect");
           //Serial.println("serverConnect(): Connection failed, retry in 3 seconds");
       }

       delay(5000);
   }

   
   return connected;
 }

void MQTTLogger::BlinkLED()
{
    digitalWrite(LED,HIGH);
    delay(5);
    digitalWrite(LED,LOW);
    delay(5);
}
