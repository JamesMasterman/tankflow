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
    void Read();
    float CurrentRate();
    double TotalVolume();
    void  PulseCounter();
    void Reset();

private:
    volatile long mLastPulseIRQ;
    volatile uint32_t mPulseCount;
    volatile float mFlowRate;
    volatile double mTotalLitres;
    volatile unsigned long mLastTime;

};



#endif
