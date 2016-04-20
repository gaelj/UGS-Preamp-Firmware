#ifndef LCDMENU_ICONS_H_
#define LCDMENU_ICONS_H_

#include <Arduino.h>
#include <avr/pgmspace.h>

#define PAGE_ALL		0
#define PAGE_MAIN		1
#define PAGE_SOURCES	2
#define PAGE_CTRL		3
#define PAGE_STANDBY	4
#define PAGE_HOURGLASS	0xFF

#define VOL_BAR_Y_POS 	192
#define VU_Y_POS 		340
#define VOL_BAR_HEIGHT 	16
#define VU_ITERATIONS 	6 // max 64 if value is averaged

#define DAC_TIMEOUT 	2000 // ms



#include "icon.h"
#include "pinout.h"
#include <Adafruit_RA8875.h>

#include "preamp.h"
#include "ugs.h"
#include "layout.h"
#include "settings.h"






class IconsPage
{
	public:
		static Adafruit_RA8875 tft;

		static int begin();
		static void ShowPage(int page_num = current_page);
		static void ProcessTouchEvents();
		static void FillScreenGradient(uint16_t Backcolor, uint16_t Backcolor2);
		static void FadeIn();
		static void FadeOut(bool log = true);
		static void DrawVolumeBar(bool drawContour, int16_t volume, int16_t prevVolume, int16_t min, int16_t max);
		static void DrawHourglass();
		static void SimulateScreenTouch();
		static void DisplayDacStatus();
		static void DisplayText(String Text);
		static void DisplayVuMeter(int channel, int value);

		static Icon icons[MAX_ICONS];
		static uint16_t current_page;

		static Layout layout_standby;
		static Layout layout_default; 	// icon layout parameters
		static Layout layout_hourglass; // icon layout parameters
		static Layout layout_sources; 	// icon layout parameters
		static Layout layout_control; 	// icon layout parameters

	private:
		static void DrawHorizontalBar(int x0, int y0, bool drawContour, int16_t value, int16_t prevValue, int16_t min, int16_t max, int16_t height, uint16_t frontColor);
		static void DoPushedIconAction(uint16_t tch_x, uint16_t tch_y);
		static void FingerWasLifted();
		static void DoTouchedIconAction();
		static void DoHeldIconAction();
		static void ListColors();
		static String PageName(int page);
		static unsigned long last_press;
		static int8_t last_touched_icon;
		static int8_t touched_icon;
		static int8_t selected_icon;
		static int8_t held_icon;
		static unsigned long initial_press;
		static unsigned int prevVuValue[4];

		static boolean current_tft_faded_in;
};




#endif /* LCDMENU_ICONS_H_ */
