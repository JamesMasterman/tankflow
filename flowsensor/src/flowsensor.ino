#include <stdint.h>
#include "flowmeter.h"
#include "flowlogger.h"

const unsigned long ONE_MIN_MS = 60*1000;
const unsigned long WATCHDOG_TIMEOUT_MS = 15*ONE_MIN_MS; //timeout for watchdog
const unsigned long FLOW_READ_INTERVAL = 2000;
const uint8_t STATION_ID = 1;

//Output - 1Hz = 0.964LPM
FlowMeter* flowMeter;
unsigned long lastFlowReading;
FlowLogger* thingsboardLogger;

STARTUP(WiFi.selectAntenna(ANT_INTERNAL));
ApplicationWatchdog wd(WATCHDOG_TIMEOUT_MS, System.reset);

//Thingsboard settings
const char THINGSBOARD_SERVER[] = "demo.thingsboard.io";
const char DeviceAttributes[] = "{\"firmware_version\":\"1.5.2\",\"software_version\":\"1.0\"}";
#define THINGSBOARD_PORT        1883
#define TOKEN           "hh0SXDHA3vBbcOhsMynl"

void setup()
{
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Flow Server!");

    lastFlowReading = 0;
    SetupFlowMeter();
    SetupLogger();
}

void loop()
{
    //Tell the watchdog we are still alive
    wd.checkin();

    //Read the flow
    ReadFlow();
    PrintFlow();
    SendFlow();

    ResetIfMidnight();

    //Wait till next read
    delay(FLOW_READ_INTERVAL);
    Particle.process();
}

void SetupFlowMeter()
{
    flowMeter = new FlowMeter();
    // The Hall-effect sensor is connected to pin A1 which uses interrupt 0.
    // Configured to trigger on a FALLING state change (transition from HIGH
    // state to LOW state)
    attachInterrupt(SENSOR_PIN, PulseCounter, FALLING);
}

void SetupLogger()
{
    thingsboardLogger = new FlowLogger();
    thingsboardLogger->Setup(THINGSBOARD_SERVER, THINGSBOARD_PORT, DeviceAttributes, TOKEN);
}

void PulseCounter()
{
    flowMeter->PulseCounter();
}

void ReadFlow()
{
  detachInterrupt(SENSOR_PIN);
  flowMeter->Read();
  attachInterrupt(SENSOR_PIN, PulseCounter, FALLING);
}

void PrintFlow()
{
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    float rate = flowMeter->CurrentRate();
    unsigned int frac;

    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(rate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    frac = (rate - int(rate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.println("L/min");

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(flowMeter->TotalVolume());
    Serial.println("L");
}

void SendFlow()
{
    float rate = flowMeter->CurrentRate();
    unsigned long volume = flowMeter->TotalVolume();
    thingsboardLogger->Send(rate, (double)volume);
}

bool sensorsReset = false;
void ResetIfMidnight()
{
  //Reset all the sensors at midnight
  if(Time.hour(Time.now()) == 0 && !sensorsReset)
  {
    flowMeter->Reset();
    sensorsReset = true;
  }

  //Clear the sensor reset flag in the next hour
  if(sensorsReset && Time.hour(Time.now()) != 0)
  {
    sensorsReset = false;
  }
}
