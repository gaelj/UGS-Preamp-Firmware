/*
 * ugsserial.h
 *
 *  Created on: 18 août 2015
 *      Author: gael
 */

#ifndef UGSSERIAL_H_
#define UGSSERIAL_H_


// preamp command keywords
// get
#define _CMD_GET_STATUS				F("GET_STATUS")
#define _CMD_GET_PRESETS			F("GET_PRESETS")
#define _CMD_GET_ALL_PRESETS		F("GET_ALL_PRESETS")
#define _CMD_GET_ICONS				F("GET_ICONS")
#define _CMD_GET_NB_RLYBRD			F("GET_RLYBRD")
#define _CMD_GET_NB_VOLBRD			F("GET_VOLBRD")

//  set
#define _CMD_VOLUME					F("VOLUME")
#define _CMD_STANDBY				F("STANDBY")
#define _CMD_RIAA_PWR				F("RIAA_PWR")
#define _CMD_MUTE					F("MUTE")
#define _CMD_BALANCE				F("BALANCE")
#define _CMD_OFFSET					  "OFFSET" // + channel number - not F() because used in a string concatenation
#define _CMD_INPUT					F("INPUT")
#define _CMD_ILLUM_TFT				F("ILLUM_TFT")
#define _CMD_ILLUM_VOL1				F("ILLUM_VOL1")
#define _CMD_ILLUM_VOL2				F("ILLUM_VOL2")
#define _CMD_ILLUM_RLY1				F("ILLUM_RLY1")
#define _CMD_ILLUM_RLY2				F("ILLUM_RLY2")
#define _CMD_PRESET					F("PRESET") // set the current preset group number
#define _CMD_EEPROM					F("EEPROM") // + preset group number => set preset's eeprom
#define _CMD_ICONS					F("ICONS")  // set full icon definition
#define _CMD_OUT_ASYM				F("OUT_ASYM") // TODO: rename to OUTASSYM
#define _CMD_EXT_OUT				F("EXTOUT")
#define _CMD_BYPASS					F("BYPASS")
#define _CMD_DACSRC					F("DACSRC")

// command
#define _CMD_RESET					F("RESET") // TODO: rename to RST_UGS
#define _CMD_RESET_ICONS			F("RST_ICONS")
#define _CMD_CUR_FACTORY			F("CUR_FACTORY") // TODO: rename to FACT_CUR
#define _CMD_ALL_FACTORY			F("ALL_FACTORY") // TODO: rename to FACT_ALL
#define _CMD_HELP					F("HELP")

// from ugs
#define _CMD_5V_PWR					F("5V_PWR")
#define _CMD_LOOPTIME				F("GET_LOOPTIME")
#define _CMD_DACSTATUS				F("DACSTATUS")
#define _CMD_ERROR					F("ERROR")


// internal commands for use with DAC (serial1)
#define _CMD_HOSTACTIVE				F("H")
#define _CMD_AUDIOSTREAMING			F("A")
#define _CMD_SAMPLERATE				F("P")
#define _CMD_DAC_SOURCE				F("S")


#include <Arduino.h>

class Ser
{
	public:
		static void SerialPrint(String key, String val);
		static void SerialPrint(String key, uint8_t val);
		static void SerialPrint(String key, uint16_t val);
		static void SerialPrint(String key, int16_t val);
		static void SerialPrint(String key, bool val);
		static void SerialPrintSep(String sep);
		static void SerialPrint(String key, byte val[]);
		static void SerialPrint(String key, long val);

		static bool DoSerialPrint;
};

#endif /* UGSSERIAL_H_ */
