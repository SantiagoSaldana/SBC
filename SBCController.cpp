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

#include <Arduino.h>
#include <USBHost_t36.h>
#include "SBCController.h"

extern void dump_hexbytes(const void *ptr, uint32_t len);

static void println(const char *title, uint32_t val, uint8_t b = DEC) {
	Serial.print(title);
	Serial.println(val, b);	
}

static void print(const char *title, uint32_t val, uint8_t b = DEC) {
	Serial.print(title);
	Serial.print(val, b);	
}

void SBCController::init()
{
	driver_ready_for_device(this);
}

// Again this class is solely to display as much information about a device as we can...
// This all comes from the information passed to it through the claim method.
bool SBCController::claim(Device_t *dev, int type, const uint8_t *descriptors, uint32_t len) {
 println("\nSBCController claim this=", (uint32_t)this, HEX);


  // Don't try to claim if it is used as USB device or HID device
  if (mydevice != NULL) return false;
  if (device != nullptr) return false;
  // Try claiming at the interface level.
  if (type != 1) return false;

  //if (len < 9+7+7) return false;//don't understand this

  // Some common stuff for both XBoxs
  uint32_t count_end_points = descriptors[4];
  if (count_end_points < 2) return false;
  //if (descriptors[5] != 0xff) return false; // bInterfaceClass, 3 = HID
  rx_ep_ = 0;
  uint32_t txep = 0;
  uint8_t rx_interval = 0;
  uint8_t tx_interval = 0;
  rx_size_ = 0;
  tx_size_ = 0;
  uint32_t descriptor_index = 9; 
  if (descriptors[descriptor_index+1] == 0x22)  {
    if (descriptors[descriptor_index] != 0x14) return false; // only support specific versions...
    descriptor_index += descriptors[descriptor_index]; // XBox360w ignore this unknown setup...
  } 

 
  while ((rx_ep_ == 0) || txep == 0) {
    print("  Index:", descriptor_index, DEC);

    if (descriptor_index >= len) return false;      // we ran off the end and did not get end points
    // see if the next data is an end point descript
    if ((descriptors[descriptor_index] == 7) && (descriptors[descriptor_index+1] == 5)) {
      if ((descriptors[descriptor_index+3] == 3)        // Type 3...
        && (descriptors[descriptor_index+4] <= 64)
        && (descriptors[descriptor_index+5] == 0)) {
        // have a bulk EP size 
        if (descriptors[descriptor_index+2] & 0x80 ) {
          rx_ep_ = descriptors[descriptor_index+2];
          rx_size_ = descriptors[descriptor_index+4];
          rx_interval = descriptors[descriptor_index+6];
        } else {
          txep = descriptors[descriptor_index+2]; 
          tx_size_ = descriptors[descriptor_index+4];
          tx_interval = descriptors[descriptor_index+6];
        }
      }
    }
    descriptor_index += descriptors[descriptor_index];  // setup to look at next one...
  }

  if ((rx_ep_ == 0) || (txep == 0)) return false; // did not find two end points.
  print("JoystickController, rx_ep_=", rx_ep_ & 15);
  print("(", rx_size_);
  print("), txep=", txep);
  print("(", tx_size_);
  Serial.println(")\n");//print with no extra doesn't work.

  rxpipe_ = new_Pipe(dev, 3, rx_ep_ & 15, 1, rx_size_, rx_interval);
  if (!rxpipe_) return false;
  txpipe_ = new_Pipe(dev, 3, txep, 0, tx_size_, tx_interval);
  if (!txpipe_) {
    //free_Pipe(rxpipe_);
    return false;
  }
  rxpipe_->callback_function = rx_callback;
  queue_Data_Transfer(rxpipe_, rxbuf_, rx_size_, this);

  txpipe_->callback_function = tx_callback;
  Serial.print("got here");
 return true;
}

void SBCController::rx_callback(const Transfer_t *transfer)
{
  //Serial.println("got here");
    if (!transfer->driver) return;
  ((SBCController *)(transfer->driver))->rx_data(transfer);    
}

void SBCController::rx_data(const Transfer_t *transfer)
{
  print_hexbytes((uint8_t*)transfer->buffer, transfer->length);  
  queue_Data_Transfer(rxpipe_, rxbuf_, rx_size_, this);
}

void SBCController::tx_callback(const Transfer_t *transfer)
{
  /*
  if (!transfer->driver) return;
  ((SBCController *)(transfer->driver))->tx_data(transfer);
  */
}

void SBCController::tx_data(const Transfer_t *transfer)
{
}

