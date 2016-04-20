/*
 * ledanimation.cpp
 *
 *  Created on: 7 nov. 2015
 *      Author: gael
 */

#include "ledanimation.h"


uint8_t LedAnimation::bufferLedsRear[2] = { 0 };
uint16_t LedAnimation::bufferLedsVolume = 0;

uint8_t LedAnimation::currentStateId = ugs_state_default;
long LedAnimation::startTimeVolume = 0;
long LedAnimation::startTimeRelay = 0;
uint8_t LedAnimation::repeatsCounter_v = 0;
uint8_t LedAnimation::repeatsCounter_r = 0;


void LedAnimation::Init()
{
	Settings::Illumination_Vol1 = Settings::Illumination_Vol1_H;
	Settings::Illumination_Vol2 = Settings::Illumination_Vol2_H;

	Settings::Illumination_Rear1 = Settings::Illumination_Rly1_H;
	Settings::Illumination_Rear2 = Settings::Illumination_Rly2_H;

	Preamp::SetIllumination_Vol(0, Settings::Illumination_Vol1);
	Preamp::SetIllumination_Vol(1, Settings::Illumination_Vol2);

	Preamp::SetIllumination_Rear(0, Settings::Illumination_Rear1);
	Preamp::SetIllumination_Rear(1, Settings::Illumination_Rear2);

	currentStateId = ugs_state_default;
	repeatsCounter_v = 0;
	repeatsCounter_r = 0;
}

void LedAnimation::SetUgsState(uint8_t state_id)
{
	currentStateId = state_id;
	startTimeVolume = millis();
	startTimeRelay = millis();
	repeatsCounter_v = 0;
	repeatsCounter_r = 0;

	LedAnimation::Loop();
}

void LedAnimation::Loop()
{
	long now = millis();
	long runningForVolume = now - startTimeVolume;
	long runningForRelay = now - startTimeRelay;
	uint8_t state = LedAnimation::GetUgsState();

	uint8_t vaid = LedAnimation::GetAnimationIdVolume(state);
	uint8_t raid = LedAnimation::GetAnimationIdRelays(state);

	uint16_t frameNrV = runningForVolume / ((long)Settings::AnimationStateDelays_v[state] * 25);
	uint16_t frameNrR = runningForRelay  / ((long)Settings::AnimationStateDelays_r[state] * 25);

	/*
	Serial.print(F("Volume animation id: "));
	Serial.print(vaid);
	Serial.print(F(" Rear animation id: "));
	Serial.print(raid);

	Serial.print(F("Volume frame: "));
	Serial.print(frameNrV);
	Serial.print(F(" Rear frame: "));
	Serial.println(frameNrR);
	*/

	for (uint8_t i = 0; i < _NbRlyBrds; i++)
		LedAnimation::bufferLedsRear[i] = 0;
	LedAnimation::bufferLedsVolume = 0;

	switch (vaid) {
		case animation_volm_volume:			DoAnim_volm_Volume(frameNrV, false); break;
		case animation_volm_blinking_all:	DoAnim_volm_BlinkingAll(frameNrV); break;
		case animation_volm_on_all:			DoAnim_volm_OnAll(frameNrV); break;
		case animation_volm_vu_meter:		DoAnim_volm_VuMeter(frameNrV); break;
		case animation_rear_chase_cw:		DoAnim_volm_ChaseCw(frameNrV); break;
		case animation_rear_chase_ccw:		DoAnim_volm_ChaseCcw(frameNrV); break;
		default: 							DoAnim_None(frameNrV); break;
	}

	switch (raid) {
		case animation_rear_active_inputs:	DoAnim_rear_ActiveInputs(frameNrR); break;
		case animation_rear_blinking_all:	DoAnim_rear_BlinkingAll(frameNrR); break;
		case animation_rear_on_all:			DoAnim_rear_OnAll(frameNrR); break;
		case animation_rear_chase_parallel:	DoAnim_rear_ChaseParallelBoards(frameNrR); break;
		case animation_rear_chase_serial:	DoAnim_rear_ChaseSerialBoards(frameNrR); break;
		default: 							DoAnim_None(frameNrR); break;
	}

	Preamp::SetIllumination_Vol(0, Settings::Illumination_Vol1);
	Preamp::SetIllumination_Vol(1, Settings::Illumination_Vol2);

	Preamp::SetIllumination_Rear(0, Settings::Illumination_Rear1);
	Preamp::SetIllumination_Rear(1, Settings::Illumination_Rear2);

	Preamp::SetRearLedsAndRelays();
	Preamp::SetVolumeLeds(bufferLedsVolume);
}

uint8_t LedAnimation::GetUgsState()
{

	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];

	if (currentStateId >= ugs_state_standby_off) {

		/*
		Serial.print(F("Current state: "));
		Serial.print(currentStateId);
		Serial.print(F(" maxRepV: "));
		Serial.print(maxRepV);
		Serial.print(F(" repeatsCounter_v: "));
		Serial.print(repeatsCounter_v);
		Serial.print(F(" maxRepR: "));
		Serial.print(maxRepR);
		Serial.print(F(" repeatsCounter_r: "));
		Serial.println(repeatsCounter_r);
		*/

		if (maxRepV > 0 && repeatsCounter_v > maxRepV)
			if (maxRepR > 0 && repeatsCounter_r > maxRepR)
				currentStateId = ugs_state_default;

		if (maxRepV == 0)
			if (maxRepR > 0 && repeatsCounter_r > maxRepR)
				currentStateId = ugs_state_default;

		if (maxRepV > 0 && repeatsCounter_v > maxRepV)
			if (maxRepR == 0)
				currentStateId = ugs_state_default;

		if (maxRepV == 0 && maxRepR == 0)
		currentStateId = ugs_state_default;

		/*
		if (currentStateId == ugs_state_default) {
			Serial.println(F("currentStateId=default"));
		}
		*/
	}

	if (currentStateId == ugs_state_default) {
		if (Settings::Power[0] == 0) currentStateId = (ugs_state_is_in_standby);
		else if (Settings::Mute == 1) currentStateId = (ugs_state_is_in_mute);
		else if (Settings::Power[0] == 1) currentStateId = (ugs_state_is_in_power_on);
	}


	return (currentStateId);
}

uint8_t LedAnimation::GetAnimationIdVolume(uint8_t state)
{
	return (Settings::anim_id_v[state]);
}

uint8_t LedAnimation::GetAnimationIdRelays(uint8_t state)
{
	return (Settings::anim_id_r[state]);
}




void LedAnimation::DoAnim_None(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	repeatsCounter_v = maxRepV + 1;
	repeatsCounter_r = maxRepR + 1;

	if (maxRepV > 0 && repeatsCounter_v > maxRepV) {
		if (maxRepR > 0 && repeatsCounter_r > maxRepR)
			currentStateId = ugs_state_default;
	}
	if (maxRepV == 0 && maxRepR == 0)
		currentStateId = ugs_state_default;
}

void LedAnimation::DoAnim_volm_Volume(uint16_t frameNr, bool pointOnly)
{
	uint8_t nbLedVol = (1 + (uint8_t)((14.0 * (float)(Settings::Volume() - Settings::MinVolumes[Settings::current_input]))
			/ (float)(Settings::MaxVolumes[Settings::current_input] - Settings::MinVolumes[Settings::current_input])));

	if (Preamp::MaxVolumeReached)
		bufferLedsVolume ^= 0xFFFF;
	else {
		if (pointOnly)
			bufferLedsVolume ^= (1 << nbLedVol - 1);
		else
			for(byte i = 1; i < nbLedVol; i++)
				bufferLedsVolume ^= (1 << i);
	}
}

void LedAnimation::DoAnim_volm_BlinkingAll(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepV > 0 && repeatsCounter_v > maxRepV) {
		if (maxRepR > 0 && repeatsCounter_r > maxRepR)
			currentStateId = ugs_state_default;
		return;
	}

	if (frameNr % 2 == 0) {
		bufferLedsVolume = 0xFFFF;
	} else {
		if (bufferLedsVolume != 0) {
			bufferLedsVolume = 0;
			if (repeatsCounter_v <= maxRepV)
				repeatsCounter_v++;
		}
	}
}
void LedAnimation::DoAnim_volm_OnAll(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepV > 0 && repeatsCounter_v > maxRepV) {
		if (maxRepR > 0 && repeatsCounter_r > maxRepR)
			currentStateId = ugs_state_default;
		return;
	}

	if (bufferLedsVolume != 0xFFFF) {
		if (repeatsCounter_v <= maxRepV)
			repeatsCounter_v++;

		bufferLedsVolume = 0xFFFF;
	}
}
void LedAnimation::DoAnim_volm_VuMeter(uint16_t frameNr)
{
	for(byte i = 0; i < 2; i++) {
		if (i == 0)
			for(byte j = 0; j < ((float)Preamp::VuMeter[0] / 128.0); j++)
				bufferLedsVolume |= (1 << j);
		else
			for(byte j = 0; j < ((float)Preamp::VuMeter[1] / 128.0); j++)
				bufferLedsVolume |= (0b1000000000000000 >> j);
	}
	DoAnim_volm_Volume(frameNr, true);
}
void LedAnimation::DoAnim_volm_ChaseCw(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepV > 0 && repeatsCounter_v > maxRepV) {
		if (maxRepR > 0 && repeatsCounter_r > maxRepR)
			currentStateId = ugs_state_default;
		return;
	}

	if (frameNr >= 16) {
		frameNr = 0;
		startTimeVolume = millis();
		if (repeatsCounter_v <= maxRepV)
			repeatsCounter_v++;
	}

	bufferLedsVolume = (1 << (frameNr));
}
void LedAnimation::DoAnim_volm_ChaseCcw(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepV > 0 && repeatsCounter_v > maxRepV) {
		if (maxRepR > 0 && repeatsCounter_r > maxRepR)
			currentStateId = ugs_state_default;
		return;
	}

	if (frameNr >= 16)  {
		frameNr = 0;
		startTimeVolume = millis();
		if (repeatsCounter_v <= maxRepV)
			repeatsCounter_v++;
	}

	bufferLedsVolume = (0b1000000000000000 >> (frameNr));
}




void LedAnimation::DoAnim_rear_ActiveInputs(uint16_t frameNr)
{
	for (uint8_t i = 0; i < _NbRlyBrds; i++) {
		bufferLedsRear[i] = 1 << Settings::input_source[Settings::current_input];
		if (Settings::ExtOut)
			bufferLedsRear[i] |= 1 << 7;
		else if (Settings::OutAssym)
			bufferLedsRear[i] |= 1 << 6;
		else
			bufferLedsRear[i] |= 1 << 5;
	}
}
void LedAnimation::DoAnim_rear_BlinkingAll(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepR > 0 && repeatsCounter_r > maxRepR) {
		if (maxRepV > 0 && repeatsCounter_v > maxRepV)
			currentStateId = ugs_state_default;
		return;
	}

	for (uint8_t i = 0; i < _NbRlyBrds; i++) {
		if (frameNr % 2 == 0) {
			bufferLedsRear[i] = 0x7F;
		} else {
			bufferLedsRear[i] = 0;
			if (repeatsCounter_r <= maxRepR)
				repeatsCounter_r++;
		}
	}
}
void LedAnimation::DoAnim_rear_OnAll(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepR > 0 && repeatsCounter_r > maxRepR) {
		if (maxRepV > 0 && repeatsCounter_v > maxRepV)
			currentStateId = ugs_state_default;
		return;
	}

	if (repeatsCounter_r <= maxRepR)
		repeatsCounter_r++;

	for (uint8_t i = 0; i < _NbRlyBrds; i++) {
		bufferLedsRear[i] = 0x7F;
	}
}
void LedAnimation::DoAnim_rear_ChaseParallelBoards(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepR > 0 && repeatsCounter_r > maxRepR) {
		if (maxRepV > 0 && repeatsCounter_v > maxRepV)
			currentStateId = ugs_state_default;
		return;
	}

	if (frameNr > 6) {
		startTimeRelay = millis();
		frameNr = 0;
		if (repeatsCounter_r <= maxRepR)
			repeatsCounter_r++;
	}
	for (uint8_t i = 0; i < _NbRlyBrds; i++) {
		bufferLedsRear[i] = 1 << (frameNr);
	}
}
void LedAnimation::DoAnim_rear_ChaseSerialBoards(uint16_t frameNr)
{
	uint8_t maxRepV = Settings::AnimationStateRepeats_v[currentStateId];
	uint8_t maxRepR = Settings::AnimationStateRepeats_r[currentStateId];
	if (maxRepR > 0 && repeatsCounter_r > maxRepR) {
		if (maxRepV > 0 && repeatsCounter_v > maxRepV)
			currentStateId = ugs_state_default;
		return;
	}

	if (frameNr > 13) {
		startTimeRelay = millis();
		frameNr = 0;
		if (repeatsCounter_r <= maxRepR)
			repeatsCounter_r++;
	}

	if (frameNr < 7) {
		bufferLedsRear[0] = 0x7F & (1 << (frameNr));
		bufferLedsRear[1] = 0;
	} else {
		// frameNr >= 7
		bufferLedsRear[0] = 0;
		bufferLedsRear[1] = 0x7F & (0b1000000 >> (frameNr - 7));
	}
}
