/* SBC Controller class
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// This simple class does nothing, but print out information about the device it
// extracts from calls to claim... 

#ifndef __SBCController_h_
#define __SBCController_h_
#include <Arduino.h>
#include <USBHost_t36.h>


 //IntervalTimer pollTimer;
 
class SBCController : public USBDriver {
public:
	SBCController(USBHost &host) { init();}

 Device_t *currentDevice;
 
 

void setGearLights(bool update,int intensity);
static int getTotalButtons(){return 39;}
  
//DO NOT CHANGE ORDER
//THESE match the order they are presented in the USB packet
enum class ButtonEnum {
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

const uint8_t lowestLightVal = (uint8_t) ControllerLEDEnum::Eject;
const uint8_t highestLightVal = (uint8_t) ControllerLEDEnum::Gear5;
const uint8_t maxLightIntensity = 15;
const uint8_t minLightIntensity = 0;

uint16_t timeBetweenPolls = 50;//time in microseconds to delay

/// <summary>
    /// Corresponds to the "Rotation Lever" joystick on the left. range: -512 - 511
/// </summary>
int16_t getRotationLever();
int16_t getSightChangeX();
int16_t getSightChangeY();
uint16_t getAimingX();
uint16_t getAimingY();
uint16_t getLeftPedal();
uint16_t getMiddlePedal();
uint16_t getRightPedal();
uint8_t getTunerDial();
int8_t getGearLever();
bool getButtonState(uint8_t buttonVal);
void SetAllLEDs(uint8_t Intensity,bool refreshState);
void SetLEDState(ControllerLEDEnum LightId, uint8_t Intensity, bool refreshState);
void SetPollTime(uint16_t milliseconds);
static void StartPolling();
void StopPolling();

void sendLightDataPacket();

bool firstSent = false;

void RefreshLEDState() 
{
    sendLightDataPacket();
}




static const int rawControlDataLength = 26;


void (*data_received)(const Transfer_t *);

static const int rawLEDDataLength = 22;
byte rawLEDData[rawLEDDataLength];

  void pollDevice();//called by timer to poll device for status.

protected:
	virtual bool claim(Device_t *dev, int type, const uint8_t *descriptors, uint32_t len);
	virtual void disconnect() {};
	void init();

  uint8_t     rx_ep_ = 0; // remember which end point this object is...
  uint16_t    rx_size_ = 0;
  uint16_t    tx_size_ = 0;
  Pipe_t      *rxpipe_;
  Pipe_t      *txpipe_;
  uint8_t     rxbuf_[64]; // receive circular buffer
  uint8_t     txbuf_[64];   // buffer to use to send commands to joystick 
  

  static void rx_callback(const Transfer_t *transfer);
  static void tx_callback(const Transfer_t *transfer);
  void rx_data(const Transfer_t *transfer);
  void tx_data(const Transfer_t *transfer);
  Device_t *mydevice = NULL;

  //SBC related stuff

  enum POVdirection
  {
      CENTER,
      LEFT,
      RIGHT,
      UP,
      DOWN
  };
  POVdirection POVhat = POVdirection::CENTER;




  // The byte buffer that the raw LED data is stored

  bool updateGearLights = true;
  int gearLightIntensity = 15;

  const int _signedAxisMin = -512;
  const int _signedAxisMax = -511;

  const int _unsignedAxisMin = 0;
  const int _unsignedAxisMax = 1023;

  uint16_t pollTimeMicroSeconds = 10000;


  int16_t getSignedAxisValue(uint8_t firstIndex, uint8_t SecondIndex);
  uint16_t getAxisValue(uint8_t firstIndex, uint8_t SecondIndex);

 
  // The byte buffer that the raw control data is stored
  byte rawControlData[rawControlDataLength];

  /// <summary>
  /// Checks to see if the buton state has changed
  /// </summary>
  /// <param name="buf">The raw data buffer</param>
  /// <param name="bytePos">The byte position to check</param>
  /// <param name="maskValue">The mask value for that button/switch</param>
  /// <returns></returns>
  bool isStateChanged(uint8_t* buf, uint8_t bytePos, uint8_t maskValue) {
  return ((buf[bytePos] & maskValue) != (rawControlData[bytePos] & maskValue));
  }
  
  void GearLightsRefresh(uint8_t gearValue);
};
#endif

