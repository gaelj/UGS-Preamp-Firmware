/*
 * ugsserial.cpp
 *
 *  Created on: 18 août 2015
 *      Author: gael
 */


#include "ugsserial.h"



bool Ser::DoSerialPrint = 1;

void Ser::SerialPrint(String key, String val)
{
    if (DoSerialPrint) {
        while (!Serial.availableForWrite())
            delay(10);

        // long m1;
        Serial.print(key);
        Serial.print(F("="));
        // m1=millis();
        //for (uint16_t i = 0; i < val.length(); i += 100 )
        //	Serial.print(val.substring(i, i+99));

        Serial.print(val);

        Serial.println("");

        //Serial.println(m1);
        //Serial.println(millis());
        //Serial.println("");
    }
}

void Ser::SerialPrint(String key, uint8_t val)
{
    if (DoSerialPrint) {
        while (!Serial.availableForWrite())
            delay(10);
        Serial.print(key);
        Serial.print(F("="));
        Serial.println(val);
        //Serial.println("");
    }
}
void Ser::SerialPrint(String key, uint16_t val)
{
    if (DoSerialPrint) {
        while (!Serial.availableForWrite())
            delay(10);
        Serial.print(key);
        Serial.print(F("="));
        Serial.println(val);
        //Serial.println("");
    }
}

void Ser::SerialPrint(String key, int16_t val)
{
    if (DoSerialPrint) {
        while (!Serial.availableForWrite())
            delay(10);
        Serial.print(key);
        Serial.print(F("="));
        Serial.println(val);
        //Serial.println("");
    }
}

void Ser::SerialPrint(String key, bool val)
{
    if (DoSerialPrint) {
        while (!Serial.availableForWrite())
            delay(10);
        Serial.print(key);
        Serial.print(F("="));
        Serial.println(val);
        //Serial.println("");
    }
}

void Ser::SerialPrint(String key, byte val[])
{
    if (DoSerialPrint) {
        while (!Serial.availableForWrite())
            delay(10);
        Serial.print(key);
        Serial.print(F("="));
        Serial.println((char*)val);
        //Serial.println("");
    }
}

void Ser::SerialPrintSep(String sep)
{
    while (!Serial.availableForWrite())
        delay(10);
    for (int i = 0; i < 5; i++)
        Serial.print(sep);
    Serial.println("");
}

void Ser::SerialPrint(String key, long val)
{
    if (DoSerialPrint) {
        while (!Serial.availableForWrite())
            delay(10);
        Serial.print(key);
        Serial.print(F("="));
        Serial.println(val);
        //Serial.println("");
    }
}
