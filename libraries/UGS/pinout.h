/*
 * pinout.h
 *
 *  Created on: 3 juin 2015
 *      Author: gael
 */

#ifndef UGS_PINOUT_H_
#define UGS_PINOUT_H_

// *** Inputs ***
#define __vu0			A2
#define __vu1			A3

#define __VolPush  		19 // int 0
#define __VolEnc1  		2  // int 4
#define __VolEnc2  		3  // int 1

// *** Interrupts ***
#define __VolEnc1Int	0
#define __VolEnc2Int	1
#define __VolPushInt	4
#define __TftInt		5

// *** SPI ***
#define __miso  		50
#define __mosi  		51
#define __sclk  		52

#define __SPI_Mux_CS   	5
#define __SPI_Vol_CS	6

#define RA8875_RESET 	8
#define RA8875_WAIT		9
#define RA8875_INT 		18  // int 5
#define RA8875_CS 		11

// *** Led Outputs ***
#define __Led_A_1    	30
#define __Led_A_2    	32
#define __Led_A_3    	34
#define __Led_A_4    	36
#define __Led_A_5    	38
#define __Led_A_6    	40
#define __Led_A_7    	42
#define __Led_A_8    	48

#define __Led_B_1    	29
#define __Led_B_2    	27
#define __Led_B_3    	25
#define __Led_B_4    	23
#define __Led_B_5    	22
#define __Led_B_6    	24
#define __Led_B_7    	26
#define __Led_B_8    	28

// *** Power relay Outputs ***
#define __PwrRly1		47
#define __PwrRly2		45
#define __5VRly			39

// *** Trigger relay Outputs ***
#define __ExtSwRly1		43
#define __ExtSwRly2		41

// *** Pwm Outputs ***
#define __PwmTft		13
#define __PwmVolLed1	44
#define __PwmVolLed2	46
#define __PwmRearLed1  	12
#define __PwmRearLed2  	10

// *** Ir i/o ***
#define __irOut  		49
#define __irIn     		7

// *** Com ports ***
#define __dac_rx		15
#define __dac_tx		14

// *** MCP relays ***
#define _mcpCh_RlyInAssym  	0
#define _mcpCh_RlyBypass   	1
#define _mcpCh_RlyIn1      	2
#define _mcpCh_RlyIn2      	3
#define _mcpCh_RlyIn3      	4
#define _mcpCh_RlyIn4      	5
#define _mcpCh_RlyIn5      	6
#define _mcpCh_RlyOutAssym 	7

// *** MCP Leds + 1 relay ***
#define _mcpCh_RlyLedExtOut	8
#define _mcpCh_LedIn1		9
#define _mcpCh_LedIn2		10
#define _mcpCh_LedIn3		11
#define _mcpCh_LedIn4		12
#define _mcpCh_LedIn5		13
#define _mcpCh_LedSymOut  	14
#define _mcpCh_LedAsymOut	15

#endif /* UGS_PINOUT_H_ */
