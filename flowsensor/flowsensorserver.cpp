#include <SPI.h>
#include <../RadioHead/RH_RF95.h>
#include "../comms/SerialCommunicator.h"
#include "flowsensorserver.h"

// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 
// 868MHz.This works but it is unknown how well the radio configures to this frequency:
//float frequency = 864.1;
const float FREQUENCY = 921.2;
const int LED = 13; //Status LED on pin 13
const int POWER = 14;

FlowSensorServer::FlowSensorServer()
{

    
}

FlowSensorServer::~FlowSensorServer()
{

}

void FlowSensorServer::Start()
{
    pinMode(LED, OUTPUT);
    //Initialize the Radio. 
    if (!mRF95.init())
    {
        SerialUSB.println("Radio Init Failed - Freezing");
        digitalWrite(LED, HIGH);
        while (1);
    }
    else
    {
        // An LED indicator to let us know radio initialization has completed.
        SerialUSB.println("Receiver up!");
        digitalWrite(LED, HIGH);
        delay(500);
        digitalWrite(LED, LOW);
        delay(500);
    }

    mRF95.setFrequency(FREQUENCY); 

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
    // you can set transmitter powers from 5 to 23 dBm:
    // Transmitter power can range from 14-20dbm.
    mRF95.setTxPower(POWER, false);
    mComms = new SerialCommunicator(mRF95);
}

void FlowSensorServer::Send(FlowPacket& packet)
{
    mComms->send(packet);
}

bool FlowSensorServer::Recv(FloatPacket& packet)
{
    return mComms->recv(packet);
}
