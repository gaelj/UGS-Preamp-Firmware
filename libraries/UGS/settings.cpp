/*
 * PresetGroup.cpp
 *
 *  Created on: 17 août 2015
 *      Author: gael
 */

#include "settings.h"

uint8_t Settings::current_input = 0xFF;
unsigned long Settings::sleep_timeout;

bool Settings::sleep_active;
byte Settings::VolIncrement;

uint8_t Settings::Illumination_TFT_H;
uint8_t Settings::Illumination_Vol1_H;
uint8_t Settings::Illumination_Vol2_H;
uint8_t Settings::Illumination_Rly1_H;
uint8_t Settings::Illumination_Rly2_H;

uint8_t Settings::Illumination_TFT_L;
uint8_t Settings::Illumination_Vol1_L;
uint8_t Settings::Illumination_Vol2_L;
uint8_t Settings::Illumination_Rly1_L;
uint8_t Settings::Illumination_Rly2_L;

uint16_t Settings::touch_timeout; // = 100;
uint16_t Settings::hold_timeout; // = 1000;
uint16_t Settings::tft_sleep_timeout; // = 30;

uint8_t Settings::VolumeIn[6];

int16_t Settings::BalanceLR;

int16_t Settings::Offset[3] = { 0 };

uint8_t Settings::DefaultInput;
bool Settings::UseDefOrLastInput;

uint8_t Settings::DefaultVolume[6] = { 0 };

bool Settings::UseDefaultOrLastVol;

uint8_t Settings::input_icons[6] = { 0 };

uint8_t Settings::input_source[6] = { 0 };
bool Settings::input_trigger[6] = { 0 };

bool Settings::psu2_trigger[6] = { 0 };
bool Settings::ext1_trigger[6] = { 0 };
bool Settings::ext2_trigger[6] = { 0 };

long Settings::loopTime = 0;

uint16_t Settings::frontColor;
uint16_t Settings::backColor;
uint16_t Settings::backColor2;
uint16_t Settings::selFrontColor;
uint16_t Settings::touchFrontColor;
uint16_t Settings::inactiveColor;

bool Settings::OutAssym;			// output is assymetrical
bool Settings::Mute;    			// selected mute
bool Settings::Bypass;    			// selected bypass
bool Settings::ExtOut;

uint8_t Settings::PowerToggleDelay;
uint8_t Settings::PowerOnMuteDelay;

uint8_t Settings::VolumeSweepSpeed;
uint8_t Settings::TftSweepSpeed;

bool Settings::CommonVolume;

uint8_t Settings::anim_id_v[NB_ANIMATION_STATES] = { 0 };
uint8_t Settings::anim_id_r[NB_ANIMATION_STATES] = { 0 };

uint8_t Settings::action_id_Rotary;
uint8_t Settings::action_id_Push;

uint8_t Settings::MinVolumes[6] = { 0 };
uint8_t Settings::MaxVolumes[6] = { 0 };

uint8_t Settings::AnimationStateDelays_v[NB_ANIMATION_STATES] = { 0 };
uint8_t Settings::AnimationStateDelays_r[NB_ANIMATION_STATES] = { 0 };

uint8_t Settings::AnimationStateRepeats_v[NB_ANIMATION_STATES] = { 0 };
uint8_t Settings::AnimationStateRepeats_r[NB_ANIMATION_STATES] = { 0 };

uint8_t Settings::VuOffsets[6] = { 0 };
uint8_t Settings::VuMultipliers[6] = { 0 };

byte Settings::Values[_e2p_NbAddresses] = { 0 };


// states (do not require saving to eeprom)
bool Settings::Power[2]; 			// power supply status by relay
bool Settings::Trigger[2]; 			// trigger by relay
unsigned long Settings::LastPowerToggle[4];	// last power toggle millis - power / trigger

int Settings::DacSamplingRate;
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
bool Settings::DacHostActive;
bool Settings::DacAudioStreaming;

uint16_t Settings::Illumination_TFT;
uint16_t Settings::Illumination_Vol1;
uint16_t Settings::Illumination_Vol2;
uint16_t Settings::Illumination_Rear1;
uint16_t Settings::Illumination_Rear2;

long Settings::lastDumpToSerialRequestTimestamp;
bool Settings::DoDumpToSerial;

bool Settings::UseInput[6] = { 1 };

Settings Settings::CurrentPresetGroup = Settings();
byte Settings::deser[_e2p_NbAddresses];






bool Settings::IsEepromValid()
{
	return (EEPROM.read(_e2p_VersionAddr) == _e2p_VersionNr);

}

void Settings::WriteDefaultValuesToAllPresets()
{
	for (uint8_t i = 0; i < _e2p_NbPresets; i++) {
		Settings(i).WriteDefaultValuesToEeprom();
	}

	SetCurrentActivePresetNumber(0);
	EEPROM.write(_e2p_VersionAddr, _e2p_VersionNr);
}

void Settings::DumpAllToSerial()
{
	for (uint8_t i = 0; i < _e2p_NbPresets; i++) {
		Settings p = Settings(i);
		p.DumpSettingsToSerial();
	}
}

void Settings::LoadCurrentActivePreset()	// load from e2p
{
	uint8_t groupNumber = EEPROM.read(_e2p_CurrentPresetAddr);
	CurrentPresetGroup = Settings(groupNumber);
}

void Settings::SetCurrentActivePresetNumber(uint8_t preset) // save to e2p
{
	EEPROM.write(_e2p_CurrentPresetAddr, preset);
}


bool Settings::ExtractValuesFromSerialString(uint8_t msgNumber, String s)
{
    for(unsigned int i = 0; i < s.length() / 2; i++) {
        String byteVal = s.substring(i*2, (i*2) + 2);

    	char buf[3];
    	byteVal.toCharArray(buf, 3);

        Settings::deser[i + 20*msgNumber] = (byte)strtoul(buf, 0, 16);
    }

    return (true);
}

void Settings::RequestDumpToSerialDelayed()
{
	DoDumpToSerial = true;
	lastDumpToSerialRequestTimestamp = millis();
}

void Settings::DelayedActions()
{
	if ((long)millis() > (lastDumpToSerialRequestTimestamp + serial_dump_delay)) {
		if (DoDumpToSerial) {
			DoDumpToSerial = false;
			DumpSettingsToSerial();
		}
	}
}


Settings::Settings() {
	groupNumber = 0xFF;
}

Settings::Settings(uint8_t _groupNumber) {
	groupNumber = _groupNumber;
	ReadValuesFromEeprom();
}

void Settings::WriteDefaultValuesToEeprom() {
	EepromUtil::eeprom_write_bytes(groupNumber * _e2p_NbAddresses,
			defaultValues, _e2p_NbAddresses);
}

void Settings::DumpDataToSerial(String key, byte data[]) {

	String val = "";
	for (int i = 0; i < _e2p_NbAddresses; i++) {
		char hexv[3];
		sprintf(hexv, "%02x", data[i]);
		val += String(hexv);
	}
	Ser::SerialPrint(key, val);
}
void Settings::DumpSettingsToSerial() {

	String key = F("EEPROM ");
	key += String(groupNumber);

	Settings::DumpDataToSerial(key, Values);
}

void Settings::SendStatusToSerial()
{
	Settings::CurrentPresetGroup.DumpSettingsToSerial();

    Serial.println(F("UGS STATUS:"));

	Ser::SerialPrint(_CMD_STANDBY, !(Settings::Power[0]));
	Ser::SerialPrint(_CMD_RIAA_PWR, (Settings::Power[1]));
	Ser::SerialPrint(_CMD_5V_PWR, (Settings::Power[0]));

	Ser::SerialPrint(_CMD_INPUT, Settings::current_input);
	Ser::SerialPrint(_CMD_DACSRC,Settings::input_trigger[ Settings::current_input]);

	Ser::SerialPrint(_CMD_VOLUME, Settings::Volume());
	Ser::SerialPrint(_CMD_MUTE, Settings::Mute);
	Ser::SerialPrint(_CMD_BALANCE, Settings::BalanceLR);

	for (int i = 0; i < _NbVolBrds / 2; i++)
		Ser::SerialPrint(_CMD_OFFSET + String(i), Settings::Offset[i]);

	Ser::SerialPrint(_CMD_OUT_ASYM, Settings::OutAssym);
	Ser::SerialPrint(_CMD_EXT_OUT, Settings::ExtOut);
	Ser::SerialPrint(_CMD_BYPASS, Settings::Bypass);

	Ser::SerialPrint(_CMD_ILLUM_TFT, Settings::Illumination_TFT);
	Ser::SerialPrint(_CMD_ILLUM_VOL1, Settings::Illumination_Vol1);
	Ser::SerialPrint(_CMD_ILLUM_VOL2, Settings::Illumination_Vol2);
	Ser::SerialPrint(_CMD_ILLUM_RLY1, Settings::Illumination_Rear1);
	Ser::SerialPrint(_CMD_ILLUM_RLY2, Settings::Illumination_Rear2);

	Ser::SerialPrint(_CMD_GET_NB_RLYBRD, (uint8_t)_NbRlyBrds);
	Ser::SerialPrint(_CMD_GET_NB_VOLBRD, (uint8_t)_NbVolBrds);

	Ser::SerialPrint(_CMD_DACSTATUS, Settings::GetDacStatus());
}

void Settings::WriteValueToEeprom(int index, uint8_t value) {

	if (index > _e2p_MaxPresetBytes) Serial.println(F("E2P o/l"));

	if (Values[index] != value) {

		EEPROM.write(groupNumber * _e2p_NbAddresses + index, value);
		Values[index] = value;
	}
}

void Settings::ReadValuesFromEeprom() {
	EepromUtil::eeprom_read_bytes(groupNumber * _e2p_NbAddresses, Values, _e2p_NbAddresses);
}

void Settings::LoadValuesToSettings(byte deser[]) {

	byte i = 0;
	byte v1;
	byte v2;

	uint8_t current_input = deser[i++]; // Input is set in the settings at the end - store it in a local for now
	Preamp::SetOutAsym((bool) deser[i++]);
	Preamp::SetMute((bool) deser[i++]);
	Preamp::SetBypass((bool) deser[i++]);
	Settings::VolIncrement = deser[i++];

	Settings::Illumination_TFT_H = deser[i++];
	Settings::Illumination_Vol1_H = deser[i++];
	Settings::Illumination_Vol2_H = deser[i++];
	Settings::Illumination_Rly1_H = deser[i++];
	Settings::Illumination_Rly2_H = deser[i++];

	Settings::Illumination_TFT_L = deser[i++];
	Settings::Illumination_Vol1_L = deser[i++];
	Settings::Illumination_Vol2_L = deser[i++];
	Settings::Illumination_Rly1_L = deser[i++];
	Settings::Illumination_Rly2_L = deser[i++];

	v1 = deser[i++];
	v2 = deser[i++];
	Settings::touch_timeout = (uint16_t) (v1 << 8 | v2);

	v1 = deser[i++];
	v2 = deser[i++];
	Settings::hold_timeout = (uint16_t) (v1 << 8 | v2);

	v1 = deser[i++];
	v2 = deser[i++];
	Settings::tft_sleep_timeout = (uint16_t) (v1 << 8 | v2);

	for (int j = 0; j < 6; j++)
		Settings::VolumeIn[j] = deser[i++];

	v1 = deser[i++];
	v2 = deser[i++];
	Settings::BalanceLR = (int16_t) (v1 << 8 | v2);

	for (unsigned int j = 0; j < 3; j++) {
		v1 = deser[i++];
		v2 = deser[i++];
		Settings::Offset[j] = (int16_t) (v1 << 8 | v2);
	}

	Settings::DefaultInput = deser[i++];
	Settings::UseDefOrLastInput = (bool) deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::DefaultVolume[j] = deser[i++];

	Settings::UseDefaultOrLastVol = (bool) deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::input_icons[j] = deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::input_source[j] = (uint8_t) deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::input_trigger[j] = (bool) deser[i++];

	v1 = deser[i++];
	v2 = deser[i++];
	Settings::frontColor = (uint16_t) (v1 << 8 | v2);
	v1 = deser[i++];
	v2 = deser[i++];
	Settings::backColor = (uint16_t) (v1 << 8 | v2);
	v1 = deser[i++];
	v2 = deser[i++];
	Settings::backColor2 = (uint16_t) (v1 << 8 | v2);
	v1 = deser[i++];
	v2 = deser[i++];
	Settings::selFrontColor = (uint16_t) (v1 << 8 | v2);
	v1 = deser[i++];
	v2 = deser[i++];
	Settings::touchFrontColor = (uint16_t) (v1 << 8 | v2);
	v1 = deser[i++];
	v2 = deser[i++];
	Settings::inactiveColor = (uint16_t) (v1 << 8 | v2);

	Settings::PowerToggleDelay = deser[i++];
	Settings::VolumeSweepSpeed = deser[i++];
	Settings::TftSweepSpeed = deser[i++];
	Settings::CommonVolume = deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::psu2_trigger[j] = (bool) deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::ext1_trigger[j] = (bool) deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::ext2_trigger[j] = (bool) deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::UseInput[j] = (bool) deser[i++];

	// default to use at least input 0 (if no input is used)
	if (Settings::UseInput[0]+Settings::UseInput[1]+Settings::UseInput[2]+Settings::UseInput[3]+Settings::UseInput[4]+Settings::UseInput[5] == 0)
		Settings::UseInput[0] = true;

	Preamp::SetExtOut(deser[i++]);

	for (int j = 1; j < NB_ANIMATION_STATES; j++) {
		Settings::anim_id_v[j]  = deser[i++];
		Settings::anim_id_r[j]  = deser[i++];
	}

	Settings::action_id_Rotary = deser[i++];
	Settings::action_id_Push = deser[i++];

	Settings::PowerOnMuteDelay = deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::MinVolumes[j] = deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::MaxVolumes[j] = deser[i++];

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		Settings::AnimationStateDelays_v[j] = deser[i++];

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		Settings::AnimationStateDelays_r[j] = deser[i++];

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		Settings::AnimationStateRepeats_v[j] = deser[i++];

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		Settings::AnimationStateRepeats_r[j] = deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::VuOffsets[j] = deser[i++];

	for (int j = 0; j < 6; j++)
		Settings::VuMultipliers[j] = deser[i++];

	// Serial.print("Read settings until ");
	// Serial.println(i);


	// *************************************************************
	// Apply input setting
	if (Settings::UseDefOrLastInput) {
		// Serial.print(F("Setting Input to last value: "));
		// Serial.println(current_input);
		Preamp::SelectInput(current_input);
	} else {
		// Serial.print(F("Setting Input to default value: "));
		// Serial.println(Settings::DefaultInput);
		Preamp::SelectInput(Settings::DefaultInput);
	}

	// Apply volume setting
	if (!Settings::UseDefaultOrLastVol) {
		// Serial.print(F("Setting volume to default value: "));
		// Serial.println(Settings::DefaultVolume[Settings::current_input]);
		Settings::VolumeIn[current_input] = Settings::DefaultVolume[current_input];
	}

	Preamp::SetExtOut((bool) Settings::ExtOut);

	// Preamp::SetVolume(Settings::Volume()); // already in SelectInput

}




void Settings::WriteSettingsToEeprom() {
	byte i = 0;
	byte v1;
	byte v2;

//0
	WriteValueToEeprom(i++, Settings::current_input);
	WriteValueToEeprom(i++, Settings::OutAssym);
	WriteValueToEeprom(i++, Settings::Mute);
	WriteValueToEeprom(i++, Settings::Bypass);
	WriteValueToEeprom(i++, Settings::VolIncrement);
//5
	WriteValueToEeprom(i++, Settings::Illumination_TFT_H);
	WriteValueToEeprom(i++, Settings::Illumination_Vol1_H);
	WriteValueToEeprom(i++, Settings::Illumination_Vol2_H);
	WriteValueToEeprom(i++, Settings::Illumination_Rly1_H);
	WriteValueToEeprom(i++, Settings::Illumination_Rly2_H);
//9
	WriteValueToEeprom(i++, Settings::Illumination_TFT_L);
	WriteValueToEeprom(i++, Settings::Illumination_Vol1_L);
	WriteValueToEeprom(i++, Settings::Illumination_Vol2_L);
	WriteValueToEeprom(i++, Settings::Illumination_Rly1_L);
	WriteValueToEeprom(i++, Settings::Illumination_Rly2_L);
//15
	v1 = (byte) (Settings::touch_timeout >> 8);
	v2 = (byte) (Settings::touch_timeout & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);

	v1 = (byte) (Settings::hold_timeout >> 8);
	v2 = (byte) (Settings::hold_timeout & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);

	v1 = (byte) (Settings::tft_sleep_timeout >> 8);
	v2 = (byte) (Settings::tft_sleep_timeout & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//21
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::VolumeIn[j]);
//27
	v1 = (byte) ((Settings::BalanceLR >> 8) & 0XFF);
	v2 = (byte) (Settings::BalanceLR & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//29
	for (unsigned int j = 0; j < 3; j++) {
		int16_t o = Settings::Offset[j];

		v1 = (byte) ((o >> 8) & 0XFF);
		v2 = (byte) (o & 0XFF);

		WriteValueToEeprom(i++, v1);
		WriteValueToEeprom(i++, v2);
	}
//35
	WriteValueToEeprom(i++, Settings::DefaultInput);
	WriteValueToEeprom(i++, Settings::UseDefOrLastInput);
//37
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::DefaultVolume[j]);
//43
	WriteValueToEeprom(i++, Settings::UseDefaultOrLastVol);
//45
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::input_icons[j]);
//50
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::input_source[j]);
//56
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::input_trigger[j]);
//62
	v1 = (byte) (Settings::frontColor >> 8);
	v2 = (byte) (Settings::frontColor & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//64
	v1 = (byte) (Settings::backColor >> 8);
	v2 = (byte) (Settings::backColor & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//66
	v1 = (byte) (Settings::backColor2 >> 8);
	v2 = (byte) (Settings::backColor2 & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//68
	v1 = (byte) (Settings::selFrontColor >> 8);
	v2 = (byte) (Settings::selFrontColor & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//70
	v1 = (byte) (Settings::touchFrontColor >> 8);
	v2 = (byte) (Settings::touchFrontColor & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//72
	v1 = (byte) (Settings::inactiveColor >> 8);
	v2 = (byte) (Settings::inactiveColor & 0XFF);
	WriteValueToEeprom(i++, v1);
	WriteValueToEeprom(i++, v2);
//74
	WriteValueToEeprom(i++, Settings::PowerToggleDelay);
	WriteValueToEeprom(i++, Settings::VolumeSweepSpeed);
	WriteValueToEeprom(i++, Settings::TftSweepSpeed);
	WriteValueToEeprom(i++, Settings::CommonVolume);
//78
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::psu2_trigger[j]);
//84
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::ext1_trigger[j]);
//90
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::ext2_trigger[j]);
//96
	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::UseInput[j]);
//102
	WriteValueToEeprom(i++, Settings::ExtOut);
//103

	for (int j = 1; j < NB_ANIMATION_STATES; j++) {
		WriteValueToEeprom(i++, Settings::anim_id_v[j]);
		WriteValueToEeprom(i++, Settings::anim_id_r[j]);
	}
//133
	WriteValueToEeprom(i++, Settings::action_id_Rotary);
	WriteValueToEeprom(i++, Settings::action_id_Push);
//135
	WriteValueToEeprom(i++, Settings::PowerOnMuteDelay);

	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::MinVolumes[j]);

	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::MaxVolumes[j]);

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		WriteValueToEeprom(i++, Settings::AnimationStateDelays_v[j]);

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		WriteValueToEeprom(i++, Settings::AnimationStateDelays_r[j]);

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		WriteValueToEeprom(i++, Settings::AnimationStateRepeats_v[j]);

	for (int j = 1; j < NB_ANIMATION_STATES; j++)
		WriteValueToEeprom(i++, Settings::AnimationStateRepeats_r[j]);

	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::VuOffsets[j]);

	for (int j = 0; j < 6; j++) // MAX_INPUTS
		WriteValueToEeprom(i++, Settings::VuMultipliers[j]);

	if (i >= _e2p_IconStoreAddress)
		Ser::SerialPrint(_CMD_ERROR, F("E2P overlap"));

	// Serial.print("Write settings until ");
	// Serial.println(i);
}

uint8_t Settings::Volume()
{
	return (VolumeIn[current_input]);
}

String Settings::GetDacStatus()
{
	String str = F("DAC: ");

	if (Settings::CurrentPresetGroup.DacAudioStreaming) {
		switch (Settings::DacSamplingRate) {
			case 0: str = F("--"); break;
			case 1: str = String(44.1); break;
			case 2: str = String(48); break;
			case 3: str = String(88.2); break;
			case 4: str = String(96); break;
			case 5: str = String(176.4); break;
			case 6: str = String(192); break;
			case 7: str = String(352.8); break;
			case 8: str = String(384); break;
			default: str += String(Settings::DacSamplingRate); break;
		}
		str += F("kHz");
	}
	else if (Settings::CurrentPresetGroup.DacHostActive)
		str += F("PC USB active");
	else
		str += F("PC USB down");

	return (str);
}
