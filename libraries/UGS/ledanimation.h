/*
 * ledanimation.h
 *
 *  Created on: 7 nov. 2015
 *      Author: gael
 */

#ifndef LEDANIMATION_H_
#define LEDANIMATION_H_

#define NB_ANIMATION_STATES					14

#include <Arduino.h>
#include "settings.h"
#include "preamp.h"

#define ugs_state_default				0
#define ugs_state_is_in_standby			1
#define ugs_state_is_in_power_on		2
#define ugs_state_is_in_mute			3
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

#define animation_none					0

// first 4 bits: volume
#define animation_volm_volume			1
#define animation_volm_blinking_all		2
#define animation_volm_on_all			3
#define animation_volm_vu_meter			4
#define animation_rear_chase_cw			5
#define animation_rear_chase_ccw		6

// last 4 bits: rear leds
#define animation_rear_active_inputs	7
#define animation_rear_blinking_all		 2
#define animation_rear_on_all			 3
#define animation_rear_chase_parallel	8
#define animation_rear_chase_serial		9

class LedAnimation
{
public:
	static void Init();
	static void Loop();
	static void SetUgsState(uint8_t state_id);
	static uint8_t bufferLedsRear[2];
	static uint16_t bufferLedsVolume;


private:
	static uint8_t GetAnimationIdVolume(uint8_t state);
	static uint8_t GetAnimationIdRelays(uint8_t state);
	static uint8_t GetUgsState();

	static uint8_t currentStateId;
	static long startTimeVolume;
	static long startTimeRelay;
	static uint8_t repeatsCounter_v;
	static uint8_t repeatsCounter_r;

	static void DoAnim_None(uint16_t frameNr);

	static void DoAnim_volm_Volume(uint16_t frameNr, bool pointOnly);
	static void DoAnim_volm_BlinkingAll(uint16_t frameNr);
	static void DoAnim_volm_OnAll(uint16_t frameNr);
	static void DoAnim_volm_VuMeter(uint16_t frameNr);
	static void DoAnim_volm_ChaseCw(uint16_t frameNr);
	static void DoAnim_volm_ChaseCcw(uint16_t frameNr);

	static void DoAnim_rear_ActiveInputs(uint16_t frameNr);
	static void DoAnim_rear_BlinkingAll(uint16_t frameNr);
	static void DoAnim_rear_OnAll(uint16_t frameNr);
	static void DoAnim_rear_ChaseParallelBoards(uint16_t frameNr);
	static void DoAnim_rear_ChaseSerialBoards(uint16_t frameNr);
};

#endif /* LEDANIMATION_H_ */
