/* USB Device Info class
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


class SBCController : public USBDriver {
public:
	SBCController(USBHost &host) { init();}

 Device_t *currentDevice;

void setGearLights(bool update,int intensity);
static int getTotalButtons(){return 39;}



/// <summary>
    /// Corresponds to the "Rotation Lever" joystick on the left. range: -512 - 511
/// </summary>
int16_t getRotationLever();
int16_t getSightChangeX();
int16_t getSightChangeY();
uint16_t getAimingX();
uint16_t getAimingY();


  static const int rawControlDataLength = 26;

  // The byte buffer that the raw control data is stored
  byte rawControlData[rawControlDataLength];

  void (*data_received)(const Transfer_t *);

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




  static const int rawLEDDataLength = 26;
  // The byte buffer that the raw LED data is stored
  
  byte rawLEDData[34];

  bool updateGearLights = true;
  int gearLightIntensity = 3;

  const int _signedAxisMin = -512;
  const int _signedAxisMax = -511;

  const int _unsignedAxisMin = 0;
  const int _unsignedAxisMax = 1023;


  int16_t getSignedAxisValue(uint8_t firstIndex, uint8_t SecondIndex);
  uint16_t getAxisValue(uint8_t firstIndex, uint8_t SecondIndex);

  

  
};
#endif
