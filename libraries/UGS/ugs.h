/*
 * ugs.h
 *
 *  Created on: 15 août 2015
 *      Author: gael
 */

#ifndef UGS_H_
#define UGS_H_


#include "pins_arduino.h"
#include <Arduino.h>
#include <avr/pgmspace.h>

#include <EEPROM.h>

#include "iconspage.h"
#include "settings.h"
#include "ugsserial.h"
#include "ledanimation.h"


class Ugs
{
	public:
		static int begin();
		static void Loop();

		static void IncrementVolume();
		static void DecrementVolume();
		static void SetVolume(uint8_t volume);
		static void SetToStandby();
		static void StartToMainMenu();
		static void SetInput(uint8_t input);
		static void ProcessLoopTime(long StartMillis);

	private:
		static void ProcessTouchEvents();
		static int8_t ProcessEncoder();
		static bool ProcessPush();
		static void ProcessIR();
		static void ProcessSerial();
		static void ProcessLedAnimations();
		static void ProcessVuMeter();
		static void ProcessDacStatus();

		static unsigned long lastWriteLoopTime;
};




#endif /* UGS_H_ */
