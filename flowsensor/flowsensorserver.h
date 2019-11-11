#ifndef FLOW_SENSOR_SERVER
#define FLOW_SENSOR_SERVER

#include <SPI.h>
#include "../RadioHead/RH_RF95.h"
#include "../comms/SerialCommunicator.h"

class FlowSensorServer
{

public:
    FlowSensorServer();
    ~FlowSensorServer();

public:
    void Start();
    void Send(FlowPacket& packet);
    FlowPacket Recv();

private:
    // We need to provide the RFM95 module's chip select and interrupt pins to the 
    // rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
    RH_RF95 mRF95(12, 6);
    SerialCommunicator mComms;
};

#endif