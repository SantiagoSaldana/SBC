// HIDDeviceInfo - Simple HID device example
// 
// This Simple test sketch is setup to print out HID information about a device
// The other two tabs are a simple C++ subclass of the USBHIDInput class that is part 
// of the USBHost_t36 library.  
//
// This subclass simply tries to connect to each different HID object and
// the only thing it does is to try to print out all of the data it receives
// in a reasonable way. 
//
// The idea is that with the output from this sketch we can hopefully add support 
// for some additional devices that are not currently supported or allows you to 
// develop your own. 
// 
// You can use Serial Input to control how much data is displayed per each HID packet
// received by the sketch.
//
// By Default it displays both the RAW (Hex dump) of the data received, as well
// as the data as the HID interpreter walks through the data into the individual
// fields, which we then print out.  
//
// There are options to turn off some of this output, also an option that you can
// toggle on or off (C) to only try to show the changed fields.   
//
// This example is in the public domain

#include <USBHost_t36.h>
#include "SBCController.h"

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
SBCController SBC(myusb); 
USBHIDParser hid1(myusb);

USBDriver *drivers[] = {&hub1, &hub2, &hid1};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "Hub2", "HID1"};
bool driver_active[CNT_DEVICES] = {false, false, false};

bool lightSent = false;

uint8_t lightValStart = 35;
uint8_t lightValEnd = 41;
uint8_t currentLightVal = lightValStart;

//this gets called once data is transferred and appropriately copied.
void rx_callback(const Transfer_t *transfer)
{
/*
  Serial.print(SBC.getRotationLever());
  Serial.print(" ");
  Serial.print(SBC.getSightChangeX());
  Serial.print(" ");
  Serial.print(SBC.getSightChangeY());
  Serial.print(" ");
  Serial.print(SBC.getAimingX());  
  Serial.print(" ");
  Serial.print(SBC.getAimingY());  
  Serial.print(" ");
  Serial.print(SBC.getLeftPedal());    
  Serial.print(" ");
  Serial.print(SBC.getMiddlePedal());  
  Serial.print(" ");  
  Serial.print(SBC.getRightPedal());  
  Serial.print(" ");
  Serial.print(SBC.getTunerDial());      
  Serial.print(" ");
  Serial.print(SBC.getGearLever());      
  for(int i=0;i<39;i++)
  {
    Serial.print(SBC.getButtonState(i));
    Serial.print(" ");
  }
  Serial.println();
*/
  Joystick.X(SBC.getAimingX());
  Joystick.Y(SBC.getAimingY());
  Joystick.Z(SBC.getRotationLever());
  Joystick.Zrotate(SBC.getLeftPedal());
  Joystick.sliderLeft(SBC.getSightChangeX());
  Joystick.sliderRight(SBC.getSightChangeY());
  for(int i=0;i<32;i++)
  {
    if(SBC.getButtonState(i)) 
    {   
      Joystick.button(i + 1, 1);
      
      if(!lightSent)
      {
        Serial.println("sending light");
      SBC.SetLEDState(SBCController::ControllerLEDEnum::Eject, 0, false);  
      lightSent = true;
      }
    }
    else
      Joystick.button(i + 1, 0);
  }
    
  Joystick.send_now();

  for(int i =lightValStart;i<lightValEnd;i++)
    SBC.SetLEDState((SBCController::ControllerLEDEnum)i, 0, false);  
     
  SBC.SetLEDState((SBCController::ControllerLEDEnum)currentLightVal, 15, true);
  currentLightVal++;

  if(currentLightVal >= lightValEnd)
    currentLightVal = lightValStart;
  
  delay(100);
       
    
}

void setup()
{
  Serial1.begin(2000000);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Joystick.useManualSend(true);
  
  SBC.data_received = rx_callback;
  myusb.begin();
}

void loop()
{
  
  myusb.Task();
  delay(50);
}

