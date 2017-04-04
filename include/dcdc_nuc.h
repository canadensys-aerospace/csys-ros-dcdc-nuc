// Copyright (c) 2017 by McGill Robotics.
// Written by Bei Chen Liu <bei.liu@mail.mcgill.ca>
// All Rights Reserved
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef DCDC_NUC_H_
#define DCDC_NUC_H_

#include <usb.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include <stdexcept>

// USB setting macros.
#define NUC_PID       0xd006
#define NUC_VID       0x04D8
#define USB_TIMEOUT    100
#define MAX_TRANSFER_SIZE 32

// CDC NUC communication protocol macros.
#define NUC_OUT_REPORT_IO_DATA  0x81
#define NUC_IN_REPORT_IO_DATA   0x82
#define NUC_OUT_REPORT_IO_DATA2 0x83
#define NUC_IN_REPORT_IO_DATA2  0x84

#define IN_REPORT_EXT_EE_DATA     0x31
#define OUT_REPORT_EXT_EE_READ    0xA1
#define OUT_REPORT_EXT_EE_WRITE   0xA2

#define TERMISTOR_CONSTS_COUNT 34

// Thermoresitior temperature converion constants.
unsigned int const TERMAL_CURVE[TERMISTOR_CONSTS_COUNT] = {
  (unsigned int) 0xB,
  (unsigned int) 0xE,
  (unsigned int) 0x13,
  (unsigned int) 0x19,
  (unsigned int) 0x1F,
  (unsigned int) 0x28,
  (unsigned int) 0x32,
  (unsigned int) 0x3E,
  (unsigned int) 0x4C,
  (unsigned int) 0x5D,
  (unsigned int) 0x6F,
  (unsigned int) 0x85,
  (unsigned int) 0x9D,
  (unsigned int) 0xB8,
  (unsigned int) 0xD6,
  (unsigned int) 0xF6,
  (unsigned int) 0x118,
  (unsigned int) 0x13C,
  (unsigned int) 0x162,
  (unsigned int) 0x188,
  (unsigned int) 0x1B0,
  (unsigned int) 0x1D6,
  (unsigned int) 0x1FC,
  (unsigned int) 0x222,
  (unsigned int) 0x246,
  (unsigned int) 0x268,
  (unsigned int) 0x289,
  (unsigned int) 0x2A8,
  (unsigned int) 0x2C5,
  (unsigned int) 0x2E0,
  (unsigned int) 0x2F9,
  (unsigned int) 0x310,
  (unsigned int) 0x325,
  (unsigned int) 0x339
};

// Date structure for all the IO date generated by the PSU.
struct Dcdc_Nuc_Data {
  bool protection_ok;   // Use unknown.
  bool not_protection_fault;  // Use unknown
  bool not_open_led;  // Use unknown.
  bool not_short_led;  // Use unknown.
  bool cfg1;  // Use unknown.
  bool cfg2;  // Use unknown.
  bool cfg3;  // Use unknown.
  bool control_frequency;  // Use unknown.
  bool not_power_switch;  // Use unknown.
  // 'false' for Dumb Mode and 'true' for Automotive Mode.
  bool mode;
  // Sense if the USB is connected, should be alwayd true.
  bool usb_sense;
  bool input_voltage_good;
  bool ignition_voltage_good;
  // State of the motherboard by measuring power output.
  bool mobo_alive_pout;
  bool ignition_raised;
  bool ignition_falled;
  bool output_enabled;
  bool thump_ouput_enabled;

  double input_voltage;
  double input_current;
  double output_voltage;
  double output_current;
  double output_power;
  // Enabled only when the output is enabled.
  double temperature;
  double ignition_voltage;
  double thump_voltage;

  // Delay between inputs good and startup sequence.
  unsigned int timer_init;
  unsigned int timer_ignition_to_output_on;
  unsigned int timer_thump_output_on_off;
  unsigned int timer_output_on_to_mobo_on_pulse;
  unsigned int timer_mobo_pulse_width;
  // Igntion detection disabled period after output enable.
  unsigned int timer_ignition_cancel;
  unsigned int timer_ignition_off_to_mobo_off_pulse;
  unsigned int timer_hard_off;
  unsigned int timer_input_voltage_count;
  unsigned int timer_ignition_voltage_count;

  // List of known states: [0] - Init; [1] - Low power; [2] - Off; [3] - Wait
  // ignition on to output on; [4] - Output on; [5] - Output on to motherboard
  // pulse on; [6] - Motherboard pulse on; [7] - On; [8] - Ignition off to
  // motherboard off; [9] - Hard off delay.
  unsigned int state_machine_state;
  unsigned char mode2;  // Use unknown
  unsigned char firmware_version_major;
  unsigned char firmware_version_minor;
};

/* USB communication wrappers */
class Dcdc_Nuc{
 public:
  // Default constuctor, connects and sets up the device on creation.
  Dcdc_Nuc();

  // Only public function, call to get all data from the PSU. See Dcdc_Nuc_Data
  // for more detail.
  struct Dcdc_Nuc_Data get_data();

  // Destructor, close the USB devide on exit.
  ~Dcdc_Nuc();

 private:
  // USB device handle used for all transation.
  struct usb_dev_handle * h_ = NULL;

  // Connect to USB device by PID and VID defined as macros.
  void connect_();

  // Clam the USB device and configure the device for communication.
  void setup_();

  // Send command to the PSU.
  // @return Number of bytes sent, return negative value on error.
  int send_(unsigned char *buff);

  // Try to receive data from the PSU.
  // @return Number of bytes received, return negative value on error.
  int recv_(unsigned char *buff, int size);

  unsigned int chars_to_uint_(unsigned char high, unsigned char low);
  // Convert thermisitor value to temperature.
  // @return temperature in degree Celcus.
  double therm_to_temp_(unsigned int termistor_value);
};

#endif  // DCDC_NUC_H_
