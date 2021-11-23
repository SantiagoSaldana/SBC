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
#include <XInput.h>
#include <USBHost_t36.h>
#include "SBC.h"
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

bool jumpPressed = false;
bool ejecting = false;//can't deal with other buttons using start or back if ejecting
uint16_t jumpThreshold = 512;
uint16_t aimingThreshold = 400;
uint16_t aimingMidPoint = 512;
int8_t lastDpadX = 0;
int8_t lastDpadY = 0;//will be -1,0,1

uint8_t leftRumble = 0;
uint8_t rightRumble = 0;

uint16_t timeBetweenCommLights = 0;//set dynamically

unsigned long currentMillis;

uint8_t startingLight = (uint8_t)ControllerLEDEnum::Comm1;
uint8_t totalLights = 5;
uint8_t lightIndex = 0;
uint8_t currentLight = startingLight;
bool finishSequence = false;
uint8_t lightIntensity = 0;


//For reference, here are all the buttons
/*
 * enum class ButtonEnum {
  RightJoyMainWeapon,
  RightJoyFire,
  RightJoyLockOn,
  Eject,
  CockpitHatch,
  Ignition,
  Start,
  MultiMonOpenClose,
  MultiMonMapZoomInOut,
  MultiMonModeSelect,
  MultiMonSubMonitor,
  MainMonZoomIn,
  MainMonZoomOut,
  FunctionFSS,
  FunctionManipulator,
  FunctionLineColorChange,
  Washing,
  Extinguisher,
  Chaff,
  FunctionTankDetach,
  FunctionOverride,
  FunctionNightScope,
  FunctionF1,
  FunctionF2,
  FunctionF3,
  WeaponConMain,
  WeaponConSub,
  WeaponConMagazine,
  Comm1,
  Comm2,
  Comm3,
  Comm4,
  Comm5,
  LeftJoySightChange,
  ToggleFilterControl,
  ToggleOxygenSupply,
  ToggleFuelFlowRate,
  ToggleBufferMaterial,
  ToggleVTLocation,
  TunerDialStateChange,
  GearLeverStateChange
};

enum class ControllerLEDEnum {
    Eject = 4,
    CockpitHatch = 5,
    Ignition = 6,
    Start = 7,
    OpenClose = 8,
    MapZoomInOut = 9,
    ModeSelect = 10,
    SubMonitorModeSelect = 11,
    MainMonitorZoomIn = 12,
    MainMonitorZoomOut = 13,
    ForecastShootingSystem = 14,
    Manipulator = 15,
    LineColorChange = 16,
    Washing = 17,
    Extinguisher = 18,
    Chaff = 19,
    TankDetach = 20,
    Override = 21,
    NightScope = 22,
    F1 = 23,
    F2 = 24,
    F3 = 25,
    MainWeaponControl = 26,
    SubWeaponControl = 27,
    MagazineChange = 28,
    Comm1 = 29,
    Comm2 = 30,
    Comm3 = 31,
    Comm4 = 32,
    Comm5 = 33,
//not sure what is missing here
    GearR = 35,
    GearN = 36,
    Gear1 = 37,
    Gear2 = 38,
    Gear3 = 39,
    Gear4 = 40,
    Gear5 = 41
};
*/

int8_t evalDPad(uint16_t axisVal, uint16_t midPoint)
{
    int16_t diff = axisVal - midPoint;
  if(abs(diff) > aimingThreshold)
    if(diff > 0)
    {
      return 1;//will press pos button
    }
    else
    {
      return -1;//will press neg button
    }
  else
    {
      return 0;
    }
}

//axisVal 0 - 1023
int8_t handleDPad(uint16_t axisVal,int8_t lastDPadVal, uint8_t negButton,  uint8_t posButton)
{
  int8_t currentVal = evalDPad(axisVal,aimingMidPoint);
  if(currentVal != lastDPadVal)
  {
    //take care of situation where currentVal = 0
    if(currentVal == 0)
    {
      if(lastDPadVal == 1)
        XInput.release(posButton);
      if(lastDPadVal == -1)
        XInput.release(negButton); 
    }     

    if(currentVal == 1)
      XInput.press(posButton);

    if(currentVal == -1)
      XInput.press(negButton);      
  }
return(currentVal);
}
    

void linkButton(ButtonEnum aButton,uint8_t xInputButton)
{
  //Serial.println(SBC.getButtonState(1));
if(SBC.buttonChanged(aButton))
{
  if(SBC.getButtonState(aButton))
    XInput.press(xInputButton);
  else
    XInput.release(xInputButton);
}
}

bool linkDoubleButton(ButtonEnum aButton,uint8_t xInputButton1,uint8_t xInputButton2)
{
  //Serial.println(SBC.getButtonState(1));
if(SBC.buttonChanged(aButton))
{
  if(SBC.getButtonState(aButton))
  {
    XInput.press(xInputButton1);
    XInput.press(xInputButton2);
    ejecting = true;
  }
  else
  {
    XInput.release(xInputButton1);
    XInput.release(xInputButton2);
    ejecting = false;
  }
  return true;
}
}

void linkTrigger(ButtonEnum aButton,XInputControl  trigger)
{
  const int TriggerMax = 255;  // uint8_t max
  
  //Serial.println(SBC.getButtonState(1));
  if(SBC.buttonChanged(aButton))
  {
    if(SBC.getButtonState(aButton))
      XInput.setTrigger(trigger, TriggerMax);
    else
      XInput.setTrigger(trigger, 0);
  }
}

//this gets called once data is transferred and appropriately copied.
void rx_callback(const Transfer_t *transfer)
{
  
/*linkTrigger(ButtonEnum::RightJoyFire,TRIGGER_RIGHT);
linkTrigger(ButtonEnum::RightJoyMainWeapon,TRIGGER_LEFT);*/
linkDoubleButton(ButtonEnum::Eject,BUTTON_BACK,BUTTON_START);
if(!ejecting)
{
  linkButton(ButtonEnum::Start,BUTTON_START);//menu
  linkButton(ButtonEnum::Ignition,BUTTON_BACK);//ignition
}
linkButton(ButtonEnum::RightJoyLockOn,BUTTON_Y);//ignition
linkButton(ButtonEnum::CockpitHatch,BUTTON_B);//horn
linkButton(ButtonEnum::LeftJoySightChange,BUTTON_R3);//horn

lastDpadX = handleDPad(SBC.getAimingX(),lastDpadX,DPAD_LEFT,DPAD_RIGHT);
lastDpadY = handleDPad(SBC.getAimingY(),lastDpadY,DPAD_UP,DPAD_DOWN);//yaxis vals were flipped, so changed order

XInput.setJoystick(JOY_LEFT,SBC.getRotationLever()*2.0f,-1*SBC.getMiddlePedal() + SBC.getRightPedal());
XInput.setJoystick(JOY_RIGHT,SBC.getSightChangeX(),SBC.getSightChangeY());

uint16_t leftPedalVal = SBC.getLeftPedal();

if(leftPedalVal > jumpThreshold && !jumpPressed)
{
  XInput.press(BUTTON_A);
  jumpPressed = true;
}

if(leftPedalVal <= jumpThreshold && jumpPressed)
{
  XInput.release(BUTTON_A);
  jumpPressed = false;
}


if(SBC.getMiddlePedal() > 250)
XInput.setTrigger(TRIGGER_LEFT, map(SBC.getMiddlePedal(),0,1023,0,255));

if(SBC.getRightPedal() > 250)
XInput.setTrigger(TRIGGER_RIGHT, map(SBC.getRightPedal(),0,1023,0,255));


XInput.send();

}

void rumbleCallback(uint8_t packetType) {
  // If we have an LED packet (0x01), do nothing
  if (packetType == (uint8_t) XInputReceiveType::LEDs) {
    return;
  }

  // If we have a rumble packet (0x00), see our rumble data on the LED
  else if (packetType == (uint8_t) XInputReceiveType::Rumble) 
  {
    rightRumble = XInput.getRumbleRight();
    leftRumble = XInput.getRumbleLeft();
  }
}


void setup()
{
  Serial1.begin(2000000);
  //while (!Serial) ; // wait for Arduino Serial Monitor
  
  
  SBC.data_received = rx_callback;
  myusb.begin();

  
  SBC.SetAllLEDs(SBC.minLightIntensity,true);
  myusb.Task();

  //make it so that it turns lights on incrementally like a startup sequence
  for(uint8_t i =SBC.lowestLightVal;i<SBC.highestLightVal;i++)
  {
    SBC.SetLEDState((ControllerLEDEnum)i, SBC.maxLightIntensity, true); 
    myusb.Task();
    delay(50);
  }

  SBC.SetAllLEDs(SBC.minLightIntensity,true);
  myusb.Task();

  XInput.setAutoSend(false);
  XInput.setRange(JOY_LEFT,-1023,1023);
  XInput.setRange(JOY_RIGHT,-450,450);//shortening range since left joystick isn't greatest on SBC
  
  // Set callback function. Function must have a 'void' return type
  // and take a single uint8_t as an argument
  XInput.setReceiveCallback(rumbleCallback);
  XInput.begin();
  
}

void handleLeftRumble()
{
  if(leftRumble > 0)
  {
    timeBetweenCommLights = 200 - (leftRumble / 2);//leftRumble max = 255, /2 = 127, higher rumble = 
    lightIntensity = map(leftRumble,0,255,SBC.minLightIntensity,SBC.maxLightIntensity);
    if(millis() - currentMillis > timeBetweenCommLights)
    {
      int8_t prevLight = lightIndex - 1;
      if(lightIndex < 0)
        prevLight = totalLights - 1;
  
      SBC.SetLEDState((ControllerLEDEnum) (startingLight+prevLight), SBC.minLightIntensity, true);  
  
      SBC.SetLEDState((ControllerLEDEnum) (startingLight+lightIndex), lightIntensity, true);
        
      lightIndex++;
      
      if(lightIndex > totalLights)
      {
        lightIndex = 0;
        leftRumble = 0;//at this point we should be done with the sequence
        finishSequence = true;
      }
      currentMillis = millis();
    }
  }
  else
  {
    if(finishSequence && (millis() - currentMillis > timeBetweenCommLights))
    {
      SBC.SetLEDState((ControllerLEDEnum) (startingLight), SBC.minLightIntensity, true);  
      finishSequence = false;
    }
  }
}




void loop()
{
  handleLeftRumble();

//Serial.println(SBC.getAimingX());
//XInput.setJoystick(JOY_LEFT, SBC.getAimingX(), 1023-SBC.getAimingY());  // flip y axis


myusb.Task();
}