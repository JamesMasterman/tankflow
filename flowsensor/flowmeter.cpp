#include "flowmeter.h"
#include<SPI.h>

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
//Output - 1Hz = 0.964LPM
const float CALIBRATION_FACTOR = 0.964;

FlowMeter::FlowMeter()
{
    pinMode(SENSOR_PIN, INPUT);
    digitalWrite(SENSOR_PIN, HIGH);

    mPulseCount        = 0;
    mFlowRate          = 0.0;
    mFlowMl            = 0;
    mTotalMl           = 0;
    mLastTime          = 0;
}

FlowMeter::~FlowMeter()
{
    
}

float FlowMeter::Read()
{
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    unsigned long interval = (millis() - mLastTime)/1000;
    mFlowRate = (pulseCount/interval) * CALIBRATION_FACTOR;
    mLastTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval
    unsigned long litres = (mFlowRate / 60) * interval;
    mTotalLitres += litres;
      
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;

    return mFlowRate;
}

float FlowMeter::CurrentRate()
{
    return mFlowRate;
}

float FlowMeter::TotalVolume()
{
    return mTotalLitres;
}

void FlowMeter::PulseCounter()
{
    pulseCount++;
}