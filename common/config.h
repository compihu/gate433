/*
 * config.h
 *
 *  Created on: Nov 12, 2015
 *      Author: compi
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define USE_DS3231
//#define USE_INDUCTION_LOOPS

//#define FAILSTATS
//#define VERBOSE
//#define DBGSERIALIN
#define EXPECT_RESPONSE

#define SHORT_MIN_TIME	220
#define SHORT_MAX_TIME	510
#define LONG_MIN_TIME	580
#define LONG_MAX_TIME	1100
#define CYCLE_MAX_TIME	( SHORT_MAX_TIME + LONG_MAX_TIME )
#define CYCLE_MIN_TIME	( SHORT_MIN_TIME + LONG_MIN_TIME )
#define	STOP_MIN_TIME	12000

#define LOOP_ACTIVE	HIGH

#define PIN_RFIN		2
#define PIN_INNERLOOP	A0
#define PIN_OUTERLOOP	A1

//PCF8574 outputs
#define PCF8574_ADDRESS		0x20
#define INNER_LIGHTS_PINS 	{ 0,1,2 }
#define OUTER_LIGHTS_PINS 	{ 3,7,6 }
#define	PIN_RELAY_SPARE		5
#define PIN_GATE			4

#define RELAY_OFF	HIGH
#define RELAY_ON	LOW

#define LCD_ADDRESS	0x27	//	usually 0x27 or 0x3f
#define LCD_WIDTH	16		//	in characters
#define LCD_HEIGHT	2		//	in lines

#define	ENFORCE_POS	false	//	enforce position requirements
#define	ENFORCE_DT	true	//	enforce date-time requirements

#define GATE_OPEN_PULSE_WIDTH	1000

#define HYBRIDDB_EEPROM_OFFSET 0
#define HYBRIDDB_EEPROM_ADDRESS 0x57
#define FLASHDB_EEPROM_OFFSET 0
#define FLASHDB_EEPROM_ADDRESS 0x50


//#define USE_INTDB
//#define USE_THINDB
//#define USE_HYBRIDDB
#define USE_FLASHDB

#endif /* CONFIG_H_ */
