/*
 * dac.cpp
 *
 *  Created on: 31 oct. 2015
 *      Author: gael
 */

#include "dac.h"

long Dac::lastStatusSend = 0;
bool Dac::input = 0;

int Dac::begin()
{
	int ret;

	pinMode(__spdif, OUTPUT);
	pinMode(__usb, OUTPUT);
	pinMode(__hostactive, INPUT);
	pinMode(__audiostream, INPUT);
	pinMode(__441, INPUT);
	pinMode(__48, INPUT);
	pinMode(__882, INPUT);
	pinMode(__96, INPUT);
	pinMode(__1764, INPUT);
	pinMode(__192, INPUT);

	Serial1.begin(__baudrate);

	SetInput(0);

	return (ret);
}

void Dac::Loop()
{
	  Dac::ProcessSerial();
	  WriteStatusToSerial();
}

void Dac::ProcessSerial()
{
	String data = "";
	int cnt = 0;
	int MAX = 1024;

	// send data only when you receive data:
	if (Serial1.available() > 0) {
		char c;
		while (c != '\n' && c != '\r' && cnt++ < MAX) {
			// read the incoming bytes:
			c = (char)Serial1.read();
			if ((int)c == -1) continue;
			// Serial1.println(cnt);
			cnt = 0;
			data += c;
		}
	}

	data.trim();

	if (data != "")
	{

		String cmd = data.substring(0, data.indexOf('='));
		String val = data.substring(data.indexOf('=') + 1);

		cmd.toUpperCase();
		val.toUpperCase();

		if (cmd == "S")
		{
			int iv = val.toInt();
			if (iv == 0 || iv == 1)
				SetInput(iv);
		}
	}
}


void Dac::WriteStatusToSerial()
{
	if (millis() - lastStatusSend < __statusPerdiod) return;

	bool s0 = digitalRead(__441);
	bool s1 = digitalRead(__48);
	bool s2 = digitalRead(__882);
	bool s3 = digitalRead(__96);
	bool s4 = digitalRead(__1764);
	bool s5 = digitalRead(__192);

	int s = 0;
	if (s2 && s4) s = 7; // 352.8
	else if (s3 && s5) s = 8; // 384
	else if (s0) s = 1;
	else if (s1) s = 2;
	else if (s2) s = 3;
	else if (s3) s = 4;
	else if (s4) s = 5;
	else if (s5) s = 6;

	Serial1.print("H=");
	Serial1.println(digitalRead(__hostactive));

	Serial1.print("A=");
	Serial1.println(digitalRead(__audiostream));

	Serial1.print("P=");
	Serial1.println(s);

	Serial1.print("S=");
	Serial1.println(Dac::input);


	Serial.print("H=");
	Serial.println(digitalRead(__hostactive));

	Serial.print("A=");
	Serial.println(digitalRead(__audiostream));

	Serial.print("P=");
	Serial.println(s);

	Serial.print("S=");
	Serial.println(Dac::input);

	lastStatusSend = millis();
}

void Dac::SetInput(bool value)
{
	Dac::input = value;
	digitalWrite(__spdif, 0);
	digitalWrite(__usb, 0);
	if (value) {
		digitalWrite(__usb, 1);
	} else {
		digitalWrite(__spdif, 1);
	}
}
