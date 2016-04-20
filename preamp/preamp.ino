#include <EEPROM.h>
#include <SPI.h>
#include <MCP23S17.h>
#include <Adafruit_GFX_AS.h>
#include <Adafruit_RA8875.h>
#include <IRremote2.h>
#include <MUSES.h>
#include <ugs.h>

void setup() { Ugs::begin(); }
void loop()  { Ugs::Loop();  }
