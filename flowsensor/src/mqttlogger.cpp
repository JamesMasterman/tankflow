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
  }
}

bool MQTTLogger::EnsureConnected()
{
  bool connected = true;
  if (!psClient->connected())
  {
      connected = ServerConnect();
  }

  return connected;
}

bool MQTTLogger::ServerConnect()
{
   int attempts = 0;
   bool connected = false;
   while (!psClient->connected() && attempts < 5)
   {
       attempts++;
       Serial.println("serverConnect(): Attempting to connect to Thingsboard server");
       if (psClient->connect("Photon", m_Token, NULL))
       {
           Serial.println("serverConnect(): Connected");
           connected = true;
           break;
       }
       else
       {
           Serial.println("serverConnect(): Connection failed, retry in 3 seconds");
       }
       psClient->loop();

       delay(3000);
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
