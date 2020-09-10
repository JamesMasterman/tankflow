#ifndef FLOW_METER
#define FLOW_METER

#include <stdint.h>

#define SENSOR_PIN A1

class FlowMeter
{

public:
    FlowMeter();
    ~FlowMeter();

public:
    float Read();
    float CurrentRate();
    unsigned long TotalVolume();
    void  PulseCounter();
    void Reset();

private:
    volatile long mLastPulseIRQ;
    volatile uint32_t mPulseCount;
    float mFlowRate;
    unsigned long mTotalLitres;
    unsigned long mLastTime;

};



#endif
