/*
 * MUSES.h
 *
 *  Created on: 9 juin 2015
 *      Author: gael
 */

#ifndef MUSES_H_
#define MUSES_H_

#include <Arduino.h>
#include <SPI.h>

#define _MUSES_VolumeSteps				224 // without counting mute

class MUSES
{
public:
	MUSES(byte SPI_Vol_CS, byte _channel);
	void SetParameters(byte LRcont1, byte LRcont2, byte ZC); // L/R synced + ZC active
	float SetAttenuation(byte volume);
	void Mute();

private:
	byte _SPI_Vol_CS;
	void SPI_Start();
	void SPI_End();
	byte channel;
};


#endif /* MUSES_H_ */
