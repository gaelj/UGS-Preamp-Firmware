/*
 * icon.h
 *
 *  Created on: 16 août 2015
 *      Author: gael
 */

#ifndef ICON_H_
#define ICON_H_



#define ICONS_LargVolCr	0  // large volume circle (mute & vol+)
#define ICONS_VolComm   1  // volume common (mute, vol +-)
#define ICONS_Slash  	2  // slash (for mute)
#define ICONS_Aux_Cont  3  // aux container
#define ICONS_Symmout   4
#define ICONS_Bypass    5
#define ICONS_Sleep     6
#define ICONS_Tools     7
#define ICONS_Standby   8
#define ICONS_Mute 		9
#define ICONS_VolDown	10
#define ICONS_VolUp		11

#define ICONS_FIRST_IN  12

	#define ICONS_PC    12
	#define ICONS_TV    13
	#define ICONS_Vinyl 14
	#define ICONS_Cd    15
	#define ICONS_Aux1	16
	#define ICONS_Aux2	17

#define ICONS_LAST_IN   17

#define ICONS_Input     18
#define ICONS_Trigger   19
#define ICONS_Speaker   20

#define ICONS_Volume    21
#define ICONS_Balance   22
#define ICONS_Offset    23

#define ICONS_1         24
#define ICONS_2         25
#define ICONS_3         26
#define ICONS_4         27
#define ICONS_5         28
#define ICONS_6         29
#define ICONS_7         30
#define ICONS_8         31
#define ICONS_9         32
#define ICONS_0         33

#define ICONS_Clear     34

#define ICONS_Plus      35
#define ICONS_Minus     36

#define ICONS_Hourglass 37

#define ICONS_Presets   38
#define ICONS_Lighting	39

#define ICONS_Up  	    40
#define ICONS_Down  	41

#define MAX_ICONS		42
#define ICONS_END		128


#define NONE			-1
#define EMPTYSPACE		-2


#define IC_CM_FRR		1 // Fill round rectangle 	(5)
#define IC_CM_FR		2 // Fill rectangle			(4)
#define IC_CM_FR2		3 // Fill rectangle 2		(6)
#define IC_CM_FT		4 // Fill triangle			(6)
#define IC_CM_FC		5 // Fill cirle				(3)
#define IC_CM_FCV		6 // Fill curve				(5)
#define IC_CM_CP	   32 // Copy icon				(1)
#define IC_CM_NO	   48 // No icon				(0)
#define IC_CM_END	  255 // End command

#define IC_BC			128 // Back color
#define IC_FC			192 // Front color


#include <Arduino.h>
#include <HardwareSerial.h>
#include <Adafruit_RA8875.h>
#include "layout.h"
#include "settings.h"

class Icon
{
	public:
		Icon();
		Icon(Layout _layout, int8_t _icon, int16_t _i, int16_t _j, Adafruit_RA8875 tft,
				bool selected, bool touched, bool inactive);
		void Draw(Adafruit_RA8875 tft, bool selected, bool touched, bool inactive);
		static bool ObjectIsTouched(uint16_t tch_x, uint16_t tch_y, uint16_t pos_x, uint16_t pos_y, uint16_t size_x, uint16_t size_y);
		bool IconIsTouched(uint16_t tch_x, uint16_t tch_y);

		Layout layout;
		int8_t icon;
		int16_t positionX;
		int16_t positionY;

		bool isSelected;
		bool isTouched;
		bool isInactive;

		static void E2pGetIcons();
		static void E2pCreateDefaultIcons();
		static void E2pCreateIcons(int16_t size, byte data[]);
		static bool E2pAppendIconData(int16_t size, byte data[]);

		static byte IconDefCache[_e2p_MaxIconBytes];



	private:
		// DrawColoredIcon(Adafruit_RA8875 tft, uint16_t fc, uint16_t bc);
		void E2pDrawIcon(Adafruit_RA8875 tft, uint16_t fc, uint16_t bc, int8_t icon, int16_t offsetX = 0, int16_t offsetY = 0);

		static void fillRect2(Adafruit_RA8875 tft,
				int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2,
				uint16_t color);
		static void FillRectGradient(Adafruit_RA8875 tft, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t Backcolor);

		int32_t icon_pos_x;
		int32_t icon_pos_y;

		static uint16_t CurrentIconWriteAddress;
		static uint16_t IconAddresses[MAX_ICONS + 1];

};




static byte DefaultIconDefinition[] = {
	ICONS_LargVolCr,
		IC_CM_FC + IC_FC, 54, 47, 29, // large circle contour
		IC_CM_FC + IC_BC, 54, 47, 22, // large circle cutout

	ICONS_VolComm,
		IC_CM_FC + IC_FC, 54, 47, 15, // small circle contour
		IC_CM_FC + IC_BC, 54, 47,  8, // small circle cutout
		IC_CM_FR + IC_BC, 10, 10, 45, 74, // clear left half of circles
		IC_CM_FT + IC_FC, 47, 16, 47, 78, 17, 47, // speaker cone
		IC_CM_FR + IC_FC, 12, 36, 18, 22, // speaker magnet

	ICONS_Slash,
		IC_CM_FR2+ IC_FC, 11, 17, 78, 83, 83, 78, // right slash
		IC_CM_FR2+ IC_BC, 17, 11, 83, 78, 89, 73, // right slash cutout

	ICONS_Aux_Cont,
		IC_CM_FRR+ IC_FC,  4, 12, 75, 75, 8,
		IC_CM_FR + IC_BC, 12, 20, 63, 59,
		IC_CM_FR + IC_BC, 71, 12,  8, 79,
		IC_CM_FR + IC_BC,  4, 12, 71,  8,
		IC_CM_FRR+ IC_FC, 20,  4, 71, 71, 8,
		IC_CM_FR + IC_BC, 28, 12, 55, 55,

	ICONS_Symmout,
		IC_CM_FR2+ IC_FC,  1, 53, 23, 74, 28, 69,
		IC_CM_FR2+ IC_FC, 24, 53, 46, 74, 51, 70,
		IC_CM_FR2+ IC_FC, 46, 64, 51, 70, 94, 27,
		IC_CM_FR2+ IC_FC, 41, 47, 46, 52, 71, 28,

	ICONS_Bypass,
		IC_CM_FC + IC_FC, 44, 55, 39,
		IC_CM_FC + IC_BC, 44, 55, 28,
		IC_CM_FR + IC_BC,  5, 53, 84, 42,
		// IC_CM_FT + IC_BC,  1, 52,  1, 60, 18, 60,
		IC_CM_FT + IC_FC, 53, 53, 87, 53, 87, 18,

	ICONS_Sleep,
		IC_CM_NO + IC_BC,

	ICONS_Tools,
		IC_CM_FC + IC_FC, 30, 30, 26,
		IC_CM_FR2+ IC_BC,  5, 17, 23, 35, 35, 23,
		IC_CM_FR2+ IC_FC, 40, 54, 78, 94, 94, 78,
		IC_CM_FR + IC_BC, 74, 92, 10,  3,
		IC_CM_FR + IC_BC, 92, 74,  3, 10,

	ICONS_Standby,
		IC_CM_FC + IC_FC, 48, 48, 36,
		IC_CM_FC + IC_BC, 48, 48, 27,
		IC_CM_FT + IC_BC, 48, 48,  5, 1, 91, 1,
		IC_CM_FR + IC_FC, 44, 12,  9, 40,

	ICONS_Mute,
		IC_CM_CP + IC_FC, ICONS_LargVolCr, 0, 0,
		IC_CM_CP + IC_FC, ICONS_VolComm, 0, 0,
		IC_CM_CP + IC_FC, ICONS_Slash, 0, 0,

	ICONS_VolDown,
		IC_CM_CP + IC_FC, ICONS_VolComm, 0, 0,

	ICONS_VolUp,
		IC_CM_CP + IC_FC, ICONS_LargVolCr, 0, 0,
		IC_CM_CP + IC_FC, ICONS_VolComm, 0, 0,

	ICONS_PC,
		IC_CM_FRR+IC_FC,  8, 16, 81, 55,  8,
		IC_CM_FR +IC_BC, 17, 24, 63, 38,
		IC_CM_FR +IC_FC,  1, 72, 94,  9,

	ICONS_TV,
		IC_CM_FRR+IC_FC,  4, 12, 87, 63, 8,
		IC_CM_FR +IC_BC, 11, 20, 72, 48,
		IC_CM_FR +IC_FC, 32, 75, 32,  8,

	ICONS_Vinyl,
		IC_CM_FC +IC_FC, 47, 47, 42,
		IC_CM_FC +IC_BC, 47, 47, 16,
		IC_CM_FC +IC_FC, 47, 47,  8,
		IC_CM_FC +IC_BC, 47, 47,  3,

	ICONS_Cd,
		IC_CM_FC +IC_FC, 47, 47, 42,
		IC_CM_FC +IC_BC, 47, 47, 38,
		IC_CM_FC +IC_FC, 47, 47, 14,
		IC_CM_FC +IC_BC, 47, 47,  5,

	ICONS_Aux1,
		IC_CM_CP + IC_FC, ICONS_Aux_Cont, 0, 0,
		IC_CM_CP + IC_FC, ICONS_1, 5, -7,

	ICONS_Aux2,
		IC_CM_CP + IC_FC, ICONS_Aux_Cont, 0, 0,
		IC_CM_CP + IC_FC, ICONS_2, 7, -7,

	ICONS_Input,
		IC_CM_NO + IC_BC,

	ICONS_Trigger,
		IC_CM_NO + IC_BC,

	ICONS_Speaker,
		IC_CM_FRR+ IC_FC, 20,  8, 55, 79,  8,
		IC_CM_FC + IC_BC, 48, 24,  8,
		IC_CM_FC + IC_BC, 48, 59, 20,
		IC_CM_FC + IC_FC, 48, 59, 12,

	ICONS_Volume,
		IC_CM_NO + IC_BC,

	ICONS_Balance,
		IC_CM_FT + IC_FC, 24, 52, 24, 77, 47, 77,
		IC_CM_FT + IC_FC, 71, 52, 71, 77, 47, 77,

	ICONS_Offset,
		IC_CM_FT + IC_FC, 47, 52, 47, 77, 70, 77,
		IC_CM_FT + IC_FC, 47, 52, 47, 28, 70, 28,

	ICONS_1,
		IC_CM_FR + IC_FC, 40, 28, 15,  7,
		IC_CM_FR + IC_FC, 48, 35,  7, 32,

	ICONS_2,
		IC_CM_FR + IC_FC, 36, 28, 23,  7,
		IC_CM_FR + IC_FC, 52, 35,  7, 16,
		IC_CM_FR + IC_FC, 36, 44, 23,  7,
		IC_CM_FR + IC_FC, 36, 51,  7, 16,
		IC_CM_FR + IC_FC, 43, 60, 16,  7,
		IC_CM_FT + IC_BC, 36, 43, 40, 43, 36, 47,
		IC_CM_FT + IC_BC, 59, 27, 59, 32, 55, 27,
		IC_CM_FT + IC_BC, 59, 51, 55, 51, 59, 47,

	ICONS_3,
		IC_CM_NO + IC_BC,
	ICONS_4,
		IC_CM_NO + IC_BC,
	ICONS_5,
		IC_CM_NO + IC_BC,
	ICONS_6,
		IC_CM_NO + IC_BC,
	ICONS_7,
		IC_CM_NO + IC_BC,
	ICONS_8,
		IC_CM_NO + IC_BC,
	ICONS_9,
		IC_CM_NO + IC_BC,
	ICONS_0,
		IC_CM_NO + IC_BC,

	ICONS_Clear,
		IC_CM_NO + IC_BC,

	ICONS_Plus,
		IC_CM_FRR+ IC_FC,  8, 39, 80, 16, 8,
		IC_CM_FRR+ IC_FC, 39,  8, 16, 80, 8,

	ICONS_Minus,
		IC_CM_FRR+ IC_FC,  8, 39, 80, 16, 8,

	ICONS_Hourglass,
		IC_CM_FR + IC_FC, 24, 10, 47,  7,
		IC_CM_FR + IC_FC, 24, 17,  7, 14, // 13
		IC_CM_FR + IC_FC, 64, 17,  7, 14, // 13
		IC_CM_FR + IC_FC, 24, 67,  7, 14,
		IC_CM_FR + IC_FC, 64, 67,  7, 14,
		IC_CM_FR + IC_FC, 24, 78, 47,  7,
		IC_CM_FR2+ IC_FC, 24, 66, 30, 69, 71, 29,
		IC_CM_FR2+ IC_FC, 24, 29, 66, 69, 71, 66,

	ICONS_Presets,
		IC_CM_NO + IC_BC,

	ICONS_Lighting,
		IC_CM_FC + IC_FC, 24, 24, 3,
		IC_CM_FC + IC_FC, 24, 40, 3,
		IC_CM_FC + IC_FC, 24, 56, 3,
		IC_CM_FC + IC_FC, 24, 72, 3,

		IC_CM_FC + IC_FC, 40, 24, 3,
		IC_CM_FC + IC_FC, 56, 24, 3,

		IC_CM_FC + IC_FC, 40, 72, 3,
		IC_CM_FC + IC_FC, 56, 72, 3,

		IC_CM_FC + IC_FC, 72, 24, 3,
		IC_CM_FC + IC_FC, 72, 40, 3,
		IC_CM_FC + IC_FC, 72, 56, 3,
		IC_CM_FC + IC_FC, 72, 72, 3,

		IC_CM_FC + IC_FC, 40, 40, 5,
		IC_CM_FC + IC_FC, 56, 40, 5,
		IC_CM_FC + IC_FC, 40, 56, 5,
		IC_CM_FC + IC_FC, 56, 56, 5,

		IC_CM_FC + IC_FC, 12, 40, 1,
		IC_CM_FC + IC_FC, 12, 56, 1,
		IC_CM_FC + IC_FC, 84, 40, 1,
		IC_CM_FC + IC_FC, 84, 56, 1,

		IC_CM_FC + IC_FC, 40, 12, 1,
		IC_CM_FC + IC_FC, 56, 12, 1,
		IC_CM_FC + IC_FC, 40, 84, 1,
		IC_CM_FC + IC_FC, 56, 84, 1,

	ICONS_Up,
		IC_CM_FR2+ IC_FC, 24, 55, 29, 60, 52, 36,
		IC_CM_FR2+ IC_FC, 72, 55, 66, 60, 44, 36,

	ICONS_Down,
		IC_CM_FR2+ IC_FC, 29, 34, 23, 39, 48, 63,
		IC_CM_FR2+ IC_FC, 66, 34, 71, 39, 48, 63,

	ICONS_END,
		IC_CM_END
};

#endif /* ICON_H_ */
