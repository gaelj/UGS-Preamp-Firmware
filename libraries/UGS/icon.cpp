/*
 * icon.cpp
 *
 *  Created on: 16 août 2015
 *      Author: gael
 */

#include "icon.h"


uint16_t Icon::CurrentIconWriteAddress = _e2p_IconStoreAddress;
uint16_t Icon::IconAddresses[MAX_ICONS + 1] = { 0 };
byte Icon::IconDefCache[_e2p_MaxIconBytes] = {0};



Icon::Icon()
{
	icon = -1;
	positionX = -1;
	positionY = -1;

	layout = Layout();

	icon_pos_x = -1;
	icon_pos_y = -1;

	isSelected = 0;
	isTouched = 0;
	isInactive = 0;
}

Icon::Icon(Layout _layout, int8_t _icon, int16_t _i, int16_t _j, Adafruit_RA8875 tft,
		bool selected, bool touched, bool inactive)
{
	icon = _icon;
	positionX = _i;
	positionY = _j;

	layout = _layout;

	icon_pos_x = _layout.x_min + positionX * _layout.ss;
	icon_pos_y = _layout.y_min + positionY * _layout.ss;

	// init inverted so as to force a first draw
	isSelected = !selected;
	isTouched = !touched;
	isInactive = !inactive;

	Draw(tft, selected, touched, inactive);
}



bool Icon::ObjectIsTouched(uint16_t tch_x, uint16_t tch_y, uint16_t pos_x, uint16_t pos_y, uint16_t size_x, uint16_t size_y)
{
	return (   tch_x >= pos_x && tch_x <= (pos_x + size_x)
			&& tch_y >= pos_y && tch_y <= (pos_y + size_y));
}

bool Icon::IconIsTouched(uint16_t tch_x, uint16_t tch_y)
{
	return (Icon::ObjectIsTouched(tch_x, tch_y, icon_pos_x, icon_pos_y, layout.icon_size, layout.icon_size));
}

void Icon::Draw(Adafruit_RA8875 tft, bool selected, bool touched, bool inactive)
{
	uint16_t fc;
	uint16_t bc;

	// ignore redundant redraws
	if (inactive && isInactive) return;
	if (touched && isTouched) return;
	if (selected == isSelected && ! touched && ! isTouched && ! inactive && ! isInactive) return;

	if (inactive) {
		fc = Settings::inactiveColor;
	} else if (touched) {
		fc = Settings::touchFrontColor;
	} else if (selected) {
		fc = Settings::selFrontColor;
	} else {
		fc = Settings::frontColor;
	}

	if (icon == ICONS_VolUp && Preamp::MaxVolumeReached)
		fc = Settings::selFrontColor;

	bc = Settings::backColor;

	isSelected = selected;
	isTouched = touched;
	isInactive = inactive;

	// DrawColoredIcon(tft, fc, bc);

	int16_t x = layout.x_min + positionX * layout.ss;
	int16_t y = layout.y_min + positionY * layout.ss;
	FillRectGradient(tft, x, y, layout.icon_size, layout.icon_size, bc);  // background

	E2pDrawIcon(tft, fc, bc, icon);
}

/*
void Icon::DrawColoredIcon(Adafruit_RA8875 tft, uint16_t fc, uint16_t bc)
{
	if (icon < 0 || icon > MAX_ICONS) {
		Serial.print("Error: trying to print icon #");
		Serial.println(this->icon);
		return;
	}
	*/
	/*
	Serial.print("printing icon: ");
	Serial.print(icon);
	Serial.print(" forecolor: ");
	Serial.println(fc);
 	 */
	/*
	uint16_t x = layout.x_min + positionX * layout.ss;
	uint16_t y = layout.y_min + positionY * layout.ss;

	// store the icon position for touch recognition
	icon_pos_x = x;
	icon_pos_y = y;

	uint16_t m = layout.size_m;
	uint16_t d = layout.size_d;

	int offset_x = 0;
	int offset_y = 0;

	FillRectGradient(tft, x, y, layout.icon_size, layout.icon_size, bc);  // background

	// tft.drawRect(x, y, icon_size, icon_size, RA8875_RED); // red container rectangle

	// Serial.print("x: "); Serial.print(x); Serial.print(" y: "); Serial.println(y);
	// Serial.print("icon size: "); Serial.print(icon_size); Serial.print(" sep: "); Serial.println(sep);
	// Serial.println();


	switch (icon) {
	case ICONS_Aux1      :
	case ICONS_Aux2      :
		// draw the common container
		tft.fillRoundRect(x +  4*m/d,  y + 12*m/d,  75*m/d, 75*m/d, 8, fc); //  4 12  75 91
		FillRectGradient(tft, x + 12*m/d,  y + 20*m/d,  63*m/d, 59*m/d, bc);     // 11 20  75 84
		FillRectGradient(tft, x + 71*m/d,  y + 12*m/d,   8*m/d, 79*m/d, bc);     // 11 20  75 84
		FillRectGradient(tft, x +  4*m/d,  y + 12*m/d,  71*m/d,  8*m/d, bc);     //  4 12  75 20


		tft.fillRoundRect( x + 20*m/d, y +  4*m/d, 71*m/d, 71*m/d, 8, fc);  // square countour    20  4     91  75

		FillRectGradient(tft, x + 28*m/d,  y + 12*m/d, 55*m/d, 55*m/d, bc);  // square cutout

		// offset the contained value
		offset_x =  8;
		offset_y = -8;

		break;
	}





	switch (icon) {

	case ICONS_Sleep     :
		tft.fillCircle  (x + 48*m/d, y + 52*m/d, 36*m/d,                           fc); // circle contour
		tft.fillCircle  (x + 48*m/d, y + 52*m/d, 27*m/d,                           bc);  // inner circle background


    //  tft.fillRect    (x + 44*m/d, y + 32*m/d, 5*m/d,     23*m/d,                frontColor); // big hand

    //  fillRect2(x + 44*m/d, y + 55*m/d,
    //            x + 62*m/d, y + 65*m/d,
    //            x + 64*m/d, y + 62*m/d, frontColor);                  // small hand


		fillRect2(tft,
				x +  8*m/d, y + 23*m/d,
				x + 13*m/d, y + 28*m/d,
				x + 30*m/d, y + 13*m/d, fc);                  // left ringer

		fillRect2(tft,
				x + 65*m/d, y + 13*m/d,
				x + 82*m/d, y + 28*m/d,
				x + 87*m/d, y + 23*m/d, fc);                  // right ringer

		tft.fillRect(x + 36*m/d,  y + 36*m/d, 23*m/d, 8*m/d, fc);  // Z top
		tft.fillRect(x + 36*m/d,  y + 60*m/d, 23*m/d, 8*m/d, fc);  // Z bottom

		fillRect2(tft,
				x + 36*m/d, y + 60*m/d,
				x + 45*m/d, y + 59*m/d,
				x + 59*m/d, y + 43*m/d, fc);                  // Z slash
		break;



	case ICONS_Mute      :
	case ICONS_VolDown   :
	case ICONS_VolUp	 :

		if (icon == ICONS_Mute || icon == ICONS_VolUp) {
			tft.fillCircle  (x + 54*m/d, y + 47*m/d, 29*m/d,      fc); // large circle contour
			tft.fillCircle  (x + 54*m/d, y + 47*m/d, 22*m/d,      bc);  // large circle cutout
		}
		tft.fillCircle  (x + 54*m/d, y + 47*m/d, 15*m/d,      fc); // small circle contour
		tft.fillCircle  (x + 54*m/d, y + 47*m/d, 8*m/d,       bc);  // small circle cutout

		FillRectGradient(tft, x + 10*m/d,  y + 10*m/d, 45*m/d, 74*m/d, bc);  // clear left half of circles

		tft.fillTriangle( x + 47*m/d, y + 16*m/d,
				x + 47*m/d, y + 78*m/d,
				x + 17*m/d, y + 47*m/d, fc); // speaker cone

		tft.fillRect(x + 12*m/d,  y + 36*m/d, 18*m/d, 22*m/d, fc);  // speaker magnet

		if (icon == ICONS_Mute) {
			fillRect2(tft,
					x + 11*m/d, y + 17*m/d,
					x + 78*m/d, y + 83*m/d,
					x + 83*m/d, y + 78*m/d, fc);    // right slash

			fillRect2(tft,
					x + 17*m/d, y + 11*m/d,
					x + 84*m/d, y + 78*m/d,
					x + 89*m/d, y + 73*m/d, bc);    // right slash cutout
		}
		break;



	case ICONS_Input     :

		tft.fillCircle  (x + 48*m/d, y + 48*m/d, 42*m/d,     fc); // circle contour
		tft.fillCircle  (x + 48*m/d, y + 48*m/d, 34*m/d,     bc);  // inner circle background

		tft.fillCircle  (x + 27*m/d, y + 46*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 68*m/d, y + 46*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 35*m/d, y + 65*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 60*m/d, y + 65*m/d, 5*m/d,      fc); // circle contour

		tft.fillCircle  (x + 40*m/d, y + 27*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 55*m/d, y + 27*m/d, 5*m/d,      fc); // circle contour
		tft.fillRect    (x + 40*m/d, y + 22*m/d, 15*m/d, 10*m/d, fc);  // link
		break;

	case ICONS_Trigger   :

		tft.fillRoundRect    (x +  4*m/d, y + 12*m/d, 87*m/d, 71*m/d, 8, fc);
		FillRectGradient(tft, x + 12*m/d, y + 20*m/d, 71*m/d, 55*m/d, bc);

		FillRectGradient(tft, x +  4*m/d, y + 36*m/d,  8*m/d, 23*m/d, bc);
		tft.fillRect    (x +  4*m/d, y + 44*m/d, 40*m/d,  7*m/d, fc);

		tft.fillTriangle( x + 44*m/d, y + 33*m/d,
				x + 44*m/d, y + 62*m/d,
				x + 59*m/d, y + 47*m/d, fc);

*/




// draw a rectangle at any angle by defining 3 of its points
void Icon::fillRect2(Adafruit_RA8875 tft, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	uint16_t x4 = x0 - x1 + x2;
	uint16_t y4 = y0 - y1 + y2;
	tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
	tft.fillTriangle(x0, y0, x4, y4, x2, y2, color);
}

void Icon::FillRectGradient(Adafruit_RA8875 tft, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t Backcolor)
{
	/*
	uint16_t blue =  (Backcolor & 0x1F);           // 5
	uint16_t green = ((Backcolor >> 5) & 0x3F);    // 6
    uint16_t red =   ((Backcolor >> 11) & 0x1F);   // 5

    float changeB = (float)blue  / (float)tft.height();
    float changeG = (float)green / (float)tft.height();
    float changeR = (float)red   / (float)tft.height();

	for (int p = y; p < y + h; p++) {

		tft.drawFastHLine(x, p, w, Backcolor);

		uint16_t bl = blue  - (p * changeB);
		uint16_t gr = green - (p * changeG);
		uint16_t re = red   - (p * changeR);

        int b = (bl & 0x1F);
        int g = (gr & 0x3F) << 5;
        int r = (re & 0x1F) << 11;

        Backcolor = (r + g + b);
	}
	*/
	tft.fillRect(x, y, w, h, Backcolor);
}




bool Icon::E2pAppendIconData(int16_t size, byte data[])
{
	bool ret;

	ret = EepromUtil::eeprom_write_bytes(Icon::CurrentIconWriteAddress, data, size);
	Icon::CurrentIconWriteAddress += size;
	return (ret);
}


void Icon::E2pGetIcons()
{
	uint16_t readAddress = _e2p_IconStoreAddress;
	byte readValue;

	do {
		readValue = EEPROM.read(readAddress);
		IconDefCache[readAddress - _e2p_IconStoreAddress] = readValue;

		//Serial.println("IDC  " + String(readAddress - _e2p_IconStoreAddress) + ": "+ String(readValue) + " ");

		if (readValue <= ICONS_END) {

			/*
			if (readValue > 0)
				Serial.println(" len: " + String(readAddress - IconAddresses[readValue - 1]));
			Serial.print("New icon: " + String(readValue) + " at " + String(readAddress));
			*/

			// ICONS_END is a special ID for the last icon
			IconAddresses[(readValue<MAX_ICONS)?readValue:MAX_ICONS] = readAddress;

		} else {

			if (readValue == 0xFF) // end byte (when used as command byte)
				break;

			if ((readValue & 0x80) == 0x80) {

				// Serial.print("Cmd " + String(readValue & 0x3F) + ", col " + ((readValue & 0x40)==0x40?"FC":"BC") + " len ");

				// it is a command byte, skip the data bytes for now

				uint16_t prevAddr = readAddress;

				switch(readValue & 0x3F) {
					case IC_CM_FRR: readAddress += 5; break;
					case IC_CM_FR : readAddress += 4; break;
					case IC_CM_FR2: readAddress += 6; break;
					case IC_CM_FT : readAddress += 6; break;
					case IC_CM_FC : readAddress += 3; break;
					case IC_CM_FCV: readAddress += 5; break;
					case IC_CM_CP:  readAddress += 3; break;
					case IC_CM_NO: break;
					default:
						Serial.print(F("!! Undf g cmd: "));
						Serial.println(String(readValue & 0x3F));
				}

				for (uint16_t i = 1; i <= (readAddress - prevAddr); i++) {
					readValue = EEPROM.read(prevAddr + i);
					IconDefCache[prevAddr - _e2p_IconStoreAddress + i] = readValue;
					//Serial.println("_IDC " + String(prevAddr - _e2p_IconStoreAddress + i) + ": "+ String(readValue) + " ");
				}
			}
			else {
				Serial.print(F("!! Undf val: "));
				Serial.println(String(readValue));
			}
		}

		readAddress++;
	}
	while (readAddress < _e2p_IconStoreAddress + _e2p_MaxIconBytes); // or break condition when FF is found as command byte


	/*
	for(int i = 0; i < MAX_ICONS + 1; i++) {
		Serial.println("icon st adr: " + String(i)+ " " + String(Icon::IconAddresses[i]));
	}
 	*/
}

void Icon::E2pCreateIcons(int16_t size, byte data[])
{

	CurrentIconWriteAddress = _e2p_IconStoreAddress;

	E2pAppendIconData(size, data); // _NbDefaultIconBytes

	for(int16_t i = 0; i < size; i++)
		IconDefCache[i] = data[i];

	Serial.print(F("Written icons in e2p. IcSz: "));
	Serial.println(size);


/*


	case ICONS_Sleep     :
		tft.fillCircle  (x + 48*m/d, y + 52*m/d, 36*m/d,                           fc); // circle contour
		tft.fillCircle  (x + 48*m/d, y + 52*m/d, 27*m/d,                           bc);  // inner circle background


    //  tft.fillRect    (x + 44*m/d, y + 32*m/d, 5*m/d,     23*m/d,                frontColor); // big hand

    //  fillRect2(x + 44*m/d, y + 55*m/d,
    //            x + 62*m/d, y + 65*m/d,
    //            x + 64*m/d, y + 62*m/d, frontColor);                  // small hand


		fillRect2(tft,
				x +  8*m/d, y + 23*m/d,
				x + 13*m/d, y + 28*m/d,
				x + 30*m/d, y + 13*m/d, fc);                  // left ringer

		fillRect2(tft,
				x + 65*m/d, y + 13*m/d,
				x + 82*m/d, y + 28*m/d,
				x + 87*m/d, y + 23*m/d, fc);                  // right ringer

		tft.fillRect(x + 36*m/d,  y + 36*m/d, 23*m/d, 8*m/d, fc);  // Z top
		tft.fillRect(x + 36*m/d,  y + 60*m/d, 23*m/d, 8*m/d, fc);  // Z bottom

		fillRect2(tft,
				x + 36*m/d, y + 60*m/d,
				x + 45*m/d, y + 59*m/d,
				x + 59*m/d, y + 43*m/d, fc);                  // Z slash
		break;



	case ICONS_Input     :

		tft.fillCircle  (x + 48*m/d, y + 48*m/d, 42*m/d,     fc); // circle contour
		tft.fillCircle  (x + 48*m/d, y + 48*m/d, 34*m/d,     bc);  // inner circle background

		tft.fillCircle  (x + 27*m/d, y + 46*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 68*m/d, y + 46*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 35*m/d, y + 65*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 60*m/d, y + 65*m/d, 5*m/d,      fc); // circle contour

		tft.fillCircle  (x + 40*m/d, y + 27*m/d, 5*m/d,      fc); // circle contour
		tft.fillCircle  (x + 55*m/d, y + 27*m/d, 5*m/d,      fc); // circle contour
		tft.fillRect    (x + 40*m/d, y + 22*m/d, 15*m/d, 10*m/d, fc);  // link
		break;

	case ICONS_Trigger   :

		tft.fillRoundRect    (x +  4*m/d, y + 12*m/d, 87*m/d, 71*m/d, 8, fc);
		FillRectGradient(tft, x + 12*m/d, y + 20*m/d, 71*m/d, 55*m/d, bc);

		FillRectGradient(tft, x +  4*m/d, y + 36*m/d,  8*m/d, 23*m/d, bc);
		tft.fillRect    (x +  4*m/d, y + 44*m/d, 40*m/d,  7*m/d, fc);

		tft.fillTriangle( x + 44*m/d, y + 33*m/d,
				x + 44*m/d, y + 62*m/d,
				x + 59*m/d, y + 47*m/d, fc);
		break;


	case ICONS_Volume	 :

		tft.fillTriangle(   x + 24*m/d, y + 77*m/d,
							x + 71*m/d, y + 28*m/d,
							x + 71*m/d, y + 77*m/d, fc); // top left corner

		break;

	case ICONS_Balance	 :

		tft.fillTriangle(   x + 24*m/d, y + 52*m/d,
							x + 24*m/d, y + 77*m/d,
							x + 47*m/d, y + 77*m/d, fc);

		tft.fillTriangle(   x + 71*m/d, y + 52*m/d,
							x + 71*m/d, y + 77*m/d,
							x + 47*m/d, y + 77*m/d, fc);


		break;

	case ICONS_Offset	 :

		tft.fillTriangle(   x + 47*m/d, y + 52*m/d,
							x + 47*m/d, y + 77*m/d,
							x + 70*m/d, y + 77*m/d, fc);

		tft.fillTriangle(   x + 47*m/d, y + 52*m/d,
							x + 47*m/d, y + 28*m/d,
							x + 70*m/d, y + 28*m/d, fc);
		break;



*/

}


void Icon::E2pCreateDefaultIcons()
{
	E2pCreateIcons(sizeof(DefaultIconDefinition), DefaultIconDefinition);
}

void Icon::E2pDrawIcon(Adafruit_RA8875 tft, uint16_t fc, uint16_t bc, int8_t _icon, int16_t offsetX, int16_t offsetY)
{
	if (_icon < 0 || _icon > MAX_ICONS) {
		Serial.print(F("Error: trying to print icon #"));
		Serial.println(_icon);
		return;
	}



	/*
	Serial.println("IIIIIIIIIIIIIIIIIIIIIIIIIII");
	Serial.print("printing icon: ");
	Serial.print(_icon);
	Serial.print(" forecolor: ");
	Serial.println(fc);
	*/

	int16_t x = layout.x_min + positionX * layout.ss;
	int16_t y = layout.y_min + positionY * layout.ss;

	if (_icon == icon) {
		// store the icon position for touch recognition
		// Serial.println("Full icon");
		icon_pos_x = x;
		icon_pos_y = y;
	}
	// else Serial.println("Partial icon");

	int16_t m = layout.size_m;
	int16_t d = layout.size_d;

	x += offsetX * m/d;
	y += offsetY * m/d;

	// tft.drawRect(x, y, icon_size, icon_size, RA8875_RED); // red container rectangle

	byte totalByteCount = Icon::IconAddresses[_icon + 1] - Icon::IconAddresses[_icon] - 1; // -1 to remove the icon-number byte
	int16_t data[6];
	uint16_t color;
	byte command = 0;
	byte currentByteCnt = 0;
	byte currentDataSize = 0;

	/*
	Serial.println("IconAddr: " + String(Icon::IconAddresses[_icon]) + " Byte count: " + String(totalByteCount));

	Serial.print("tot B Cnt: "); Serial.println(totalByteCount);
	Serial.print("st adr: "); Serial.println(Icon::IconAddresses[_icon]);
	*/
	/*
	for(int i = 0; i < MAX_ICONS + 1; i++) {
		Serial.println("icon st adr: " + String(i)+ " " + String(Icon::IconAddresses[i]));
	}
	*/
	/*
	Serial.print("x: "); Serial.print(x); Serial.print(" y: "); Serial.println(y);
	Serial.print("m: "); Serial.print(m); Serial.print(" d: "); Serial.println(d);
	*/

	// read the icon data
	for(int i = 0; i <= totalByteCount; i++) { // use an extra byte to run the last command
		// byte dt = EEPROM.read(Icon::IconAddresses[_icon] + i + 1);
		byte dt = IconDefCache[Icon::IconAddresses[_icon] + i + 1 - _e2p_IconStoreAddress];

		// Serial.println("d " + String(dt));


		if (currentByteCnt == currentDataSize) {

			/*
			Serial.print("run prev cmd: ");
			Serial.println(command);

			for (int k = 0; k < 6; k++)
				Serial.println(data[k]);
			 */
			// run the previous command
			switch(command) {
				case 0: 		break; // no previus command (first run) or empty definition
				case IC_CM_FRR: tft.fillRoundRect	(x + data[0]*m/d, y + data[1]*m/d,     data[2]*m/d,     data[3]*m/d,     data[4]*m/d,     color); 				break;
				case IC_CM_FR:  tft.fillRect		(x + data[0]*m/d, y + data[1]*m/d,     data[2]*m/d,     data[3]*m/d,     color); 								break;
				case IC_CM_FR2: fillRect2	(tft, 	 x + data[0]*m/d, y + data[1]*m/d, x + data[2]*m/d, y + data[3]*m/d, x + data[4]*m/d, y + data[5]*m/d, color); 	break;
				case IC_CM_FT:  tft.fillTriangle	(x + data[0]*m/d, y + data[1]*m/d, x + data[2]*m/d, y + data[3]*m/d, x + data[4]*m/d, y + data[5]*m/d, color); 	break;
				case IC_CM_FC:  tft.fillCircle		(x + data[0]*m/d, y + data[1]*m/d,     data[2]*m/d,     color); 												break;
				case IC_CM_FCV: tft.fillCurve		(x + data[0]*m/d, y + data[1]*m/d,     data[2]*m/d,     data[3]*m/d,     data[4]*m/d,     color); 				break;
				case IC_CM_CP: 	E2pDrawIcon(tft, fc, bc, data[0], (int8_t)data[1], (int8_t)data[2]); break;
				case IC_CM_NO:  break;
				default: 		Serial.print(F("!! Udf drw cmd "));
						Serial.println(String(command)); break;
			}

			/*
			switch(command) {
				case IC_CM_FRR: Serial.println("tft.fillRoundRect	("+ String(x + data[0]*m/d) + ", " + String(y + data[1]*m/d) + ",     " + String(data[2]*m/d) + ",     " + String(data[3]*m/d) + ",     " + String(data[4]*m/d) + ",     " + String(color) + ")"); 				break;
				case IC_CM_FR:  Serial.println("tft.fillRect		("+ String(x + data[0]*m/d) + ", " + String(y + data[1]*m/d) + ",     " + String(data[2]*m/d) + ",     " + String(data[3]*m/d) + ",     " + String(color) + ")"); 								break;
				case IC_CM_FR2: Serial.println("fillRect2	(tft, 	 "+ String(x + data[0]*m/d) + ", " + String(y + data[1]*m/d) + ", " + String(x + data[2]*m/d) + ", " + String(y + data[3]*m/d) + ", " + String(x + data[4]*m/d) + ", " + String(y + data[5]*m/d) + ", " + String(color) + ")"); 	break;
				case IC_CM_FT:  Serial.println("tft.fillTriangle	("+ String(x + data[0]*m/d) + ", " + String(y + data[1]*m/d) + ", " + String(x + data[2]*m/d) + ", " + String(y + data[3]*m/d) + ", " + String(x + data[4]*m/d) + ", " + String(y + data[5]*m/d) + ", " + String(color) + ")"); 	break;
				case IC_CM_FC:  Serial.println("tft.fillCircle		("+ String(x + data[0]*m/d) + ", " + String(y + data[1]*m/d) + ",     " + String(data[2]*m/d) + ",     " + String(color) + ")"); 												break;
				case IC_CM_FCV: Serial.println("tft.fillCurve		("+ String(x + data[0]*m/d) + ", " + String(y + data[1]*m/d) + ",     " + String(data[2]*m/d) + ",     " + String(data[3]*m/d) + ",     " + String(data[4]*m/d) + ",     " + String(color) + ")"); 				break;
			}
			*/

			if (i < totalByteCount - 1 && ((dt & 0x80) == 0x80)) {
				// if we are not at the end and it is checked as a command byte, process it
				currentByteCnt = 0;

				command = dt & 0x3F;

				if ((dt & 0x40) == 0x40) {
					color = fc;
				} else {
					color = bc;
				}
				// Serial.println("cl: " + String(color));

				/*
				Serial.print("proc new cmd: ");
				Serial.println(command);
				*/

				switch(command) {
					case IC_CM_FRR:  currentDataSize = 5; break;
					case IC_CM_FR:   currentDataSize = 4; break;
					case IC_CM_FR2:  currentDataSize = 6; break;
					case IC_CM_FT:   currentDataSize = 6; break;
					case IC_CM_FC:   currentDataSize = 3; break;
					case IC_CM_FCV:  currentDataSize = 5; break;
					case IC_CM_CP:   currentDataSize = 3; break;
					case IC_CM_NO: 	 currentDataSize = 0; break;
					default: Serial.print(F("!! Udf drw cmd ds "));
						Serial.println(String(command)); break;
				}
			}
		} else
			data[currentByteCnt++] = dt;
	}
}
