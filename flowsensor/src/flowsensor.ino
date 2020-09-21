#include <stdint.h>
#include "flowmeter.h"
#include "flowlogger.h"

const unsigned long ONE_MIN_MS = 60*1000;
const unsigned long WATCHDOG_TIMEOUT_MS = 15*ONE_MIN_MS; //timeout for watchdog
const unsigned long LOOP_TIME_MS = 15000;

FlowMeter* flowMeter;
FlowLogger* logger;

STARTUP(WiFi.selectAntenna(ANT_INTERNAL));
ApplicationWatchdog wd(WATCHDOG_TIMEOUT_MS, System.reset);
SYSTEM_THREAD(ENABLED);

//Thingsboard settings
const char THINGSBOARD_SERVER[] = "192.168.1.8";
const char DeviceAttributes[] = "{\"firmware_version\":\"1.5.2\",\"software_version\":\"1.0\"}";
#define THINGSBOARD_PORT        1883
#define TOKEN           "LluAjG0opsXDFjOTWcg1"

unsigned long lastSend = 0;
double lastVolume = 0;

void setup()
{
    /*Serial.begin(9600);
    while(!Serial);
    Serial.println("Flow Server!");*/

    SetupFlowMeter();
    SetupLogger();
    SendFlow();
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
    logger = new FlowLogger();
    logger->Setup(THINGSBOARD_SERVER, THINGSBOARD_PORT, DeviceAttributes, TOKEN);
}

void loop()
{
    unsigned long start = millis();

    //Tell the watchdog we are still alive
    wd.checkin();

    //Read the flow
    ReadFlow();
    //PrintFlow();

    if(ShouldSend())
    {
      SendFlow();
      lastSend = millis();
    }

    ResetIfMidnight();

    //Wait till next read
    unsigned long readDiff = millis() - start;
    delay(LOOP_TIME_MS - readDiff);
}

bool ShouldSend()
{
  unsigned long interval = millis() - lastSend;
  float volume = flowMeter->TotalVolume();
  if(volume > lastVolume)
  {
    lastVolume = volume;
    return true;
  }
  else
  {
    return false;
  }
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
  if(WiFi.ready())
  {
    float rate = flowMeter->CurrentRate();
    double volume = flowMeter->TotalVolume();
    logger->Send(rate, volume);
  }

  lastSend = millis();
}

bool sensorsReset = false;
void ResetIfMidnight()
{
  //Reset all the sensors at midnight
  if(Time.hour(Time.now()) == 0 && !sensorsReset)
  {
    flowMeter->Reset();
    lastVolume = 0;
    SendFlow();
    sensorsReset = true;
  }

  //Clear the sensor reset flag in the next hour
  if(sensorsReset && Time.hour(Time.now()) != 0)
  {
    sensorsReset = false;
  }
}
