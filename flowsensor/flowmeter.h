#ifndef FLOW_METER
#define FLOW_METER

const byte SENSOR_INTERUPT  = 0;  // 0 = digital pin 2
const byte SENSOR_PIN       = 2;


class FlowMeter
{

public:
    FlowMeter();
    ~FlowMeter();


public:
    float Read();
    float CurrentRate();
    float TotalVolume();
    void  PulseCounter();


private:
    volatile byte mPulseCount;
    float mFlowRate;
    unsigned long mTotalLitres;
    unsigned long mLastTime;

};



#endif