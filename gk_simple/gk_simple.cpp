// Do not remove the include below
#include "gk_simple.h"
#include "decode433.h"
#include "toolbox.h"
#include "commsyms.h"
#include <serialout.h>
#include <eepromdb.h>
#include "sdfatlogwriter.h"
#include "globals.h"
#include <Wire.h>
#include <ds3231.h>

ts			g_dt;
char		g_iobuf[32];
uint8_t		g_inidx(0);
uint16_t	g_lastcheckpoint;

eepromdb	g_db;

void processinput();



void setup()
{
	bool loginit(false);

#ifdef VERBOSE
	Serial.begin( BAUDRATE );
	delay(10);
	for( char c = 0; c < 79; ++c ) Serial.print('-');
	Serial.println();
#endif

	pinMode( PIN_GATE, OUTPUT );
	digitalWrite( PIN_GATE, RELAY_OFF );
	setup433();
	g_codeready = false;
	g_code = 0;

	for( uint8_t pin = 4; pin <10; ++pin ) {
		pinMode( pin, OUTPUT );
		digitalWrite( pin, RELAY_OFF );
	}

	if( g_sd.begin( SS, SPI_HALF_SPEED )) {
		if( !(loginit = g_logger.init()) )
			Serial.println(F("Logger fail"));
	} else
		Serial.println(F("SD fail"));

	if( !loginit )
	{
		delay(100);
		for( int i = 0; i < 3;  ++i ) {
			digitalWrite( 5, RELAY_ON );
			delay( 500 );
			digitalWrite( 5, RELAY_OFF );
			delay( 500 );
		}
	}

	Wire.begin();
	DS3231_init( DS3231_INTCN );
	DS3231_get( &g_dt );
	g_logger.log( logwriter::INFO, g_dt, F("Reset") );

	digitalWrite( IN_YELLOW, RELAY_ON );
	delay( 500 );
	digitalWrite( OUT_YELLOW, RELAY_ON );

	g_db.init();
}

void loop()
{
	static uint16_t	code(0);
	static uint8_t	cnt(0);

	if( getlinefromserial( g_iobuf, sizeof( g_iobuf ), g_inidx) )
		processinput();

	if( g_codeready )
	{
		uint16_t	id( getid( g_code ));
		uint8_t		btn( getbutton( g_code ));

		if( code != g_code ) {
			if( cnt )
			{
				DS3231_get( &g_dt );
				g_logger.log( logwriter::DEBUG, g_dt, F("Abort"), id, btn );
#ifdef VERBOSE
				Serial.print( F("Aborting ") );
				Serial.print( code );
				Serial.print( ' ' );
				Serial.println( g_code );
#endif
			}
			code = g_code;
			cnt = 0;
		} else if( cnt++ > 3 ) {
			database::dbrecord	rec;

#ifdef VERBOSE
			Serial.print( id );
#endif
			DS3231_get( &g_dt );
			g_db.getParams( id, rec );

			digitalWrite( IN_YELLOW, RELAY_OFF );
			digitalWrite( OUT_YELLOW, RELAY_OFF );

			if( rec.enabled() )
			{
				digitalWrite( IN_GREEN, RELAY_ON );
				digitalWrite( OUT_GREEN, RELAY_ON );
#ifdef VERBOSE
				Serial.println(F(" -> opening gate."));
#endif
				digitalWrite( PIN_GATE, RELAY_ON );
				g_logger.log( logwriter::INFO, g_dt, F("Ack"), id, btn );
				delay(1000);
				digitalWrite( PIN_GATE, RELAY_OFF );
				delay(20000);
				digitalWrite( IN_GREEN, RELAY_OFF );
				digitalWrite( OUT_GREEN, RELAY_OFF );
			} else {
#ifdef VERBOSE
			Serial.println(F(" -> ignoring."));
#endif
				digitalWrite( IN_RED, RELAY_ON );
				digitalWrite( OUT_RED, RELAY_ON );
				g_logger.log( logwriter::INFO, g_dt, F("Deny"), id, btn );
				delay(3000);
				digitalWrite( IN_RED, RELAY_OFF );
				digitalWrite( OUT_RED, RELAY_OFF );
			}
			digitalWrite( IN_YELLOW, RELAY_ON );
			digitalWrite( OUT_YELLOW, RELAY_ON );
			cnt = 0;
		} else Serial.print('.');
		g_codeready = false;
	}
}

//////////////////////////////////////////////////////////////////////////////
void processinput()
{
	const char	*inptr( g_iobuf );

	Serial.println( g_iobuf );

	if( iscommand( inptr, F("dl"))) {
		g_logger.dump( &Serial );
	} else if( iscommand( inptr, F("tl"))) {	// truncate log
		g_logger.truncate();

	} else {
		Serial.println( F(ERRS "CMD"));
	}
	g_inidx = 0;
}

