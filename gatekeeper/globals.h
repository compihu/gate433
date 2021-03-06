/*
 * globals.h
 *
 *  Created on: Dec 29, 2015
 *      Author: compi
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_
#include <Arduino.h>
#include "config.h"
#include "thindb.h"
#ifdef USE_INTDB
#include "intdb.h"
#endif
#ifdef USE_HYBRIDDB
#include "hybriddb.h"
#endif
#ifdef USE_HYBINTDB
#include "hybintdb.h"
#endif
#ifdef USE_I2CDB
#include "i2cdb.h"
#endif
#include <SdFat.h>
#include "inductiveloop.h"
#include <arduinooutputs.h>
#include <PCF8574outputs.h>
#include "trafficlights.h"

struct ts;

class PCF8574;
class LiquidCrystal_I2C;
class sdfatlogwriter;
class display;
class DS3231_DST;

extern const uint8_t	g_innerlightspins[3];
extern const uint8_t	g_outerlightspins[3];
extern const uint8_t	g_otherrelaypins[2];

extern display				g_display;
extern SdFat				g_sd;
#ifdef USE_THINDB
extern thindb				g_db;
#endif	//	USE_THINDB
#ifdef USE_INTDB
extern intdb				g_db;
#endif	//	USE_INTDB
#ifdef USE_HYBRIDDB
extern hybriddb				g_db;
#endif	//	USE_HYBRIDDB
#ifdef USE_HYBINTDB
extern hybintdb				g_db;
#endif	//	USE_HYBINTDB
#ifdef USE_I2CDB
extern i2cdb				g_db;
#endif	//	USE_FLASHDB
extern inductiveloop		g_indloop;
extern PCF8574outputs		g_outputs;
extern trafficlights		g_lights;
extern char					g_iobuf[32];
extern uint8_t				g_inidx;
extern uint16_t				g_codedisplayed;
extern bool					g_sdpresent;
extern DS3231_DST			g_clk;
extern ts					g_time;
extern bool					g_timevalid;
extern unsigned long		g_lastdtupdate;

extern sdfatlogwriter		g_logger;

extern uint16_t 			g_lastcheckpoint;

#endif	//	GLOBALS_H_
