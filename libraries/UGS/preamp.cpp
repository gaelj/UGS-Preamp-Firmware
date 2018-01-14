/*
 * preamp.cpp
 *
 *  Created on: 28 janv. 2015
 *      Author: gael
 */

#include "preamp.h"

#include "iconspage.h"

uint8_t Preamp::PreviousVolume = 0;
bool Preamp::MaxVolumeReached = 0;

#if _NbRlyBrds == 2
MCP Preamp::Mcp[_NbRlyBrds] = { MCP(0), MCP(1) };
#endif
#if _NbVolBrds == 2
MUSES Preamp::Muses[_NbVolBrds] = { MUSES(__SPI_Vol_CS, 0), MUSES(__SPI_Vol_CS, 1) };
#endif
#if _NbRlyBrds == 4
MCP Preamp::Mcp[_NbRlyBrds] = { MCP(0), MCP(1), MCP(2), MCP(3) };
#endif
#if _NbVolBrds == 4
MUSES Preamp::Muses[_NbVolBrds] = { MUSES(__SPI_Vol_CS, 0), MUSES(__SPI_Vol_CS, 1), MUSES(__SPI_Vol_CS, 2), MUSES(__SPI_Vol_CS, 3) };
#endif
#if _NbRlyBrds == 6
MCP Preamp::Mcp[_NbRlyBrds] = { MCP(0), MCP(1), MCP(2), MCP(3), MCP(4), MCP(5) };
#endif
#if _NbVolBrds == 6
MUSES Preamp::Muses[_NbVolBrds] = { MUSES(__SPI_Vol_CS, 0), MUSES(__SPI_Vol_CS, 1), MUSES(__SPI_Vol_CS, 2), MUSES(__SPI_Vol_CS, 3), MUSES(__SPI_Vol_CS, 4), MUSES(__SPI_Vol_CS, 5) };
#endif


int16_t Preamp::MusesVolumes[_NbVolBrds] = { 0 };	// real volume setting per channel
byte Preamp::Source;         				// selected input (physical); from 0 to 4
bool Preamp::InAssym;

bool Preamp::curEncoderValue1 = 0; 		// buffered and unprocessed encoder wheel tick count
bool Preamp::curEncoderValue2 = 0; 		// buffered and unprocessed encoder wheel tick count
bool Preamp::curPush = 0;		  			// unprocessed encoder push
long Preamp::pushStartTime = 0;
bool Preamp::longPushProcessed = 0;

long Preamp::DacLastSeenActive;
bool Preamp::DacIsDown = 0;

uint16_t Preamp::VuMeter[2] = { 0 };

bool Preamp::delayedTriggerRequestState[4] = { 0 };
bool Preamp::delayedTriggerRequest[4] = { 0 };
bool Preamp::delayedUnMuteRequest = false;

uint16_t Preamp::LedsAndRelayCache[_NbRlyBrds] = { 0 };

#ifdef _IRactive
IRrecv Preamp::irrecv = IRrecv(__irIn);
#endif

void Preamp::begin()
{
	pinMode(__vu0, INPUT);
	pinMode(__vu1, INPUT);

	pinMode(__VolPush, INPUT);
	pinMode(__VolEnc1, INPUT);
	pinMode(__VolEnc2, INPUT);
	// pinMode(__irIn, INPUT);


	pinMode(__irOut, OUTPUT);

	pinMode(__SPI_Mux_CS, OUTPUT);
	pinMode(__SPI_Vol_CS, OUTPUT);

	pinMode(__Led_A_1, OUTPUT);
	pinMode(__Led_A_2, OUTPUT);
	pinMode(__Led_A_3, OUTPUT);
	pinMode(__Led_A_4, OUTPUT);
	pinMode(__Led_A_5, OUTPUT);
	pinMode(__Led_A_6, OUTPUT);
	pinMode(__Led_A_7, OUTPUT);
	pinMode(__Led_A_8, OUTPUT);
	pinMode(__Led_B_1, OUTPUT);
	pinMode(__Led_B_2, OUTPUT);
	pinMode(__Led_B_3, OUTPUT);
	pinMode(__Led_B_4, OUTPUT);
	pinMode(__Led_B_5, OUTPUT);
	pinMode(__Led_B_6, OUTPUT);
	pinMode(__Led_B_7, OUTPUT);
	pinMode(__Led_B_8, OUTPUT);

	pinMode(__PwrRly1, OUTPUT);
	pinMode(__PwrRly2, OUTPUT);

	pinMode(__ExtSwRly1, OUTPUT);
	pinMode(__ExtSwRly2, OUTPUT);

	pinMode(__5VRly, OUTPUT);
	pinMode(__PwmVolLed1, OUTPUT);
	pinMode(__PwmVolLed2, OUTPUT);
	pinMode(__PwmRearLed1, OUTPUT);
	pinMode(__PwmRearLed2, OUTPUT);
	pinMode(__PwmTft, OUTPUT);


#ifdef _IRactive
	irrecv.enableIRIn(); // Start the receiver
#endif

	Serial2.begin(__baudrate);

	LedAnimation::Init();

	// ignore soft start on initial power-off
	Settings::LastPowerToggle[0] = millis() - (5000 * Settings::PowerToggleDelay);
	Settings::LastPowerToggle[1] = millis() - (5000 * Settings::PowerToggleDelay);

	SetPower(0, Settings::Power[0]);
	SetPower(1, Settings::Power[1]);
	SetTrigger(0, Settings::Trigger[0]);
	SetTrigger(0, Settings::Trigger[1]);

	// Settings::current_input = 0xFF;

	curEncoderValue1 = digitalRead(__VolEnc1);
	curEncoderValue2 = digitalRead(__VolEnc2);

	curPush = ! digitalRead(__VolPush);

	analogWrite(__PwmTft, 255);
}

void Preamp::InitializeSpiSlaves()
{
	// SPI: Use word-write mode to Set all of the pins on outputchip to be outputs (0)

	for(int channel = 0; channel < _NbRlyBrds; channel++) {

		digitalWrite(__SPI_Mux_CS, LOW);
		Preamp::Mcp[_NbRlyBrds] = MCP(channel);
		digitalWrite(__SPI_Mux_CS, HIGH);

		digitalWrite(__SPI_Mux_CS, LOW);
		Preamp::Mcp[channel].pinMode(0x0000);
		digitalWrite(__SPI_Mux_CS, HIGH);
	}

	for(int c = 0; c < _NbVolBrds; c++) {
		Muses[c] = MUSES(__SPI_Vol_CS, c);
		Muses[c].SetParameters(1, 1, 0); // byte LRcont1, byte LRcont2, byte ZC
	}
}

void Preamp::DisplayText(String Text)
{
	IconsPage::DisplayText(Text);
}

void Preamp::SetPowerDelayed(uint8_t relay, bool state)
{
	delayedTriggerRequest[relay] = 1;
	delayedTriggerRequestState[relay] = state;

	if (state && Settings::Power[relay] == 0) {
		Preamp::SetMute(1);
		// delayedUnMuteRequest = true;
		if (relay == 0)
			LedAnimation::SetUgsState(ugs_state_standby_off);
	}
	else if (state == 0 && Settings::Power[relay]) {
		if (relay == 0)
			LedAnimation::SetUgsState(ugs_state_standby_on);
	}
}

void Preamp::SetTriggerDelayed(uint8_t trigger, bool state)
{
	delayedTriggerRequest[trigger+2] = 1;
	delayedTriggerRequestState[trigger+2] = state;

	if (state && Settings::Trigger[trigger] == 0) {
		Preamp::SetMute(1);
		//delayedUnMuteRequest = true;
	}
	if (state != Settings::Trigger[trigger])
		LedAnimation::SetUgsState(ugs_state_trigger_change);
}

void Preamp::SetPower(uint8_t relay, bool state)
{
	if (state && Settings::Power[relay] == 0) {
		Preamp::SetMute(1);
		delayedUnMuteRequest = true;
	}/*
	else if (state == 0 && Settings::Power[relay]) {
		Preamp::SetMute(1);
		delayedUnMuteRequest = true;
	}*/


	Settings::LastPowerToggle[relay] = millis();
	switch (relay) {
		case 0:
			Ser::SerialPrint(_CMD_STANDBY, !state);
			Ser::SerialPrint(_CMD_5V_PWR, state);
			digitalWrite(__PwrRly1, state);
			digitalWrite(__5VRly, state);
			if (state == 1 && Settings::Power[0] == 0) {
				delay(300);
				Preamp::InitializeSpiSlaves();
			}
		break;
		case 1:
			Ser::SerialPrint(_CMD_RIAA_PWR, state);
			digitalWrite(__PwrRly2, state);
		break;
	}
	Settings::Power[relay] = state;
}

void Preamp::SetTrigger(uint8_t trigger, bool state)
{
	if (state && Settings::Trigger[trigger] == 0) {
		Preamp::SetMute(1);
		delayedUnMuteRequest = true;
	}

	Settings::LastPowerToggle[trigger+2] = millis();
	switch (trigger) {
		case 0: digitalWrite(__ExtSwRly1, !state); Settings::Trigger[0] = !state; break;
		case 1: digitalWrite(__ExtSwRly2, !state); Settings::Trigger[1] = !state; break;
	}
}

void Preamp::DelayedActions()
{
	bool allMuteReqClear = true;
	for(byte rt = 0; rt < 4; rt++) {
		if (delayedTriggerRequest[rt])
			if (millis() > ((1000 * Settings::PowerToggleDelay) + Settings::LastPowerToggle[rt])) {
				if (rt < 2)
					Preamp::SetPower(rt, delayedTriggerRequestState[rt]);
				else
					Preamp::SetTrigger(rt-2, delayedTriggerRequestState[rt]);
				delayedTriggerRequest[rt] = false;
			}
		if (delayedUnMuteRequest) {
			if (millis() < ((1000 * Settings::PowerOnMuteDelay) + Settings::LastPowerToggle[rt])) {
				allMuteReqClear = false;
			}
		}
	}
	if (allMuteReqClear && delayedUnMuteRequest) {
		Preamp::SetMute(0);
		delayedUnMuteRequest = false;
	}
}

void Preamp::SetBypass(bool state)
{
	Settings::Bypass = state;
	SetRearLedsAndRelays();

	Ser::SerialPrint(_CMD_BYPASS, Settings::Bypass);
}

void Preamp::SelectInput(uint8_t input) // input goes from 0 to 5
{
	// if (Settings::current_input == input) return;
	if (input > 5) return;
	if (Settings::UseInput[input] == 0) return;

	uint8_t prevVol = Settings::Volume();

	// turn on or off riaa power & triggers of new input
	Preamp::SetPowerDelayed(1, Settings::psu2_trigger[input]);
	Preamp::SetTriggerDelayed(0, Settings::ext1_trigger[input]);
	Preamp::SetTriggerDelayed(1, Settings::ext2_trigger[input]);
	Preamp::SetDacSource((bool)Settings::input_trigger[input]);

	if (Settings::UseDefaultOrLastVol == 0)
		Settings::VolumeIn[input] = Settings::DefaultVolume[input];

	Preamp::SetSource(Settings::input_source[input]);
	Preamp::PreviousVolume = 0;
	Settings::current_input = input;
	Preamp::SetVolume(Settings::Volume());

	if (USE_DISPLAY && IconsPage::current_page == PAGE_MAIN) {

		// refresh active source icon
		int16_t curIcon;
		curIcon = Settings::input_icons[input];
		IconsPage::icons[curIcon] = Icon(IconsPage::layout_default, curIcon, 4, 1, IconsPage::tft, false, false, false);

		// refresh volume slider
		IconsPage::DrawVolumeBar(0, Settings::Volume(), prevVol, 0, _MUSES_VolumeSteps);
		if (Preamp::MaxVolumeReached)
			IconsPage::icons[ICONS_VolUp].Draw(IconsPage::tft, true, false, false);
		else
			IconsPage::icons[ICONS_VolUp].Draw(IconsPage::tft, false, false, false);
	}

	LedAnimation::SetUgsState(ugs_state_input_change);


	Ser::SerialPrint(_CMD_INPUT, input);

	// empty serial buffer
	String cmd;
	String val;
	Preamp::ProcessSerial(cmd, val);
}

void Preamp::SetSource(uint8_t source)
{
	if (source > 4) return;
	Source = source;
	SetRearLedsAndRelays();
}

void Preamp::SetDacSource(bool source)
{
	// send request to DAC on serial2
	Serial2.print(F("S="));
	Serial2.println(source);

	Ser::SerialPrint(_CMD_DACSRC, source);
}

void Preamp::SetInAsym(bool asym)
{
	InAssym = asym;
	SetRearLedsAndRelays();
}

void Preamp::SetOutAsym(bool asym)
{
	Settings::OutAssym = asym;
	SetRearLedsAndRelays();

	Ser::SerialPrint(_CMD_OUT_ASYM, Settings::OutAssym);
}

void Preamp::SetExtOut(bool ext)
{
	Settings::ExtOut = ext;
	SetRearLedsAndRelays();

	Ser::SerialPrint(_CMD_EXT_OUT, Settings::ExtOut);
}

void Preamp::SetMuses(uint8_t volume)
{
	// Apply balance and offsets
	for (byte c = 0; c < _NbVolBrds / 2; c++)
	{
		byte i = c * 2;

		MusesVolumes[i]     = volume;
		MusesVolumes[i + 1] = volume;

		if (volume > 0)
		{
			if (Settings::BalanceLR < 0) {
				MusesVolumes[i + 1] -= (-Settings::BalanceLR);
			} else {
				MusesVolumes[i    ] -= Settings::BalanceLR;
			}

			MusesVolumes[i    ] += Settings::Offset[c];
			MusesVolumes[i + 1] += Settings::Offset[c];
		}
	}

	// Enforce min and max volumes and apply to Muses
	MaxVolumeReached = 0;
	for(byte i = 0; i < _NbVolBrds; i++)
	{
		if (MusesVolumes[i] < 0) MusesVolumes[i] = 0;
		if (MusesVolumes[i] >= Settings::MaxVolumes[Settings::current_input]) {
			MusesVolumes[i] = Settings::MaxVolumes[Settings::current_input];
			MaxVolumeReached = 1;
		}

		Muses[i].SetAttenuation((char)MusesVolumes[i]);
	}
}

void Preamp::SetVolume(uint8_t volume)
{
#ifdef debug_print_serial_set
	Ser::SerialPrint(F("REM:VOLUME SET"), volume);
#endif

	if (volume >= 0xF0) volume = 0;
	else if (volume > _MUSES_VolumeSteps) volume = _MUSES_VolumeSteps;
	else if (volume > Settings::MaxVolumes[Settings::current_input]) volume = Settings::MaxVolumes[Settings::current_input];

	if (volume <= Settings::MinVolumes[Settings::current_input]) {
		if (volume < 8)
			volume = Settings::MinVolumes[Settings::current_input]; // moving volume up from 0
		else
			volume = 0;
	}

	if (Settings::CommonVolume) {

		int16_t diff = (int16_t)volume - (int16_t)Settings::DefaultVolume[Settings::current_input];

		for(int i=0; i < MAX_INPUTS; i++) {
			int16_t newVol;
			newVol = (int16_t)Settings::DefaultVolume[i] + diff;

			if (newVol < 0)
				Settings::VolumeIn[i] = 0;
			else if (newVol > _MUSES_VolumeSteps)
				Settings::VolumeIn[i] = _MUSES_VolumeSteps;
			else
				Settings::VolumeIn[i] = (uint8_t)newVol;
		}
	}
	else
		Settings::VolumeIn[Settings::current_input] = volume;

	if (!Settings::Mute) {
		SweepMuses(Preamp::PreviousVolume, volume);
	}

	if (Preamp::PreviousVolume != volume)
		LedAnimation::SetUgsState(ugs_state_volume_changed);

	Preamp::PreviousVolume = volume;

	Ser::SerialPrint(F("VOLUME"), Settings::Volume());
}

void Preamp::SweepMuses(uint8_t volume1, uint8_t volume2)
{
	if (volume1 > volume2) {
		for (int v = volume1; v >= volume2; v--) {
			SetMuses(v);
			delay(Settings::VolumeSweepSpeed);
		}
	}
	else {
		for (int v = volume1; v <= volume2; v++) {
			SetMuses(v);
			delay(Settings::VolumeSweepSpeed);
		}
	}
}

void Preamp::SetMute(bool mute)
{
#ifdef debug_print_serial_set
	Ser::SerialPrint(F("REM:MUTE SET"), mute);
#endif

	if (Settings::Mute != mute) {
		Settings::Mute = mute;
		if (mute) {
			SweepMuses(Settings::Volume(), 0);
			LedAnimation::SetUgsState(ugs_state_mute_on);
		}
		else {
			SweepMuses(0, Settings::Volume());
			LedAnimation::SetUgsState(ugs_state_mute_off);
		}

		// refresh mute icon
		if (USE_DISPLAY && IconsPage::current_page == PAGE_MAIN) {
			IconsPage::icons[ICONS_Mute].Draw(IconsPage::tft, Settings::Mute, false, false);
		}

		Ser::SerialPrint(_CMD_MUTE, mute);
	}
}

void Preamp::SetBalance(int16_t balance)
{
	Settings::BalanceLR = balance;
	SetVolume(Settings::Volume());
	Ser::SerialPrint(_CMD_BALANCE, balance);
}

void Preamp::SetOffset(uint8_t channel_pair, int16_t offset)
{
	Settings::Offset[channel_pair] = offset;
	SetVolume(Settings::Volume());
	Ser::SerialPrint(_CMD_OFFSET + String(channel_pair), offset);
}

/*
int16_t Preamp::ConvertIlluminationlevel(int8_t level)
{
	// 0 to 10
	int16_t p;
	switch (level) {
	case 0:		p = 0;		break;
	case 1:		p = 8;		break;
	case 2:		p = 16;		break;
	case 3:		p = 24;		break;
	case 4:		p = 32;	break;
	case 5:		p = 48;	break;
	case 6:		p = 64;	break;
	case 7:		p = 96;	break;
	case 8:		p = 128;	break;
	case 9:		p = 192;	break;
	case 10:	default:	p = 255;	break;
	}
	return (p);
}
*/

void Preamp::SetIllumination_TFT(uint8_t level, bool log)
{
	/*
#ifdef debug_print_serial_set
	SerialPrint("REM:ILLUM_TFT SET", level);
#endif
	 */
	if (Settings::Illumination_TFT != level) {

		Settings::Illumination_TFT = level;
		// analogWrite(__PwmTft, 255);
		IconsPage::tft.PWM1out(level);

		if (log)
			Ser::SerialPrint("ILLUM_TFT", Settings::Illumination_TFT);
	}
}

void Preamp::SetIllumination_Vol(uint8_t channel, uint8_t level)
{
	if (channel == 0) {
		analogWrite(__PwmVolLed1, 0xFF - level);
		Settings::Illumination_Vol1 = level;
	} else if (channel == 1) {
		analogWrite(__PwmVolLed2, 0xFF - level);
		Settings::Illumination_Vol2 = level;
	}
}

void Preamp::SetIllumination_Rear(uint8_t channel, uint8_t level)
{
	if (channel == 0) {
		Settings::Illumination_Rear1 = level;
		analogWrite(__PwmRearLed1, 0xFF - level);
	} else if (channel == 1) {
		Settings::Illumination_Rear2 = level;
		analogWrite(__PwmRearLed2, 0xFF - level);
	}
}


void Preamp::SetRearLedsAndRelays()
{
	uint16_t w[_NbRlyBrds] = { 0 };
	bool changeFound = false;

	for(int channel = 0; channel < _NbRlyBrds; channel++) {

		w[channel] = (uint16_t)    (
				(1 & InAssym) |
			   ((1 & Settings::Bypass) << 1) |
			   ((1 & (Source == 0)) << 2) |
			   ((1 & (Source == 1)) << 3) |
			   ((1 & (Source == 2)) << 4) |
			   ((1 & (Source == 3)) << 5) |
			   ((1 & (Source == 4)) << 6) |
			   ((1 & Settings::OutAssym) << 7) |
			   ((1 & Settings::ExtOut) << 8) |
			((0x7F & LedAnimation::bufferLedsRear[channel]) << 9));

		if (Preamp::LedsAndRelayCache[channel] != w[channel]) {
			/*
			uint16_t wc;

			Serial.print(F("Mux "));
			Serial.print(channel);
			Serial.print(F(": Leds: "));

			Serial.print(String(LedAnimation::bufferLedsRear[channel], BIN));

			Serial.print(F(" - Complete: "));

			Serial.println(String(w[channel], BIN));
			*/
			changeFound = true;
		}
	}


	if (changeFound) {
		for(int channel = 0; channel < _NbRlyBrds; channel++) {
			if (Preamp::LedsAndRelayCache[channel] != w[channel]) {
				digitalWrite(__SPI_Mux_CS, LOW);
				Preamp::Mcp[channel].digitalWrite(w[channel]);
				digitalWrite(__SPI_Mux_CS, HIGH);
				Preamp::LedsAndRelayCache[channel] = w[channel];
			}
		}
	}
}

void Preamp::SetVolumeLeds(uint16_t leds)
{
	digitalWrite(__Led_A_1, (leds) & 1);
	digitalWrite(__Led_A_2, (leds >> 1) & 1);
	digitalWrite(__Led_A_3, (leds >> 2) & 1);
	digitalWrite(__Led_A_4, (leds >> 3) & 1);
	digitalWrite(__Led_A_5, (leds >> 4) & 1);
	digitalWrite(__Led_A_6, (leds >> 5) & 1);
	digitalWrite(__Led_A_7, (leds >> 6) & 1);
	digitalWrite(__Led_A_8, (leds >> 7) & 1);
	digitalWrite(__Led_B_1, (leds >> 8) & 1);
	digitalWrite(__Led_B_2, (leds >> 9) & 1);
	digitalWrite(__Led_B_3, (leds >> 10) & 1);
	digitalWrite(__Led_B_4, (leds >> 11) & 1);
	digitalWrite(__Led_B_5, (leds >> 12) & 1);
	digitalWrite(__Led_B_6, (leds >> 13) & 1);
	digitalWrite(__Led_B_7, (leds >> 14) & 1);
	digitalWrite(__Led_B_8, (leds >> 15) & 1);
}



void(* resetFunc) (void) = 0; // declare reset function @ address 0
void Preamp::ResetArduino()
{
	resetFunc();
}

void Preamp::ReadVuMeters()
{
	Preamp::VuMeter[0] = 0;
	Preamp::VuMeter[1] = 0;

	for(byte j = 0; j < VU_ITERATIONS; j++) {
		for(byte i = 0; i < 2; i++) {
			uint16_t r = analogRead(__vu0 + i); // 10 bits analog readout (0-1023)
			if (Preamp::VuMeter[i] < r)
				Preamp::VuMeter[i] = r;
		}
	}

	uint16_t o = (400 + Settings::VuOffsets[Settings::current_input]);
	float m = (((float) Settings::VuMultipliers[Settings::current_input]) / 4);

	for (byte i = 0; i < 2; i++) {
		if (Preamp::VuMeter[i] > o) {
			Preamp::VuMeter[i] -= o;

			Preamp::VuMeter[i] = ((float) Preamp::VuMeter[i]) * m;

			if (Preamp::VuMeter[i] > 1023)
				Preamp::VuMeter[i] = 1023;
		}
		else
			Preamp::VuMeter[i] = 0;
	}
}

int8_t Preamp::ProcessEncoder()
{
	int8_t ret = 0;

	bool newEncoderValue1 = digitalRead(__VolEnc1);
	bool newEncoderValue2 = digitalRead(__VolEnc2);

	if (curEncoderValue1 == 1 && curEncoderValue2 == 1) {
		if (newEncoderValue1 != newEncoderValue2) {
			if (newEncoderValue1 == 1)
				ret = 1;
			else
				ret = -1;
		}
	}

	Preamp::curEncoderValue1 = newEncoderValue1;
	Preamp::curEncoderValue2 = newEncoderValue2;

	return (ret);
}

// Returns -1 if no push, 0 if push active, press time if push released
long Preamp::ProcessPush()
{
	bool newPush = ! digitalRead(__VolPush);

	if (newPush == 1 && curPush == 0) {
		// start new push
		pushStartTime = millis();
		curPush = newPush;
		return (0);
	}
	else if (newPush == 0 && curPush == 1) {
		// finger lifted
		curPush = newPush;

		if (longPushProcessed == 0) {
			// push action will now be done
			return (millis() - pushStartTime);
		}
		else {
			// push action was done (long push) ; do nothing but reset the trigger
			longPushProcessed = 0;
			return (-1);
		}
	}
	else {
		// no press change
		if (((millis() - pushStartTime) > 1000) && curPush == 1 && longPushProcessed == 0) {
			// a long push has been triggered ; do push action
			longPushProcessed = 1;
			return (millis() - pushStartTime);
		}
		else {
			return (-1);
		}
	}
}

bool Preamp::ProcessSerial(String &cmd, String &val)
{
	// bool dataFound = false;
	String data = "";
	char c;
	// int d = 0;
	uint16_t cnt = 0;
	uint16_t MAX = 8192;

	if (Serial.available() > 0) {

		data = Serial.readStringUntil('\r');
		if (data.startsWith("\n")) data = data.substring(1);
		if (data.endsWith("\n")) data = data.substring(0, data.length() - 2);
		data.trim();
	}
	if (Serial1.available() > 0) {

		data = Serial1.readStringUntil('\r');
		if (data.startsWith("\n")) data = data.substring(1);
		if (data.endsWith("\n")) data = data.substring(0, data.length() - 2);
		data.trim();
	}
	if (Serial2.available() > 0) {

		data = Serial2.readStringUntil('\r');
		if (data.startsWith("\n")) data = data.substring(1);
		if (data.endsWith("\n")) data = data.substring(0, data.length() - 2);
		data.trim();
	}
	/*
	else if (Serial2.available() > 0)
		while (c != '\n' && c != '\r'  && cnt++ < MAX) {
			// read the incoming bytes:
			c = (char)Serial2.read();
			if ((int)c > -1) {
				cnt = 0;
				data += c;
			}
		}
	*/

	data.trim();

	if (data != "")
	{
		/*
		Serial.print("RX (");
		Serial.print(d);
		Serial.print("):");
		Serial.println(data);
		*/

		cmd = data.substring(0, data.indexOf('='));
		val = data.substring(data.indexOf('=') + 1);

		cmd.toUpperCase();
		val.toUpperCase();

		/*
		if (d == 2)
		{
			Serial.print("RX (");
			Serial.print(d);
			Serial.print("):");
			Serial.println(data);

			Serial.print(cmd);
			Serial.print(" => ");
			Serial.println(val);

			if (cmd == "HOSTACTIVE")
				Settings::DacHostActive = (val == "1");
			else if (cmd == "AUDIOSTREAMING")
				Settings::DacAudioStreaming = (val == "1");
			else if (cmd == "SAMPLERATE")
			{
				if (val == "1") Settings::DacSamplingRate = 44.1;
				else if (val == "2") Settings::DacSamplingRate = 48;
				else if (val == "3") Settings::DacSamplingRate = 88.2;
				else if (val == "4") Settings::DacSamplingRate = 96;
				else if (val == "5") Settings::DacSamplingRate = 176.4;
				else if (val == "6") Settings::DacSamplingRate = 192;
				else if (val == "7") Settings::DacSamplingRate = 352.8;
				else if (val == "8") Settings::DacSamplingRate = 384;
			}
			Preamp::DacLastSeenActive = millis();
		}
		*/
	}


	return (data != "");
}






#ifdef _IRactive
String Preamp::ProcessIR()
{
	String c = "";
	if (irrecv.decode(&results)) {
		// digitalWrite(STATUS_PIN, HIGH);
		//LcdMenu::InvertDisplay(1);
		storeCode(&results);
		irrecv.resume(); // resume receiver
		// digitalWrite(STATUS_PIN, LOW);
		//LcdMenu::InvertDisplay(0);
		c = myIRdecode(codeValue);

		Serial.print(F("IR: "));
		Serial.println(codeValue);
		Serial.print(F("decodes as: "));
		Serial.println(c);

	}

	return (c);
}


// gaj remote codes
/*
unsigned long Preamp::myIRCodes[21] = { 16753245, 16736925, 16769565, 16720605, 16712445, 16761405, 16769055, 16754775, 16748655, 16738455, 16750695, 16756815, 16724175, 16718055, 16743045, 16716015, 16726215, 16734885, 16728765, 16730805, 16732845 };
String Preamp::myIRValues[21] = { "CH-", "CH", "CH+", "PREV", "NEXT", "PLAY", "VOL-", "VOL+", "EQ", "0", "100+", "200+", "1", "2", "3", "4", "5", "6",  "7", "8", "9" };
*/
String Preamp::myIRdecode(unsigned long c)
{
	/*
  for(int i = 0; i < sizeof(myIRCodes); i++)
  {
    if (myIRCodes[i] == c)
      return myIRValues[i];
  }*/
	switch(c) {
	case 3772833823:
		//LcdMenu::ChangeVolume(1, &Preamp::Volume[Preamp::SelIn], Preamp::VolIncrement, _e2p_Volume + Preamp::SelIn, true, false);
		return ("VOL+");
	case 3772829743:
		//LcdMenu::ChangeVolume(0, &Preamp::Volume[Preamp::SelIn], Preamp::VolIncrement, _e2p_Volume + Preamp::SelIn, true, false);
		return ("VOL-");
	}
  return ((String)c);
}


// Storage for the recorded code
int Preamp::codeType = -1; // The type of code
unsigned long Preamp::codeValue; // The code value if not raw
//unsigned int Preamp::rawCodes[RAWBUF]; // The durations if raw
int Preamp::codeLen; // The length of the code
//int Preamp::toggle = 0; // The RC5/6 toggle state
decode_results Preamp::results = decode_results();

// Stores the code for later playback
// Most of this code is just logging
void Preamp::storeCode(decode_results *results) {
  codeType = results->decode_type;
  int count = results->rawlen;
  if (codeType == UNKNOWN) {
	  /*
	//Serial.println("Received unknown code, saving as raw");
	codeLen = results->rawlen - 1;
	// To store raw codes:
	// Drop first value (gap)
	// Convert from ticks to microseconds
	// Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
	for (int i = 1; i <= codeLen; i++) {
	  if (i % 2) {
		// Mark
		rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
		//Serial.print(" m");
	  }
	  else {
		// Space
		rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
		//Serial.print(" s");
	  }
	  //Serial.print(rawCodes[i - 1], DEC);
	}
	//Serial.println("");
	*/
  }
  else {/*
    if (codeType == NEC) {
      //Serial.print("Received NEC: ");
      if (results->value == REPEAT) {
        // Don't record a NEC repeat value as that's useless.
        //Serial.println("repeat; ignoring.");
        return;
      }
    }
    else if (codeType == SONY) {
      //Serial.print("Received SONY: ");
    }
    else if (codeType == RC5) {
      //Serial.print("Received RC5: ");
    }
    else if (codeType == RC6) {
      //Serial.print("Received RC6: ");
    }
    else {
      //Serial.print("Unexpected codeType ");
      //Serial.print(codeType, DEC);
      //Serial.println("");
    }
    */
    //Serial.println(results->value, HEX);
    codeValue = results->value;
    codeLen = results->bits;
  }
}
#endif
