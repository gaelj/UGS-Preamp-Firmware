/*
 * icons.cpp
 *
 *  Created on: 3 juin 2015
 *      Author: gael
 */


#include "iconspage.h"


Adafruit_RA8875 IconsPage::tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

Layout IconsPage::layout_standby;
Layout IconsPage::layout_default;
Layout IconsPage::layout_hourglass;
Layout IconsPage::layout_sources;
Layout IconsPage::layout_control;

uint16_t IconsPage::current_page;
Icon IconsPage::icons[MAX_ICONS];

int8_t  IconsPage::last_touched_icon = NONE;
int8_t  IconsPage::touched_icon = NONE;
int8_t  IconsPage::selected_icon = NONE;
int8_t  IconsPage::held_icon = NONE;

unsigned int IconsPage::prevVuValue[4] = { 0 };

unsigned long IconsPage::last_press;
unsigned long IconsPage::initial_press;

boolean IconsPage::current_tft_faded_in;


int IconsPage::begin()
{

	// define icon layouts
	// standby
	layout_standby = Layout("B", 8, 8, tft.width(), tft.height(), 3, 2, 16);

	// main
	layout_default = Layout("D", 8, 8, tft.width(), tft.height(), 3, 2, 16);

	// hourglass
	layout_hourglass = Layout("H", 256, 96, tft.width(), tft.height(), 3, 1, 16);

	// sources
	layout_sources = Layout("S", 8, 8, tft.width(), tft.height(), 3, 2, 16);
	layout_sources.y_min = 8 + layout_control.ss / 2;

	// ctrl
	layout_control = Layout("C", 8, 8, tft.width(), tft.height(), 3, 2, 16);
	layout_control.x_min = 8 + layout_control.ss * 3 / 2;
	layout_control.y_min = 8 + layout_control.ss / 2;

	// initialize icons
	Icon::E2pGetIcons();

	if (USE_DISPLAY == 0) return (0);

	if (!tft.begin(RA8875_800x480)) return (-1);

	tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	tft.PWM1config(true, RA8875_PWM_CLK_DIV8); // PWM output for backlight
	tft.PWM1out(0);

	pinMode(RA8875_INT, INPUT);
	digitalWrite(RA8875_INT, HIGH);
	tft.touchEnable(true);

	return (0);
}

/*
void IconsPage::ListColors()
{
	return;

	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t sz = 2;

	for (y = 0; y < 256*sz; y += sz)
		for (x = 0; x < 256*sz; x += sz)
			tft.fillRect(x, y, sz, sz, 256 * y + x);

	return;

	tft.fillRect(x, y, sz, sz, ALICEBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, ANTIQUEWHITE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, AQUA); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, AQUAMARINE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, AZURE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BEIGE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BISQUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BLACK); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BLANCHEDALMOND); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BLUEVIOLET); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BROWN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, BURLYWOOD); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CADETBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CHARTREUSE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CHOCOLATE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CORAL); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CORNFLOWERBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CORNSILK); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CRIMSON); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, CYAN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKCYAN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKGOLDENROD); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKGRAY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKKHAKI); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKMAGENTA); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKOLIVEGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKORANGE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKORCHID); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKRED); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKSALMON); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKSEAGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKSLATEBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKSLATEGRAY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKTURQUOISE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DARKVIOLET); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DEEPPINK); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DEEPSKYBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DIMGRAY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, DODGERBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, FIREBRICK); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, FLORALWHITE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, FORESTGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, FUCHSIA); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, GAINSBORO); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, GHOSTWHITE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, GOLD); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, GOLDENROD); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, GRAY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, GREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, GREENYELLOW); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, HONEYDEW); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, HOTPINK); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, INDIANRED); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, INDIGO); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, IVORY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, KHAKI); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LAVENDER); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LAVENDERBLUSH); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LAWNGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LEMONCHIFFON); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTCORAL); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTCYAN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTGOLDENRODYELLOW); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTGREY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTPINK); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTSALMON); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTSEAGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTSKYBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTSLATEGRAY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTSTEELBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIGHTYELLOW); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIME); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LIMEGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, LINEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MAGENTA); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MAROON); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMAQUAMARINE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMORCHID); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMPURPLE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMSEAGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMSLATEBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMSPRINGGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMTURQUOISE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MEDIUMVIOLETRED); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MIDNIGHTBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MINTCREAM); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MISTYROSE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, MOCCASIN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, NAVAJOWHITE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, NAVY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, OLDLACE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, OLIVE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, OLIVEDRAB); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, ORANGE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, ORANGERED); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, ORCHID); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PALEGOLDENROD); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PALEGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PALETURQUOISE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PALEVIOLETRED); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PAPAYAWHIP); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PEACHPUFF); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PERU); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PINK); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PLUM); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, POWDERBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, PURPLE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, RED); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, ROSYBROWN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, ROYALBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SADDLEBROWN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SALMON); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SANDYBROWN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SEAGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SEASHELL); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SIENNA); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SILVER); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SKYBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SLATEBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SLATEGRAY); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SNOW); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, SPRINGGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, STEELBLUE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, TAN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, TEAL); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, THISTLE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, TOMATO); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, TURQUOISE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, VIOLET); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, WHEAT); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, WHITE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, WHITESMOKE); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, YELLOW); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
	tft.fillRect(x, y, sz, sz, YELLOWGREEN); x += sz; if (x > tft.width()) { x = 0; y += sz;} if (y > tft.height()) return;
}
*/




void IconsPage::DisplayDacStatus()
{
	String str;
	str = Settings::GetDacStatus();
	Ser::SerialPrint(_CMD_DACSTATUS, str);

	if (USE_DISPLAY && current_page == PAGE_MAIN) {

		tft.fillRect(240, 240, 400, 80, Settings::backColor);

		if (Preamp::DacIsDown) return;

		// wake up display
		IconsPage::SimulateScreenTouch();

		tft.textMode();

		tft.textEnlarge(3);
		tft.setTextColor(Settings::frontColor, Settings::backColor);
		tft.textTransparent(Settings::frontColor);

		tft.textEnlarge(1);
		tft.textSetCursor(240, 240);

		tft.print(str);
		tft.textEnlarge(1);
		tft.graphicsMode();
	}
}

void IconsPage::DisplayText(String Text)
{
	if (current_page != PAGE_MAIN) return;

	tft.fillRect(0, 0, 80, 80, Settings::backColor);

	// wake up display
	IconsPage::SimulateScreenTouch();

	tft.textMode();

	tft.setTextColor(Settings::frontColor, Settings::backColor);
	tft.textTransparent(Settings::frontColor);

	tft.textEnlarge(1);
	tft.textSetCursor(40, 40);
	tft.print(Text);
	// tft.textEnlarge(1);
	tft.graphicsMode();

	Serial.print(F("DisplayText "));
	Serial.println(Text);
}

void IconsPage::ShowPage(int page_num)
{
	// SerialPrint("REM:PAGE SET", PageName(page_num));
	if (USE_DISPLAY == 0) return;

	/*
	Serial.println("##########################################");
	Serial.print("Show page #");
	Serial.println(page_num);
	*/

	FadeOut(false);

	current_page = page_num;

	last_touched_icon = NONE;
	touched_icon = NONE;
	selected_icon = NONE;

	// clear icon positions
	for (int i = 0; i < MAX_ICONS; i++) {
		icons[i] = Icon();
	}

	switch(page_num)
	{/*
	case PAGE_ALL:
		// initialize variables
		Serial.println("PAGE_ALL");

		// show the first 15 icons in the collection by order of definition
		for (int i = 0; i < 15; i++) {
			icons[i] = Icon(layout_default, i, x, y, tft, false, false);

			// arrange icon position horizontally
			x += 1;
			if (x > 5)) {
				y += 1;
				x = 0;
			}
		}
		break;
	 */

	case PAGE_STANDBY:
		// SerialPrint("PAGE", "PAGE_STANDBY");
		FillScreenGradient(Settings::backColor, Settings::backColor2);
		// icons[ICONS_Standby] = Icon(layout_standby, ICONS_Standby, 0, 2, tft, false, false, false);
		icons[ICONS_Standby] = Icon(layout_hourglass, ICONS_Standby, 0, 0, tft, false, false, false);
		break;


	case PAGE_MAIN:
		// initialize variables
		// Preamp::SerialPrint("PAGE", "PAGE_MAIN");

		FillScreenGradient(Settings::backColor, Settings::backColor2);

		/*
		Serial.print("Settings::current_input: ");
		Serial.println(Settings::current_input);
		*/

		int16_t cur_inp_icon;
		cur_inp_icon = Settings::input_icons[Settings::current_input];

		/*
		Serial.print("cur_inp_icon: ");
		Serial.println(cur_inp_icon);
		 */
		icons[ICONS_VolDown] = 	Icon(layout_default, ICONS_VolDown,	1, 0, tft, false, false, false);
		icons[ICONS_Speaker] = 	Icon(layout_default, ICONS_Speaker,	2, 0, tft, false, false, true);
		icons[ICONS_VolUp] = 	Icon(layout_default, ICONS_VolUp,	3, 0, tft, false, false, false);

		icons[ICONS_Mute] = 	Icon(layout_default, ICONS_Mute, 	0, 1, tft, Settings::Mute, false, false);

		if (Settings::UseInput[0]+Settings::UseInput[1]+Settings::UseInput[2]+Settings::UseInput[3]+Settings::UseInput[4]+Settings::UseInput[5] > 1)
			if (Settings::UseInput[Settings::current_input] == 1)
				icons[cur_inp_icon] = 	Icon(layout_default, cur_inp_icon, 	4, 1, tft, false, false, false);

		icons[ICONS_Standby] = 	Icon(layout_default, ICONS_Standby,	0, 2, tft, false, false, false);
		icons[ICONS_Tools] = 	Icon(layout_default, ICONS_Tools, 	4, 2, tft, false, false, false);

		DrawVolumeBar(1, Settings::Volume(), 0, 0, _MUSES_VolumeSteps);

		// ListColors();

		tft.textMode();

		/*
		tft.textSetCursor(uint16_t x, uint16_t y);
		tft.textColor(uint16_t foreColor, uint16_t bgColor);
		tft.textTransparent(uint16_t foreColor);
		tft.textEnlarge(uint8_t scale);
		tft.textWrite(const char* buffer, uint16_t len=0);
		*/

		tft.setTextColor(Settings::frontColor, Settings::backColor);
		tft.textTransparent(Settings::frontColor);

		tft.textEnlarge(1);
		tft.textSetCursor(250, 320);
		tft.textWrite("UNIVERSAL GAIN STAGE");

		tft.textSetCursor(250, 360);
		tft.textWrite("PRESET #");

		char gn[1];
		gn[0] = String(Settings::CurrentPresetGroup.groupNumber).charAt(0);

		tft.textSetCursor(380, 360);
		tft.textWrite(gn, 1);

		tft.graphicsMode();

		IconsPage::DisplayDacStatus();

		break;

	case PAGE_SOURCES:
		// initialize variables
		// SerialPrint("PAGE", "PAGE_SOURCES");
		FillScreenGradient(Settings::backColor, Settings::backColor2);

		int x; x = 1;
		int y; y = 0;

		for(int i = 0; i < MAX_INPUTS; i++)
		{
			if (Settings::UseInput[i] == 0)
				continue;

			icons[Settings::input_icons[i]] =
					Icon(layout_sources, Settings::input_icons[i], x, y, tft, (Settings::current_input == i), false, false);
			x++;
			if (x > 3) // 3 icons per line
			{
				x = 1;
				y++;
			}
		}

		break;

	case PAGE_CTRL:
		// initialize variables
		// SerialPrint("PAGE", "PAGE_CTRL");
		FillScreenGradient(Settings::backColor, Settings::backColor2);

		icons[ICONS_Symmout] = 	Icon(layout_control, ICONS_Symmout,	0, 0, tft, Settings::OutAssym, false, false);
		icons[ICONS_Bypass] = 	Icon(layout_control, ICONS_Bypass,	1, 0, tft, Settings::Bypass, false, false);
		icons[ICONS_Balance] = 	Icon(layout_control, ICONS_Balance,	0, 1, tft, false, false, false);
		icons[ICONS_Offset] = 	Icon(layout_control, ICONS_Offset,	1, 1, tft, false, false, false);

		/* drawIcon(ICONS_Sleep,  layout_control, 2, 0);
		drawIcon(ICONS_PC,        layout_control, 1, 1);
		drawIcon(ICONS_TV,        layout_control, 2, 1);
		drawIcon(ICONS_Vinyl,     layout_control, 3, 1);

		drawIcon(ICONS_Cd,        layout_control, 1, 2);
		drawIcon(ICONS_Aux1,      layout_control, 2, 2);
		drawIcon(ICONS_Aux2,      layout_control, 3, 2);
		*/

		break;
	}

	FadeIn();
}

void IconsPage::FillScreenGradient(uint16_t Backcolor, uint16_t Backcolor2)
{
	if (USE_DISPLAY == 0) return;

	if (Backcolor != Backcolor2) {

		uint16_t blue =  (Backcolor & 0x1F);           // 5
		uint16_t green = ((Backcolor >> 5) & 0x3F);    // 6
		uint16_t red =   ((Backcolor >> 11) & 0x1F);   // 5

		uint16_t blue2 =  (Backcolor2 & 0x1F);           // 5
		uint16_t green2 = ((Backcolor2 >> 5) & 0x3F);    // 6
		uint16_t red2 =   ((Backcolor2 >> 11) & 0x1F);   // 5

		float changeB = ((float)blue2 - (float)blue)  / (float)tft.height();
		float changeG = ((float)green2 - (float)green) / (float)tft.height();
		float changeR = ((float)red2 - (float)red)  / (float)tft.height();

		uint16_t color = Backcolor;

		for (uint16_t y = 0; y < tft.height(); y++) {

			tft.drawFastHLine(0, y, tft.width(), color);

			float bl = (float)blue  + ((float)y * changeB);
			float gr = (float)green + ((float)y * changeG);
			float re = (float)red   + ((float)y * changeR);

			int b = ((uint16_t)bl & 0x1F);
			int g = ((uint16_t)gr & 0x3F) << 5;
			int r = ((uint16_t)re & 0x1F) << 11;

			color = (r + g + b);
		}
	}
	else
		tft.fillScreen(Backcolor);
}


String IconsPage::PageName(int page)
{
	String pg = "";
	switch(page)
	{
		case 0: pg = "PAGE_ALL"; break;
		case 1: pg = "PAGE_MAIN"; break;
		case 2: pg = "PAGE_SOURCES"; break;
		case 3: pg = "PAGE_CTRL"; break;
		case 4: pg = "PAGE_STANDBY"; break;
	}

	return (pg);
}

void IconsPage::ProcessTouchEvents()
{
	//Serial.println("Process touch events");
	if (USE_DISPLAY == 0) return;

	if (! digitalRead(RA8875_INT))
	{
		if (tft.touched())
		{
			// Serial.println("Touch detected");

			// get touched coordinates
			uint16_t tx, ty; // raw touch coordinates
			tft.touchRead(&tx, &ty);

			float xScale = 1024.0F/tft.width();
			float yScale = 1024.0F/tft.height();

			uint16_t tch_x = (uint16_t)(tx/xScale);
			uint16_t tch_y = (uint16_t)(ty/yScale);

			DoPushedIconAction(tch_x, tch_y);

			if (held_icon > NONE) {
				DoHeldIconAction();
				Settings::CurrentPresetGroup.WriteSettingsToEeprom();
			}
		}
	}
	else
	{
		FingerWasLifted();
	}

	// do not turn off display if timeout is 0
	if (Settings::tft_sleep_timeout > 0) {

		if (millis() > (last_press + (Settings::tft_sleep_timeout * 1000))) {
			if (current_tft_faded_in)
				FadeOut();
		} else {
			if (! current_tft_faded_in)
				FadeIn();
		}

	}
}

void IconsPage::FingerWasLifted()
{
	// last_press = millis();

	if (last_touched_icon != NONE) {

		// Serial.print("Setting initial press (finger lifted) ");
		// Serial.println(last_touched_icon);
		initial_press = millis();
		last_touched_icon = NONE;
	}

	if (touched_icon != NONE && millis() > initial_press + Settings::touch_timeout) {
		// finger is lifted for long enough, confirm it

		// Serial.println("finger is lifted (confirmed)");

		DoTouchedIconAction();
		Settings::CurrentPresetGroup.WriteSettingsToEeprom();

		// reset any previously active icon
		if (touched_icon > NONE) {
			// Serial.print("touched_icon reset (finger lifted): ");
			// Serial.println(touched_icon);

			icons[touched_icon].Draw(tft, icons[touched_icon].isSelected, false, icons[touched_icon].isInactive);
		}

		touched_icon = NONE;
	}
}


void IconsPage::DoHeldIconAction()
{
	if (held_icon > NONE) {
		// an icon is being pressed and held
		// Serial.println("Held touch detected");

		switch(current_page)
		{
			case PAGE_MAIN:
				switch (held_icon)
				{
					case ICONS_VolDown:
						Ugs::DecrementVolume();
						break;

					case ICONS_VolUp:
						Ugs::IncrementVolume();
						break;
				}
		}
		held_icon = NONE;

		delay(50);
	}
}

void IconsPage::DoTouchedIconAction()
{

	if (touched_icon != NONE) { // if (millis() > last_press + touch_press_timeout) {
		// long enough "continuous" press detected. The icon is selected

		/*
		Serial.print("action ");
		Serial.println(touched_icon);
		*/

		switch(current_page)
		{/*
			case PAGE_ALL:

				if (active_icon > NONE) {
					// draw new icon with green background
					if (selected_icon != active_icon)
						drawIcon(active_icon, icon_pos_x[active_icon], icon_pos_y[active_icon], RA8875_BLACK, RA8875_GREEN);

					// clear previous selected icon
					if (selected_icon > NONE)
						drawIcon(selected_icon, icon_pos_x[selected_icon], icon_pos_y[selected_icon], RA8875_BLACK, RA8875_WHITE);

					if (selected_icon == active_icon)
						// un-select if already selected
						selected_icon = NONE;
					else
						// select
						selected_icon = active_icon;
				}
				break;
		 	 */
			case PAGE_STANDBY:
				switch (touched_icon)
				{
					case ICONS_Standby:
						Ugs::StartToMainMenu();
					break;
				}

				break;

			case PAGE_MAIN:
				switch (touched_icon)
				{
					case ICONS_Standby:
						Ugs::SetToStandby();
						break;

					case ICONS_Tools:
						ShowPage(PAGE_CTRL);
						break;

					case ICONS_Mute:
						if (Settings::Mute) {

							// un-select if already selected
							//selected_icon = NONE;
							Preamp::SetMute(0);

						} else {

							// select
							//selected_icon = ICONS_Mute;
							Preamp::SetMute(1);

						}

						break;

					case ICONS_VolDown:
						Ugs::DecrementVolume();
						break;

					case ICONS_VolUp:
						Ugs::IncrementVolume();
						break;

					case ICONS_Speaker:
						icons[ICONS_Speaker].Draw(tft, false, false, true);
						break;

					case EMPTYSPACE: // pressed inactive space
						break;

					default:
						if (touched_icon == Settings::input_icons[Settings::current_input])
						{
							ShowPage(PAGE_SOURCES);
						}
				}
				break;

			case PAGE_SOURCES:
				switch (touched_icon)
				{
					case EMPTYSPACE: // pressed inactive space
						ShowPage(PAGE_MAIN);
						break;
					default:
						int16_t selinput = 0;
						for(int i = 0; i < MAX_INPUTS; i++) {
							if (Settings::input_icons[i] == touched_icon) {
								selinput = i;
								break;
							}
						}
						Ugs::SetInput(selinput);
						ShowPage(PAGE_MAIN);
						break;
				}
				break;


			case PAGE_CTRL:
				switch (touched_icon)
				{
					case EMPTYSPACE: // pressed inactive space
						ShowPage(PAGE_MAIN);
						break;
					case ICONS_Symmout:
						Preamp::SetOutAsym(!Settings::OutAssym);
						icons[ICONS_Symmout].Draw(tft, Settings::OutAssym, false, false);
						break;
					case ICONS_Bypass:
						Preamp::SetBypass(!Settings::Bypass);
						icons[ICONS_Bypass].Draw(tft, Settings::Bypass, false, false);
						break;
					case ICONS_Balance:
						break;
					case ICONS_Offset:
						break;
					default:
						break;
				}
		}
	}
}

void IconsPage::DoPushedIconAction(uint16_t tch_x, uint16_t tch_y)
{

	bool icon_found = false;

	for(int i = 0; i < MAX_ICONS; i++)
	{
		if (icons[i].IconIsTouched(tch_x, tch_y)) {

			// Serial.print("Toggle Icon Selection: ");
			// Serial.println(i);

			// an icon is under the pressed point

			last_press = millis();

			if (last_touched_icon != i) {

				// Serial.print("Setting initial press (toggle icon) ");
				// Serial.println(last_touched_icon);

				initial_press = millis();
				last_touched_icon = i;
			}


			if (millis() > initial_press + Settings::hold_timeout) {
				// an icon is being pressed & held
				held_icon = i;

				// Serial.print("press and hold ");
				// Serial.println(held_icon);

			} else if (touched_icon != i && millis() > initial_press + Settings::touch_timeout
					&& i != ICONS_Speaker && i != ICONS_Hourglass) {
				// new active icon is pressed long enough, confirm it

				if (touched_icon > NONE) {
					// Serial.print("touched_icon reset (empty space): ");
					// Serial.println(touched_icon);
					icons[touched_icon].Draw(tft, icons[touched_icon].isSelected, false, icons[touched_icon].isInactive);
				}

				touched_icon = i;
				held_icon = NONE;

				// Serial.print("pressed ");
				// Serial.println(touched_icon);

				// draw the active icon in red
				icons[touched_icon].Draw(tft, icons[touched_icon].isSelected, true, icons[touched_icon].isInactive);
			}


			icon_found = true;
			break;
		}
	}

	if (! icon_found)
	{
		// Check for volume bar scrolling

		uint16_t width =  _MUSES_VolumeSteps - 0 + 1;
		uint16_t x0 = (tft.width() - width) / 2;
		uint16_t y0 = VOL_BAR_Y_POS;

		if (Icon::ObjectIsTouched(tch_x, tch_y, x0, y0, width, VOL_BAR_HEIGHT)) {

			last_press = millis();
			initial_press = millis();

			Ugs::SetVolume((tch_x - x0));
		}

		// pressing in empty space
		last_press = millis();
		held_icon = NONE;

		if (last_touched_icon != EMPTYSPACE) {

			//Serial.print("Setting initial press (empty space) ");
			//Serial.println(last_touched_icon);
			initial_press = millis();
			last_touched_icon = EMPTYSPACE;
		}

		if (touched_icon != EMPTYSPACE && millis() > initial_press + Settings::touch_timeout) {
			// empty space is pressed long enough, confirm it

			// Serial.println("Confirming touch empty space");


			// reset any previously active icon
			if (touched_icon > NONE) {
				// Serial.print("touched_icon reset (empty space): ");
				// Serial.println(touched_icon);
				icons[touched_icon].Draw(tft, icons[touched_icon].isSelected, false, icons[touched_icon].isInactive);
			}

			touched_icon = EMPTYSPACE;
		}
	}
}

void IconsPage::DrawHorizontalBar(int x0, int y0, bool drawContour, int16_t value, int16_t prevValue, int16_t min, int16_t max, int16_t height, uint16_t frontColor)
{
	if (USE_DISPLAY == 0) return;

	int16_t width = max - min + 1;

	// background
	if (value < prevValue)
		tft.fillRect(x0 + value, y0, prevValue - value , height, Settings::backColor);

	// volume bar
	if (value > prevValue)
		tft.fillRect(x0 + prevValue, y0, value - prevValue, height, frontColor);

	// countour
	if (drawContour)
		tft.drawRoundRect(x0 - 4, y0 - 5, width + 8, height + 11, 8, Settings::frontColor);
}

void IconsPage::DrawVolumeBar(bool drawContour, int16_t volume, int16_t prevVolume, int16_t min, int16_t max)
{
	if (current_page == PAGE_STANDBY) return;

	int16_t width = max - min + 1;
	int x0 = (tft.width() - width) / 2;
	int y0 = VOL_BAR_Y_POS;

	IconsPage::DrawHorizontalBar(x0, y0, drawContour, volume, prevVolume, min, max, VOL_BAR_HEIGHT, Settings::frontColor);
}

void IconsPage::DisplayVuMeter(int channel, int value)
{
	if (USE_DISPLAY == 0) return;
	if (current_page != PAGE_MAIN) return;


	// int16_t min = 0;
	// int16_t max = 524;

	// int16_t width = max - min + 1;


	// IconsPage::DrawHorizontalBar(x0, y0, true, value, prevVuValue[channel], min, max, VOL_BAR_HEIGHT, Settings::frontColor);
	prevVuValue[channel] = value;

	tft.fillRect(240 + channel * 120, 400, 120, 80, Settings::backColor);
	tft.textMode();

	tft.setTextColor(Settings::frontColor, Settings::backColor);
	tft.textTransparent(Settings::frontColor);

	tft.textEnlarge(3);
	tft.textSetCursor(240 + channel * 120, 400);

	String str = String(value);

	tft.print(str);

	tft.textEnlarge(1);
	tft.graphicsMode();
}


void IconsPage::DrawHourglass()
{
	if (USE_DISPLAY == 0) return;
	if (current_page == PAGE_HOURGLASS) return;
	// FillScreenGradient(Settings::backColor, Settings::backColor2);
	FillScreenGradient(Settings::backColor, Settings::backColor);
	icons[ICONS_Hourglass] = Icon(layout_hourglass, ICONS_Hourglass, 0, 0, tft, false, false, true);
	current_page = PAGE_HOURGLASS;
}


void IconsPage::SimulateScreenTouch()
{
	last_press = millis();
}

// turn on the display and do a PWM sweep
void IconsPage::FadeIn()
{
	if (USE_DISPLAY == 0) return;
	tft.displayOn(true);
	int16_t i = Settings::Illumination_TFT;
	while (i < Settings::Illumination_TFT_H)
	{
		if (i < 60) i += 2;
		else if (i > 180) i += 2;
		else i++;

		Preamp::SetIllumination_TFT(i, false);
		delay(Settings::TftSweepSpeed);
	}
	//tft.PWM1out(255);
	//Preamp::SetIllumination_TFT(255);
	Preamp::SetIllumination_TFT(Settings::Illumination_TFT_H);
	current_tft_faded_in = true;

}

// do a PWM sweep and turn off the display
void IconsPage::FadeOut(bool log)
{
	if (USE_DISPLAY == 0) return;
	int16_t i = Settings::Illumination_TFT;
	while (i > Settings::Illumination_TFT_L)
	{
		if (i < 60) i -= 2;
		else if (i > 180) i -= 2;
		else i--;

		Preamp::SetIllumination_TFT(i, false);
		delay(Settings::TftSweepSpeed);
	}
	//tft.PWM1out(0);
	//tft.displayOn(false);
	//Preamp::SetIllumination_TFT(0);
	Preamp::SetIllumination_TFT(Settings::Illumination_TFT_L);
	current_tft_faded_in = false;
}




