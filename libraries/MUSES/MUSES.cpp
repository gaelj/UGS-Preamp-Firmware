/*
 * MUSES.cpp
 *
 *  Created on: 9 juin 2015
 *      Author: gael
 */


#include "MUSES.h"

byte _SPI_Vol_CS;
byte channel;
word val;

MUSES::MUSES(byte SPI_VOL_CS, byte _channel)
{
	_SPI_Vol_CS = SPI_VOL_CS;
	channel = _channel;
	pinMode(_SPI_Vol_CS, OUTPUT);
	SPI.begin();
}


void MUSES::SetParameters(byte LRcont1, byte LRcont2, byte ZC)
{
	// byte LRcont1 = 1; // 1 = ON
	// byte LRcont2 = 1; // 1 = ON
	// byte ZC = 0; // 0 = ON

	SPI_Start();

	val = 0;
	val |= (LRcont1 << 7);
	val |= (LRcont2 << 6);
	val |= (ZC << 5);

	SPI.transfer(val);

	val = channel;
	val |= (1 << 6);
	SPI.transfer(val);

	SPI_End();
}

float MUSES::SetAttenuation(byte volume)
{
	if (volume > _MUSES_VolumeSteps)
	{
		Serial.print(F("Volume error: "));
		Serial.println(volume);
		return (-1);
	}

	// Serial.print("Setting volume to ");
	// Serial.println(volume);

	SPI_Start();

	val = 0;
	if (volume > 0)
		val |= (_MUSES_VolumeSteps + 0x10 - volume);
	// Serial.print(val, BIN);
	// Serial.print(" ");
	SPI.transfer(val);

	val = channel;
	SPI.transfer(val);


	//Serial.println(val, BIN);
	SPI_End();

	if (volume > 0)
		return ((volume - _MUSES_VolumeSteps) / 2); // attenuation in db
	else
		return(-120.0);
}

void MUSES::Mute()
{
	SetAttenuation(0);
}




void MUSES::SPI_Start()
{	
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV64);
	SPI.setDataMode(SPI_MODE2);
	digitalWrite(_SPI_Vol_CS, LOW);
}

void MUSES::SPI_End()
{
	digitalWrite(_SPI_Vol_CS, HIGH);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV64);
	SPI.setDataMode(SPI_MODE0);
}
