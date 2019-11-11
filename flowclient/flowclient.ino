#include "flowsensorclient.h"

const uint8_t STATION_ID = 1;

FlowSensorClient* client;

void setup() 
{
    SerialUSB.begin(9600);
    while(!SerialUSB);
    SerialUSB.println("Flow Client!");

    lastFlowReading = 0;
    SetupClient();
}

void loop() 
{
    FlowPacket packet;
    if(client->Recv(packet))
    {
       //read flow packet
    }
   
}

void SetupClient()
{
    server = new FlowSensorClient();
    server->Start();
}


void SendFlow()
{
    char result[30];
    sprintf(result, "rate=%0.2f,total=%0.2f", flowMeter->CurrentRate(), flowMeter->TotalVolume());
    serial->send(STATION_ID, result, sizeof(result));
}
