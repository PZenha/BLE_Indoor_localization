/* 
	Editor: http://www.visualmicro.com
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: ESP32 Dev Module, Platform=esp32, Package=espressif
*/

#define __ESP32_ESp32__
#define __ESP32_ESP32__
#define ARDUINO 10805
#define ARDUINO_MAIN
#define F_CPU 240000000L
#define printf iprintf
#define __ESP32__
#define ESP_PLATFORM
#define HAVE_CONFIG_H
#define F_CPU 240000000L
#define ARDUINO 10805
#define ARDUINO_ESP32_DEV
#define ARDUINO_ARCH_ESP32
#define CORE_DEBUG_LEVEL 0
//
//
void FillCurrentBeaconArray(unsigned int major, unsigned int minor);

#include "pins_arduino.h" 
#include "arduino.h"
#include "ESP32_BLE_scan.ino"
