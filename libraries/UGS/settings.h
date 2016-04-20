/*
 * PresetGroup.h
 *
 *  Created on: 17 août 2015
 *      Author: gael
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#define _NbRlyBrds    			2 	// defines the number of relay boards
#define _NbVolBrds    			2 	// defines the number of relay boards
#define USE_DISPLAY     		1
#define COM_TO_PC_BITRATE		115200

#define _IRactive					// activate IR remote decode functions (undef to save memory)
#define MAX_INPUTS				6	// maximum preamp input count (do not change)

#define _e2p_VersionNr 			1    // version flag (used to trigger eeprom reset to defauts)

#define _e2p_NbAddresses		208  // size of a set of settings in bytes
#define _e2p_NbPresets			4    // available sets of settings
#define _e2p_MaxPresetBytes		832
// The first 1024 EEPROM bytes are devoted to presets !

#define _e2p_IconStoreAddress	1024  // icon definition data in EEPROM
#define _e2p_MaxIconBytes		720
// The next 1024 bytes are reserved for icon definitions !

#define _e2p_CurrentPresetAddr	4094 // address of current preset number (0 to NbPreset-1)
#define _e2p_VersionAddr		4095 // address of version byte

#define debug_print_serial_set		// log "SET" items to serial

#define serial_dump_delay		300  // delay in ms after last serial command before sending eeprom contents by serial (improves responsivity when remote controled from the PC)



#include <pins_arduino.h>
#include <Arduino.h>
#include <EEPROM.h>

#include "EepromUtil.h"
#include "icon.h"
#include "preamp.h"
#include "ugsserial.h"
#include "ledanimation.h"

class Settings
{
public:
	static bool IsEepromValid();
	static void WriteDefaultValuesToAllPresets();
	static void DumpAllToSerial();

	Settings();
	Settings(uint8_t _groupNumber);

	static Settings CurrentPresetGroup;

	static void LoadCurrentActivePreset();	// load from e2p
	static void SetCurrentActivePresetNumber(uint8_t preset); // save to e2p

	static void SendStatusToSerial();	// write all status strings to serial
	static void DumpDataToSerial(String key, byte data[]);

	void WriteDefaultValuesToEeprom();

	void DumpSettingsToSerial();
	void RequestDumpToSerialDelayed();
	void DelayedActions();

	static long lastDumpToSerialRequestTimestamp;
	static bool DoDumpToSerial;

	bool ExtractValuesFromSerialString(uint8_t msgNumber, String s); // extracts to static variable deser
	void LoadValuesToSettings(byte deser[]);
	void WriteSettingsToEeprom();

	void ReadValuesFromEeprom();
	static String GetDacStatus();


	uint8_t groupNumber;

	static long loopTime; // UGS main loop time counter

	// settings (persisted to eeprom)

	static uint8_t Volume();

	static uint8_t current_input;
	static bool OutAssym;			// output is assymetrical
	static bool Mute;    			// selected mute
	static bool Bypass;    			// selected bypass
	static bool ExtOut;				// external board output

	static uint8_t VolIncrement;
	static unsigned long sleep_timeout;
	static bool sleep_active;

	static uint8_t Illumination_TFT_H;
	static uint8_t Illumination_Vol1_H;
	static uint8_t Illumination_Vol2_H;
	static uint8_t Illumination_Rly1_H;
	static uint8_t Illumination_Rly2_H;

	static uint8_t Illumination_TFT_L;
	static uint8_t Illumination_Vol1_L;
	static uint8_t Illumination_Vol2_L;
	static uint8_t Illumination_Rly1_L;
	static uint8_t Illumination_Rly2_L;

	static uint16_t touch_timeout; // ms
	static uint16_t hold_timeout;  // ms
	static uint16_t tft_sleep_timeout;   // s

	static uint8_t VolumeIn[6];

	static int16_t BalanceLR;

	static int16_t Offset[3];

	static uint8_t DefaultInput;
	static bool UseDefOrLastInput;

	static uint8_t DefaultVolume[6];

	static bool UseDefaultOrLastVol;

	static uint8_t input_icons[6];

	static uint8_t input_source[6];
	static bool input_trigger[6];

	static uint16_t frontColor;
	static uint16_t backColor;
	static uint16_t backColor2;
	static uint16_t selFrontColor;
	static uint16_t touchFrontColor;
	static uint16_t inactiveColor;

	static uint8_t PowerToggleDelay;		// in seconds

	static uint8_t VolumeSweepSpeed;
	static uint8_t TftSweepSpeed;

	static bool CommonVolume;

	static bool psu2_trigger[6];
	static bool ext1_trigger[6];
	static bool ext2_trigger[6];

	static bool UseInput[6];

	static uint8_t anim_id_v[NB_ANIMATION_STATES];
	static uint8_t anim_id_r[NB_ANIMATION_STATES];

	static uint8_t action_id_Rotary;
	static uint8_t action_id_Push;

	static uint8_t PowerOnMuteDelay;		// in seconds

	static uint8_t MinVolumes[6];
	static uint8_t MaxVolumes[6];

	static uint8_t AnimationStateDelays_v[NB_ANIMATION_STATES];  // x25 ms. 14 bytes * 5 = 70 bytes
	static uint8_t AnimationStateDelays_r[NB_ANIMATION_STATES];  // x25 ms. 14 bytes * 5 = 70 bytes

	static uint8_t AnimationStateRepeats_v[NB_ANIMATION_STATES]; // 14 bytes * 5 = 70 bytes
	static uint8_t AnimationStateRepeats_r[NB_ANIMATION_STATES]; // 14 bytes * 5 = 70 bytes

	static uint8_t VuOffsets[6];								 // VU offsets (400 + x)
	static uint8_t VuMultipliers[6];							 // VU multipliers (x / 4)



	// states (values not saved to e2p)
	static bool Power[2]; 						// power supply status by relay
	static bool Trigger[2]; 					// trigger by relay
	static unsigned long LastPowerToggle[4];	// last power toggle millis - power / trigger

	static int DacSamplingRate;
	/*
	 L1 		=  44.1 KHz
	 L2 		=  48   KHz
	 L3 		=  88.2 KHz
	 L4 		=  96   KHz
	 L5 		= 176.4 KHz
	 L6 		= 192   KHz
	 L3 + L5 = 352.8 kHz
	 L4 + L6 = 384   kHz
	 */
	static bool DacHostActive;
	static bool DacAudioStreaming;

	static uint16_t Illumination_TFT;
	static uint16_t Illumination_Vol1;
	static uint16_t Illumination_Vol2;
	static uint16_t Illumination_Rear1;
	static uint16_t Illumination_Rear2;

	// used to spare slow write operations. Contains a copy of the e2p values
	static byte Values[_e2p_NbAddresses];
	static byte deser[_e2p_NbAddresses];

private:
	void WriteValueToEeprom(int address, uint8_t value);

};


static const byte defaultValues[_e2p_NbAddresses] = {
// first => #0
	0,// _e2p_current_input  					3
	1,    // _e2p_OutAssym						1
	0,    // _e2p_Mute							1
	0,    // _e2p_Bypass						1
	2,    // _e2p_VolIncrement					3
//5
	255,  // _e2p_Illum_TFT_H					8
	255,  // _e2p_IllumVol1_H					8
	255,  // _e2p_IllumVol2_H					8
	255,  // _e2p_IllumRly1_H					8
	255,  // _e2p_IllumRly2_H					8
//10
	80,	  // _e2p_Illum_TFT_L					8
	80,	  // _e2p_IllumVol1_L					8
	80,	  // _e2p_IllumVol2_L					8
	80,   // _e2p_IllumRly1_L					8
	80,   // _e2p_IllumRly2_L					8
//15
	0, 100,    // _e2p_touch_timeout			16
	0x3, 0xE8, // _e2p_hold_timeout = 1000		16
	0, 30,     // _e2p_tft_sleep_timeout		16
//21
	170,  // _e2p_VolumeIn1						8
	170,  // _e2p_VolumeIn2						8
	170,  // _e2p_VolumeIn3						8
	170,  // _e2p_VolumeIn4						8
	170,  // _e2p_VolumeIn5						8
	170,  // _e2p_VolumeIn6						8
//27
	0, 0, // _e2p_BalanceLR						16
//29
	0, 0, // _e2p_Offset1						16
	0, 0, // _e2p_Offset2						16
	0, 0, // _e2p_Offset3						16
//35
	0,    // _e2p_DefaultInput					3
	1,    // _e2p_UseDefOrLastInput				1
//37
	170,  // _e2p_DefaultVolume1				8
	170,  // _e2p_DefaultVolume2				8
	170,  // _e2p_DefaultVolume3				8
	170,  // _e2p_DefaultVolume4				8
	170,  // _e2p_DefaultVolume5				8
	170,  // _e2p_DefaultVolume6				8
//43
	1,    // _e2p_UseDefaultOrLastVol			1
//44
	ICONS_PC,		// _e2p_input_icons1		5
	ICONS_TV,		// _e2p_input_icons2		5
	ICONS_Cd,		// _e2p_input_icons4		5
	ICONS_Vinyl,	// _e2p_input_icons3		5
	ICONS_Aux1,		// _e2p_input_icons5		5
	ICONS_Aux2,		// _e2p_input_icons6		5
//50
	0, 0, 4, 2, 1, 3, // _e2p_input_sources		6x3
	0, 1, 0, 0, 0, 0, // _e2p_input_triggers	6x1
//62
	0xFF, 0xFF,   // _e2p_frontColor			16
	0x00, 0x00,   // _e2p_backColor				16
	0x00, 0x00,   // _e2p_backColor2			16
	0x1C, 0x9F,   // _e2p_selFrontColor			16
	0xCA, 0xEB,   // _e2p_touchFrontColor		16
	0xDD, 0x24,   // _e2p_inactiveColor			16
//74
	5,   // PowerToggleDelay   (Seconds)		3
	2,   // VolumeSweepSpeed					5
	2,   // TftSweepSpeed						5
	0,   // CommonVolume						1
//78
	0, 0, 1, 0, 0, 0, // psu_sources			6
	0, 0, 0, 0, 0, 0, // ext trig 1				6
	0, 0, 0, 0, 0, 0, // ext trig 2				6
	1, 1, 1, 1, 1, 1, // use inputs				6
//102
	0,    // ext out							1
//103
	0, // animation id V standby off
	0, // animation id R standby off
	0, // animation id V standby on
	0, // animation id R standby on
	0, // animation id V mute off
	0, // animation id R mute off
	0, // animation id V mute on
	0, // animation id R mute on
	0, // animation id V input change
	0, // animation id R input change
	0, // animation id V trigger change
	0, // animation id R trigger change
	0, // animation id V is_in_standby
	0, // animation id R is_in_standby
	0, // animation id V is_in_power_on
	0, // animation id R is_in_power_on
	0, // animation id V is_in_mute
	0, // animation id R is_in_mute
	0, // animation id V screen change
	0, // animation id R screen change
	0, // animation id V action executed
	0, // animation id R action executed
	0, // animation id V setting updated
	0, // animation id R setting updated
	0, // animation id V Volume Changed
	0, // animation id R Volume Changed
//129
	0, // action id rotary
	0, // action id push
//131
	2, // power on mute delay
//132
	100,  // MinVolume1							8
	100,  // MinVolume2							8
	100,  // MinVolume3 						8
	100,  // MinVolume4							8
	100,  // MinVolume5							8
	100,  // MinVolume6							8
//138
	210,  // MaxVolume1							8
	210,  // MaxVolume2							8
	210,  // MaxVolume3							8
	210,  // MaxVolume4							8
	210,  // MaxVolume5							8
	210,  // MaxVolume6							8
//144
	/*
#define ugs_state_default				0
#define ugs_state_is_in_mute			1
#define ugs_state_is_in_standby			2
#define ugs_state_is_in_power_on		3
#define ugs_state_standby_off			4
#define ugs_state_standby_on			5
#define ugs_state_mute_off				6
#define ugs_state_mute_on				7
#define ugs_state_input_change			8
#define ugs_state_trigger_change		9
#define ugs_state_screen_change			10
#define ugs_state_action_executed		11
#define ugs_state_settings_updated		12
#define ugs_state_volume_changed		13
*/
	// animation delays by state
	20, 40, 40, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, // volume animation delays (x25ms)
	20, 40, 40, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, // rear   animation delays (x25ms)

	// nb of animation repetitions
	 0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1 , 1, // volume animations
	 0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1 , 1, // rear animations

	 // VU offsets (400 + x)
	 175, 175, 175, 175, 175, 175,

	 // VU multipliers (x / 4)
	   8,   8,   8,   8,   8,   8


//last => #195 (count 196)

};

#endif /* SETTINGS_H_ */
