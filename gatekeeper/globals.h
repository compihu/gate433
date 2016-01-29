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
#include "intdb.h"
#include "hybriddb.h"
#include "flashdb.h"
#include "inductiveloop.h"
#include "trafficlights.h"

struct ts;

class PCF8574;
class LiquidCrystal_I2C;
class sdfatlogwriter;

extern const uint8_t	g_innerlightspins[3];
extern const uint8_t	g_outerlightspins[3];
extern const uint8_t	g_otherrelaypins[2];

extern LiquidCrystal_I2C	g_lcd;
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
#ifdef USE_FLASHDB
extern flashdb				g_db;
#endif	//	USE_FLASHDB
extern inductiveloop		g_indloop;
extern PCF8574				g_i2cio;
extern char					g_iobuf[32];
extern uint8_t				g_inidx;
extern uint16_t				g_codedisplayed;
extern bool					g_sdpresent;
extern ts					g_t;
extern unsigned long		g_lastdtupdate;

extern sdfatlogwriter		g_logger;

#endif	//	GLOBALS_H_
