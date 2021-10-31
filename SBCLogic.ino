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


//this gets called once data is transferred and appropriately copied.
void rx_callback(const Transfer_t *transfer)
{
  Serial.println(SBC.getAimingX());
}

void setup()
{
  Serial1.begin(2000000);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("\n\nUSB HID Device Info Program");
  Serial.println("\nThis Sketch shows information about plugged in HID devices");
  Serial.println("\n*** You can control the output by simple character input to Serial ***");
  Serial.println("R - Turns on or off showing the raw data");
  Serial.println("C - Toggles showing changed data only on or off");
  Serial.println("<anything else> - toggles showing the Hid formatted breakdown of the data\n");

  SBC.data_received = rx_callback;
  myusb.begin();
}

void loop()
{
  myusb.Task();
  delay(50);
}
