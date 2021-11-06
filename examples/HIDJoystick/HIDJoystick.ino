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
#include "limits.h"

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
SBCController SBC(myusb); 
USBHIDParser hid1(myusb);

USBDriver *drivers[] = {&hub1, &hub2, &hid1};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "Hub2", "HID1"};
bool driver_active[CNT_DEVICES] = {false, false, false};

//this gets called once data is transferred and appropriately copied.
void rx_callback(const Transfer_t *transfer)
{
/*
  Serial.print(SBC.getAimingX());
  Serial.print(" ");
  Serial.print(SBC.getAimingY());
  Serial.print(" ");
  Serial.print(SBC.getRotationLever());
  Serial.print(" ");
  Serial.print(SBC.getLeftPedal());
  Serial.print(" ");
  Serial.print(SBC.getSightChangeX());
  Serial.print(" ");
  Serial.println(SBC.getSightChangeX());*/
  
  Joystick.X(SBC.getAimingX());
  Joystick.Y(SBC.getAimingY());
  Joystick.Z(map(SBC.getRotationLever(),-512,512,0,1023));
  Joystick.Zrotate(SBC.getLeftPedal());
  Joystick.sliderLeft(map(SBC.getSightChangeX(),-512,512,0,1023));
  Joystick.sliderRight(map(SBC.getSightChangeY(),-512,512,0,1023));
  for(int i=0;i<32;i++)  
      Joystick.button(i + 1, SBC.getButtonState(i));
    
  Joystick.send_now();
  //minimum time between Polls is 3.  Lower than this and we can't send lightpacket as well.
}


void setup()
{
  Serial1.begin(2000000);
  //while (!Serial) ; // wait for Arduino Serial Monitor
  Joystick.useManualSend(true);
  
  SBC.data_received = rx_callback;
  myusb.begin();

  
  SBC.SetAllLEDs(SBC.minLightIntensity,true);
  myusb.Task();

  //make it so that it turns lights on incrementally like a startup sequence
  for(uint8_t i =SBC.lowestLightVal;i<SBC.highestLightVal;i++)
  {
    SBC.SetLEDState((SBCController::ControllerLEDEnum)i, SBC.maxLightIntensity, true); 
    myusb.Task();
    delay(50);
  }

  SBC.SetAllLEDs(SBC.minLightIntensity,true);
  myusb.Task();
}

void loop()
{
myusb.Task();
}

