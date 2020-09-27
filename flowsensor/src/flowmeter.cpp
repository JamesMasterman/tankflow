#include "flowmeter.h"
#include<SPI.h>

//Output - 1Hz = 0.964LPM
const float CALIBRATION_FACTOR = 5.2;//0.964; //Empirical factor. 0.964 is manufacturer number

FlowMeter::FlowMeter()
{
    pinMode(SENSOR_PIN, INPUT_PULLUP);

    mPulseCount        = 0;
    mFlowRate          = 0.0;
    mTotalLitres       = 0;
    mLastTime          = 0;
    mLastPulseIRQ      = 0;
}

FlowMeter::~FlowMeter()
{
    detachInterrupt(SENSOR_PIN);
}

void FlowMeter::Read()
{
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    unsigned long interval = (millis() - mLastTime)/1000;
    mFlowRate = (((double)mPulseCount/(double)interval)) * CALIBRATION_FACTOR;

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this interval
    double litres = (mFlowRate / 60.0) * (double)interval;
    mTotalLitres += litres;

    // Reset the pulse counter so we can start incrementing again
    mPulseCount = 0;
    mLastTime = millis();
}

float FlowMeter::CurrentRate()
{
    return mFlowRate;
}

double FlowMeter::TotalVolume()
{
    return mTotalLitres;
}

void FlowMeter::Reset()
{
  mTotalLitres = 0;
}

void FlowMeter::PulseCounter()
{
  if (millis() - mLastPulseIRQ > 10) // Ignore switch-bounce glitches less than 10ms after the reed switch closes
  {
    mLastPulseIRQ = millis();
    mPulseCount++;
  }
}
