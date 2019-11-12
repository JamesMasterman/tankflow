#include <stdint.h>
#include "flowmeter.h"
#include "flowsensorserver.h"

const unsigned long FLOW_READ_INTERVAL = 1000;
const uint8_t STATION_ID = 1;

//Output - 1Hz = 0.964LPM
FlowMeter* flowMeter;
unsigned long lastFlowReading;
FlowSensorServer* server;

void setup() 
{
    SerialUSB.begin(9600);
    while(!SerialUSB);
    SerialUSB.println("Flow Server!");

    lastFlowReading = 0;
    SetupFlowMeter();
    SetupServer();
}

void loop() 
{
    if(millis() - lastFlowReading > FLOW_READ_INTERVAL)
    {
       ReadFlow();
       //SendFlow();
       lastFlowReading = millis();
    }
}

void SetupFlowMeter()
{
    flowMeter = new FlowMeter();
    // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
    // Configured to trigger on a FALLING state change (transition from HIGH
    // state to LOW state)
    attachInterrupt(SENSOR_PIN, PulseCounter, FALLING);
}

void SetupServer()
{
    server = new FlowSensorServer();
    server->Start();
}

void PulseCounter()
{
    flowMeter->PulseCounter();
}

void ReadFlow()
{
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    
        
    float rate = flowMeter->Read();

    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    SerialUSB.print("Flow rate: ");
    SerialUSB.print(int(rate));  // Print the integer part of the variable
    SerialUSB.print(".");             // Print the decimal point
    frac = (rate - int(rate)) * 10;
    SerialUSB.print(frac, DEC) ;      // Print the fractional part of the variable
    SerialUSB.println("L/min");
  
    // Print the cumulative total of litres flowed since starting
    SerialUSB.print("  Output Liquid Quantity: ");             
    SerialUSB.print(flowMeter->TotalVolume());
    SerialUSB.println("L"); 

    // Enable the interrupt again now that we've finished reading output
    attachInterrupt(SENSOR_PIN, PulseCounter, FALLING);
   
}

void SendFlow()
{
    char result[30];
    sprintf(result, "rate=%0.2f,total=%0.2f", flowMeter->CurrentRate(), flowMeter->TotalVolume());
  //  server->Send(STATION_ID, result, sizeof(result));
}
