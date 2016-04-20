/*
 * usg.cpp
 *
 *  Created on: 15 août 2015
 *      Author: gael
 */

#include "ugs.h"

unsigned long Ugs:: lastWriteLoopTime = 0;

int Ugs::begin()
{
	Serial.begin(COM_TO_PC_BITRATE);

	Ser::SerialPrintSep(F("#-#-"));

	Preamp::begin();

	if (!Settings::IsEepromValid()) {
		Serial.println(F("Format e2p"));
		Settings::WriteDefaultValuesToAllPresets();
		// Serial.print(F("Done: "));
		// Serial.println(millis());
	}

	Ser::SerialPrintSep(F("-"));

	Settings::LoadCurrentActivePreset();
	Settings::CurrentPresetGroup.LoadValuesToSettings(Settings::CurrentPresetGroup.Values);
	Settings::CurrentPresetGroup.DumpSettingsToSerial();

	LedAnimation::Init();

	IconsPage::begin();
	IconsPage::ShowPage(PAGE_STANDBY);

	Ser::SerialPrintSep(F("-"));
	Settings::SendStatusToSerial();
	Ser::SerialPrintSep(F("#"));

	return (1);
}

void Ugs::Loop()
{
	long StartMillis = millis();

	Ugs::ProcessTouchEvents();				// Serial.print(F("TouchEvents:   ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Ugs::ProcessSerial();					// Serial.print(F("Serial:        ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Ugs::ProcessEncoder();					// Serial.print(F("Encoder:       ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Ugs::ProcessPush();						// Serial.print(F("Push:          ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Ugs::ProcessLedAnimations();			// Serial.print(F("LedAnimations: ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Ugs::ProcessVuMeter();					// Serial.print(F("VuMeter:       ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Ugs::ProcessDacStatus();				// Serial.print(F("DacStatus:     ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Ugs::ProcessIR();						// Serial.print(F("IR:            ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Settings::CurrentPresetGroup.DelayedActions();	// Serial.print(F("PresetLoop:    ")); Serial.println((millis() - StartMillis)); StartMillis = millis();
	Preamp::DelayedActions();
	Ugs::ProcessLoopTime(StartMillis);
}

void Ugs::ProcessLoopTime(long StartMillis)
{
	if (millis() < (250 + lastWriteLoopTime))
		return;

	long _loopTime = millis() - StartMillis;

	if (Settings::loopTime != 0 && ((Settings::loopTime + 5) > _loopTime) && (Settings::loopTime < (_loopTime + 5)))
		return;

	Settings::loopTime = _loopTime;
	Ser::SerialPrint(_CMD_LOOPTIME, Settings::loopTime);
	lastWriteLoopTime = millis();
}

void Ugs::IncrementVolume()
{
	uint16_t newVol = (Settings::Volume() + Settings::VolIncrement);

	if (newVol > Settings::MaxVolumes[Settings::current_input]) newVol = Settings::MaxVolumes[Settings::current_input];
	else if (Settings::Volume() == 0) newVol = Settings::MinVolumes[Settings::current_input];

	Ugs::SetVolume(newVol);
}

void Ugs::DecrementVolume()
{
	uint16_t newVol = (Settings::Volume() - Settings::VolIncrement);

	if (newVol < 0) newVol = 0;
	else if (Settings::Volume() == Settings::MinVolumes[Settings::current_input]) newVol = 0;
	else if (newVol < Settings::MinVolumes[Settings::current_input]) newVol = Settings::MinVolumes[Settings::current_input];

	Ugs::SetVolume(newVol);
}

void Ugs::SetVolume(uint8_t volume)
{
	/*
	 Serial.print("previous volume: ");
	 Serial.println(Preamp::PreviousVolume);

	 Serial.print("volume: ");
	 Serial.println(volume);
	 */

	// anti-skip (touchscreen)
	if (Preamp::PreviousVolume > 20 && volume == 0) return;

	uint8_t previousVolume = Preamp::PreviousVolume;
	Preamp::SetVolume((uint8_t)volume);

	if (IconsPage::current_page == PAGE_MAIN) {
		IconsPage::DrawVolumeBar(0, Settings::Volume(), previousVolume, 0, _MUSES_VolumeSteps);

		if (Preamp::MaxVolumeReached)
			IconsPage::icons[ICONS_VolUp].Draw(IconsPage::tft, true, false, false);
		else
			IconsPage::icons[ICONS_VolUp].Draw(IconsPage::tft, false, false, false);
	}

	// empty serial buffer
	String cmd;
	String val;
	Preamp::ProcessSerial(cmd, val);
}

void Ugs::SetToStandby()
{
	Preamp::SetMute(1);

	IconsPage::DrawHourglass();

	Preamp::SetPower(0, 0);
	Preamp::SetPower(1, 0);

	IconsPage::ShowPage(PAGE_STANDBY);

	// empty serial buffer
	String cmd;
	String val;
	Preamp::ProcessSerial(cmd, val);
}

void Ugs::StartToMainMenu()
{
	IconsPage::DrawHourglass();

	Preamp::SetPowerDelayed(0, 1); // main power

	if (Settings::UseDefOrLastInput) {
		// Serial.print("Setting Input to last value: ");
		// Serial.println(Settings::current_input);
		uint8_t inp = Settings::current_input;
		// Settings::current_input = 0xFF;
		Ugs::SetInput(inp);
	} else {
		// Serial.print("Setting Input to default value: ");
		// Serial.println(Settings::DefaultInput);
		// int16_t inp = Settings::current_input;
		// Settings::current_input = 0xFF;
		Ugs::SetInput(Settings::DefaultInput);
	}

	// Preamp::SetMute(0);

	// in last position to reflect correct UI values
	IconsPage::ShowPage(PAGE_MAIN);

	// empty serial buffer
	String cmd;
	String val;
	Preamp::ProcessSerial(cmd, val);
}

void Ugs::SetInput(uint8_t input)
{
	Preamp::SelectInput(input);
}

void Ugs::ProcessDacStatus()
{
	if (Preamp::DacLastSeenActive + (float) DAC_TIMEOUT < millis()) {
		if (!Preamp::DacIsDown) {
			Preamp::DacIsDown = true;
			IconsPage::DisplayDacStatus();
		}
	} else {
		if (Preamp::DacIsDown) {
			Preamp::DacIsDown = false;
			IconsPage::DisplayDacStatus();
		}
	}
}

void Ugs::ProcessIR()
{
	String ir = Preamp::ProcessIR();

	if (ir == F("VOL+"))
		IncrementVolume();
	if (ir == F("VOL-"))
		DecrementVolume();
}

bool Ugs::ProcessPush()
{
	long p = Preamp::ProcessPush();

	if (p > 0) {
		// Push released

		if (Settings::Power[0] == 0)
			Ugs::StartToMainMenu();
		else if (p < 1000) {
			// short push (< 1s)
			Preamp::SetMute(!Settings::Mute);
		}
		else {
			// long push (>= 1s)
			Ugs::SetToStandby();
		}

		return (true);
	}
	else if (p == 0) {
		// Push active
		return (false);
	}
	else {
		// -1: No push
		return (false);
	}
}

int8_t Ugs::ProcessEncoder()
{
	int8_t ret = Preamp::ProcessEncoder();

	if (ret > 0)
		IncrementVolume();
	else if (ret < 0)
		DecrementVolume();

	return (ret);
}

void Ugs::ProcessTouchEvents() {
	IconsPage::ProcessTouchEvents();
}



void Ugs::ProcessLedAnimations()
{
	LedAnimation::Loop();
}

void Ugs::ProcessVuMeter()
{
	Preamp::ReadVuMeters();

	// for(byte i = 0; i < 2; i++) {
	// 	IconsPage::DisplayVuMeter(i, Preamp::VuMeter[i]);
	// }

	// for(byte i = 0; i < 2; i++)
	// 	Preamp::SetIllumination_Vol(i, Preamp::VuMeter[i] * 0xFF / 0x3FF);

	// if (val[0] > 600 || val[1] > 600)
	// 	Preamp::SetRearLeds(0, 0xFFFF);
	// else
	// 	Preamp::SetRearLeds(0, 0);
}



void Ugs::ProcessSerial()
{
	String cmd = "";
	String val = "";

	if (Preamp::ProcessSerial(cmd, val)) {
		// A command has been recevied by serial

		// Serial.print("RX cmd: "); 	Serial.println(cmd);
		// Serial.print("RX val: "); 	Serial.println(val);

		// if (String(val.toInt()) != val)
		// 	Serial.println("not int");

		// Ser::SerialPrintSep("-");

		if (cmd == _CMD_VOLUME) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			Ser::DoSerialPrint = 0;

			if (val == "+") {
				IncrementVolume();
			} else if (val == "-") {
				DecrementVolume();
			} else if (String(val.toInt()) == val) {
				SetVolume(val.toInt());
			}

			Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_STANDBY) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			if (val == "0") {
				StartToMainMenu();
			} else if (val == "1") {
				SetToStandby();
			}
		}

		else if (cmd == _CMD_RIAA_PWR) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			if (val == "0") {
				Preamp::SetPower(1, 0);
			} else if (val == "1") {
				Preamp::SetPower(1, 1);
			}
		}

		else if (cmd == _CMD_MUTE) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			if (val == "0") {
				Preamp::SetMute(0);
			} else if (val == "1") {
				Preamp::SetMute(1);
			}
		}

		else if (cmd == _CMD_BALANCE) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			Ser::DoSerialPrint = 0;

			if (String(val.toInt()) == val) {

				Preamp::SetBalance(val.toInt());
			}

			Ser::DoSerialPrint = 1;
		}

		else if (cmd.startsWith(_CMD_OFFSET)) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			int index = cmd.substring(sizeof(_CMD_OFFSET) - 1).toInt();

			Ser::DoSerialPrint = 0;

			if (String(val.toInt()) == val) {
				Settings::Offset[index] = val.toInt();
				SetVolume(Settings::Volume());
			}

			Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_INPUT) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Ugs::SetInput(val.toInt());
		}

		else if (cmd == _CMD_OUT_ASYM) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Preamp::SetOutAsym((bool) val.toInt());
		}

		else if (cmd == _CMD_EXT_OUT) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Preamp::SetExtOut((bool) val.toInt());
		}

		else if (cmd == _CMD_BYPASS) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Preamp::SetBypass((bool) val.toInt());
		}

		else if (cmd == _CMD_DACSRC) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			// Ser::DoSerialPrint = 0;
			Preamp::SetDacSource((bool) val.toInt());
			// Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_ILLUM_TFT) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Ser::DoSerialPrint = 0;
			Preamp::SetIllumination_TFT(val.toInt());
			Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_ILLUM_VOL1) {
			Ser::DoSerialPrint = 0;
			Preamp::SetIllumination_Vol(0, val.toInt());
			Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_ILLUM_VOL2) {
			Ser::DoSerialPrint = 0;
			Preamp::SetIllumination_Vol(1, val.toInt());
			Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_ILLUM_RLY1) {
			Ser::DoSerialPrint = 0;
			Preamp::SetIllumination_Rear(0, val.toInt());
			Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_ILLUM_RLY2) {
			Ser::DoSerialPrint = 0;
			Preamp::SetIllumination_Rear(1, val.toInt());
			Ser::DoSerialPrint = 1;
		}

		else if (cmd == _CMD_RESET) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Preamp::ResetArduino();
		}

		else if (cmd == _CMD_GET_STATUS) {
			Settings::SendStatusToSerial();
		}

		else if (cmd == _CMD_GET_PRESETS) {
			// Settings::CurrentPresetGroup.DumpSettingsToSerial();
			// do not remove this empty section !
		}

		else if (cmd == _CMD_GET_ICONS) {
			char hexv[3];

			Serial.print(_CMD_ICONS);
			Serial.print("=");

			for (uint16_t i = 0; i < sizeof(Icon::IconDefCache); i++) {

				if (Icon::IconDefCache[i - 1] == 0xFF
						&& Icon::IconDefCache[i] == 0
						&& Icon::IconDefCache[i + 1] == 0
						&& Icon::IconDefCache[i + 2] == 0
						&& Icon::IconDefCache[i + 3] == 0
						&& Icon::IconDefCache[i + 4] == 0)
					break;

				sprintf(hexv, "%02x", Icon::IconDefCache[i]);
				Serial.print(String(hexv));
			}

			Serial.println("");
		}

		else if (cmd == _CMD_GET_ALL_PRESETS) {
			Settings::DumpAllToSerial();
		}

		else if (cmd == _CMD_GET_NB_RLYBRD) {
			Ser::SerialPrint(_CMD_GET_NB_RLYBRD, (uint8_t)_NbRlyBrds);
		}

		else if (cmd == _CMD_GET_NB_VOLBRD) {
			Ser::SerialPrint(_CMD_GET_NB_VOLBRD, (uint8_t)_NbVolBrds);
		}

		else if (cmd == _CMD_LOOPTIME) {
			Ser::SerialPrint(_CMD_LOOPTIME, Settings::loopTime);
		}

		else if (cmd.startsWith(_CMD_EEPROM)) {

			uint8_t eepWordSize = 7; // length of word "EEPROM"
			uint8_t lastFrameId = (_e2p_NbAddresses / 20);

			uint8_t eepromGroupNumber = (cmd.substring(eepWordSize, eepWordSize + 1)).toInt();
			char messageLetter = cmd.substring(eepWordSize + 1, eepWordSize + 2).charAt(0);
			char _A = 'A';
			uint8_t msgNumber = (messageLetter - _A);

			if (msgNumber == 0)
				IconsPage::DrawHourglass();

			Settings s = Settings(eepromGroupNumber);

			if (s.ExtractValuesFromSerialString(msgNumber, val)) {
				/*
				Serial.print(msgNumber);
				Serial.print(" ");
				Serial.println(lastFrameId);
				*/
				if (msgNumber == lastFrameId) {
					Settings::SetCurrentActivePresetNumber(eepromGroupNumber);
					s.LoadValuesToSettings(Settings::deser);
					s.WriteSettingsToEeprom();
					Settings::CurrentPresetGroup = s;
					// Preamp::ResetArduino();
					if (Settings::Power[0])
						IconsPage::ShowPage(PAGE_MAIN);
					else
						IconsPage::ShowPage(PAGE_STANDBY);

					LedAnimation::SetUgsState(ugs_state_settings_updated);
				}
			}

			Ser::SerialPrintSep(F("#"));
			return;

		} else if (cmd == _CMD_PRESET) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			IconsPage::DrawHourglass();

			int nb = val.toInt();

			Settings::SetCurrentActivePresetNumber(nb);
			Settings s = Settings(nb);
			s.LoadValuesToSettings(s.Values);
			Settings::CurrentPresetGroup = s;
			Settings::CurrentPresetGroup.DumpSettingsToSerial();

			// Preamp::ResetArduino();
			if (Settings::Power[0])
				IconsPage::ShowPage(PAGE_MAIN);
			else
				IconsPage::ShowPage(PAGE_STANDBY);

		} else if (cmd == _CMD_ICONS) {
			// wake up display
			IconsPage::SimulateScreenTouch();

			IconsPage::DrawHourglass();

			byte data[val.length() / 2];
			for (uint16_t i = 0; i < val.length(); i += 2)
				data[i / 2] = (byte) (val.substring(i, i + 1)).toInt();

			Icon::E2pCreateIcons(val.length() / 2, data);
			Icon::E2pGetIcons();

			if (Settings::Power[0])
				IconsPage::ShowPage(PAGE_MAIN);
			else
				IconsPage::ShowPage(PAGE_STANDBY);

		} else if (cmd == _CMD_RESET_ICONS) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			IconsPage::DrawHourglass();

			Icon::E2pCreateDefaultIcons();
			Icon::E2pGetIcons();

			if (Settings::Power[0])
				IconsPage::ShowPage(PAGE_MAIN);
			else
				IconsPage::ShowPage(PAGE_STANDBY);
		}

		else if (cmd == _CMD_CUR_FACTORY) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Settings::CurrentPresetGroup.WriteDefaultValuesToEeprom();
			Settings::CurrentPresetGroup.ReadValuesFromEeprom();
			Settings::CurrentPresetGroup.DumpSettingsToSerial();

			/*
			Serial.println(F("LoadValueToSettings Values"));
			Serial.println(F("Values:"));

			for (int i = 0; i < _e2p_NbAddresses; i++) {
				Serial.print(i);
				Serial.print(F(" "));
				Serial.println(Settings::CurrentPresetGroup.Values[i]);
			}
			*/

			Settings::CurrentPresetGroup.LoadValuesToSettings(
					Settings::CurrentPresetGroup.Values);
		}

		else if (cmd == _CMD_ALL_FACTORY) {
			// wake up display
			IconsPage::SimulateScreenTouch();
			Settings::WriteDefaultValuesToAllPresets();
			Settings::CurrentPresetGroup.ReadValuesFromEeprom();
			Settings::CurrentPresetGroup.DumpSettingsToSerial();

			Settings::CurrentPresetGroup.LoadValuesToSettings(
					Settings::CurrentPresetGroup.Values);
		}

		// DAC signals
		else if (cmd == _CMD_HOSTACTIVE) {
			Preamp::DacLastSeenActive = millis();
			Preamp::DacIsDown = false;
			if (Settings::DacHostActive != (val == "1")) {
				Settings::DacHostActive = (val == "1");
				IconsPage::DisplayDacStatus();
			}
			return;
		}

		else if (cmd == _CMD_AUDIOSTREAMING) {
			Preamp::DacLastSeenActive = millis();
			Preamp::DacIsDown = false;
			if (Settings::DacAudioStreaming != (val == "1")) {
				Settings::DacAudioStreaming = (val == "1");
				IconsPage::DisplayDacStatus();
			}
			return;
		}

		else if (cmd == _CMD_SAMPLERATE) {
			Preamp::DacLastSeenActive = millis();
			Preamp::DacIsDown = false;
			if (Settings::DacSamplingRate != (val.toInt())) {
				Settings::DacSamplingRate = val.toInt();
				IconsPage::DisplayDacStatus();
			}
			return;
		}

		else if (cmd == _CMD_DAC_SOURCE) {
			Preamp::DacLastSeenActive = millis();
			Preamp::DacIsDown = false;

			// correct the source if it is not
			bool expectedDacSrc = (bool) Settings::input_trigger[Settings::current_input];
			if (expectedDacSrc != (val.toInt())) {
				Ser::DoSerialPrint = 0;
				Preamp::SetDacSource(expectedDacSrc);
				Ser::DoSerialPrint = 1;
			}

			return;
		}

		else if (cmd == _CMD_HELP) {
			Serial.println(F("UGS Keywords:"));
			Serial.println();
			Serial.println(F("Get value:"));
			Serial.println(_CMD_GET_STATUS);
			Serial.println(_CMD_GET_PRESETS);
			Serial.println(_CMD_GET_ALL_PRESETS);
			Serial.println(_CMD_GET_ICONS);
			Serial.println(_CMD_GET_NB_RLYBRD);
			Serial.println(_CMD_GET_NB_VOLBRD);
			Serial.println();

			Serial.println(F("Set value:"));
			Serial.println(_CMD_VOLUME);
			Serial.println(_CMD_STANDBY);
			Serial.println(_CMD_RIAA_PWR);
			Serial.println(_CMD_MUTE);
			Serial.println(_CMD_BALANCE);
			Serial.println(_CMD_OFFSET);
			Serial.println(_CMD_INPUT);
			Serial.println(_CMD_ILLUM_TFT);
			Serial.println(_CMD_ILLUM_VOL1);
			Serial.println(_CMD_ILLUM_VOL1);
			Serial.println(_CMD_ILLUM_RLY1);
			Serial.println(_CMD_ILLUM_RLY2);
			Serial.println(_CMD_PRESET);
			Serial.println(_CMD_EEPROM);
			Serial.println(_CMD_ICONS);
			Serial.println(_CMD_OUT_ASYM);
			Serial.println(_CMD_EXT_OUT);
			Serial.println(_CMD_BYPASS);
			Serial.println(_CMD_DACSRC);
			Serial.println();

			Serial.println(F("Command:"));
			Serial.println(_CMD_RESET);
			Serial.println(_CMD_RESET_ICONS);
			Serial.println(_CMD_CUR_FACTORY);
			Serial.println(_CMD_ALL_FACTORY);
			Serial.println();

		}

		else {
			Serial.print(F("UGS: error: "));
			Serial.println(cmd);
			return;
		}

		Settings::CurrentPresetGroup.WriteSettingsToEeprom();
		Settings::CurrentPresetGroup.RequestDumpToSerialDelayed();
		Ser::SerialPrintSep(F("#"));

	}
}
