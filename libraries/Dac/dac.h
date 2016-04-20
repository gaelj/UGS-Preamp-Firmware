/*
 * dac.h
 *
 *  Created on: 31 oct. 2015
 *      Author: gael
 */

#ifndef DAC_H_
#define DAC_H_

// #include "pins_arduino.h"
#include <Arduino.h>
// #include <avr/pgmspace.h>

#define __spdif			2
#define __usb			3
#define __hostactive	4
#define __audiostream	5
#define __441			6
#define __48			7
#define __882			8
#define __96			9
#define __1764			10
#define __192			11

#define __baudrate		115200
#define __statusPerdiod	500

class Dac
{
	public:
		static int begin();
		static void Loop();

	private:
		static void WriteStatusToSerial();
		static void ProcessSerial();
		static void SetInput(bool value);

		static long lastStatusSend;
		static bool input;
};


#endif /* DAC_H_ */
