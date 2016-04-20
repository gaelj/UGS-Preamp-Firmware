/*
 * preamp.h
 *
 *  Created on: 28 janv. 2015
 *      Author: gael
 */

#ifndef PREAMP_H_
#define PREAMP_H_

#include <Arduino.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <MCP23S17.h>


#include "IRremote2.h"
#include "pinout.h"
#include "MUSES.h"
#include "settings.h"
#include "ledanimation.h"

#define __baudrate		115200



class Preamp
{
	public:
		static void begin();

		static void ReadVuMeters();
#ifdef _IRactive
		static String ProcessIR();
#endif
		static bool ProcessSerial(String &cmd, String &val);
		static int8_t ProcessEncoder();
		static long ProcessPush();

		static void InitializeSpiSlaves();
		static void SetIllumination_TFT(uint8_t level, bool log = true); // set backlight intensity from 0 to 10 (PWM)
		static void SetIllumination_Vol(uint8_t channel, uint8_t level); // set backlight intensity from 0 to 10 (PWM)
		static void SetIllumination_Rear(uint8_t channel, uint8_t level);// set backlight intensity from 0 to 10 (PWM)
		static void SetVolume(uint8_t volume);							 // set MUSES volume
		static void SetMute(bool mute);									 // set MUSES mute
		static void SetBalance(int16_t balance);						 // set MUSES balance L/R
		static void SetOffset(uint8_t channel_pair, int16_t offset);	 // set MUSES balance U/D
		static void SetPowerDelayed(uint8_t relay, bool state);			 // control the PSUs
		static void SetTriggerDelayed(uint8_t trigger, bool state);		 // control the triggers
		static void SetPower(uint8_t relay, bool state);				 // control the PSUs
		static void SetTrigger(uint8_t trigger, bool state);			 // control the triggers
		static void SetBypass(bool state);								 // control the bypass relays
		static void SelectInput(uint8_t input);							 // select the input according to menu order (0 to 5)
		static void SetInAsym(bool asym);								 // set preamp in assym (true) or sym (false)
		static void SetOutAsym(bool asym);								 // set preamp in assym (true) or sym (false)
		static void SetExtOut(bool ext);								 // set preamp out assym (true) or sym (false)
		static void SetVolumeLeds(uint16_t leds);						 // set volume leds bit by bit
		static void SetRearLedsAndRelays();								 // set rear leds according to buffered values
		static void ResetArduino();
		static void DisplayText(String Text);
		static void SetDacSource(bool source);							 // control the DAC source by serial
		static void DelayedActions();

		static uint8_t PreviousVolume;
		static bool MaxVolumeReached;
		static long DacLastSeenActive;
		static bool DacIsDown;
		static uint16_t VuMeter[2]; // 10 bits (0-1023)

	private:
		static MCP Mcp[_NbRlyBrds];
		static MUSES Muses[_NbVolBrds];
		static void SetSource(uint8_t source); 			// select the source from 0 to 5
		static void SetMuses(uint8_t volume);
		static void SweepMuses(uint8_t volume1, uint8_t volume2);

		static int16_t MusesVolumes[_NbVolBrds];		// volume setting per channel
		static byte Source;         					// selected input (physical); from 0 to 4
		static bool InAssym;							// input is assymetrical

		// button variables
		static bool curEncoderValue1; 			// buffered and unprocessed encoder wheel tick count
		static bool curEncoderValue2; 			// buffered and unprocessed encoder wheel tick count
		static bool curPush;		  			// unprocessed encoder push
		static long pushStartTime;		  		// encoder push time
		static bool longPushProcessed;

		// delayed relay / trigger requests
		static bool delayedTriggerRequestState[4];
		static bool delayedTriggerRequest[4];
		static bool delayedUnMuteRequest;

#ifdef _IRactive
		static IRrecv irrecv;
		static decode_results results;

		static uint16_t LedsAndRelayCache[_NbRlyBrds];

		// IR variables
		//static unsigned long myIRCodes[21];
		//static String myIRValues[21];
		static int codeType; // The type of code
		static unsigned long codeValue; // The code value if not raw
		//static unsigned int rawCodes[RAWBUF]; // The durations if raw
		static int codeLen; // The length of the code
		//static int toggle; // The RC5/6 toggle state

		static String myIRdecode(unsigned long c);
		static void storeCode(decode_results *results);
#endif
};



#endif /* PREAMP_H_ */
