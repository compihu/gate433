/*
 * gatehandler.cpp
 *
 *  Created on: Oct 29, 2015
 *      Author: compi
 */

#include "config.h"
#include "gatehandler.h"
#include "decode433.h"

gatehandler::gatehandler( database &db
			, trafficlights &lights
			, inductiveloop &loop
			, LiquidCrystal_I2C lcd
			, bool enforcepos
			, bool enforcedt )
: m_db( db )
, m_lights( lights )
, m_indloop( loop )
, m_lcd( lcd )
, m_enforcepos( enforcepos )
, m_enforcedt( enforcedt )
, m_status( WAITSETTLE )
, m_tlstatus( trafficlights::OFF )
, m_ilstatus( inductiveloop::NONE )
, m_conflict( false )
, m_inner( false )
//, m_innersaved( false )
{
}

void gatehandler::loop( unsigned long currmillis )
{
	inductiveloop::STATUS	ilstatus;
	bool					conflict, ilchanged, inner;

	conflict = m_indloop.update( ilstatus );
	ilchanged = (ilstatus != m_ilstatus) || (conflict != m_conflict );
	inner = ilstatus == inductiveloop::INNER;

	switch( m_status )
	{
	case WAITSETTLE:
		if( !ilchanged ) break;
		if( ilstatus == inductiveloop::NONE && m_lights != trafficlights::OFF ) {
			m_lights.set( trafficlights::OFF, inner );
		} else if( conflict ) {
			m_lights.set( trafficlights::CONFLICT, inner );
		} else if( ilstatus != inductiveloop::NONE ) {
			startcodewait( inner );
		}
		break;

	case CODEWAIT:
		if( ilchanged ) {
			if( conflict ) {
				m_lights.set( trafficlights::CONFLICT, inner );
				m_status = WAITSETTLE;
			} else if( ilstatus == inductiveloop::NONE ) {
				m_lights.set( trafficlights::OFF, inner );
				m_status = WAITSETTLE;
			} else {
				startcodewait( inner );	// wtf???
			}
			break;
		}
		if( g_codeready ) {
			serialoutln( F(CMNTS "Code received: "), g_code >> 2);
			if( authorize( g_code, inner ) ) {
				m_lights.set( trafficlights::ACCEPTED, inner );
				m_status = PASS;
			} else {
				m_lights.set( trafficlights::DENIED, inner );
				m_inner = inner;
				m_status = RETREAT;
			}
		}
		break;

	case PASS:
		if( !ilchanged ) break;
		if( conflict ) {
			m_lights.set( trafficlights::PASS, inner );
		} else if( ilstatus == inductiveloop::NONE ) {
			m_lights.set( trafficlights::OFF, inner );
			m_status = WAITSETTLE;
		}
		break;

	case RETREAT:
		if( !ilchanged ) break;
		if( ilstatus != ( m_inner ? inductiveloop::INNER : inductiveloop::OUTER)) {
			if( ilstatus == inductiveloop::NONE ) {
				m_lights.set( trafficlights::OFF, inner );
				m_status = WAITSETTLE;
			} else {
				startcodewait( inner );
			}
		}
		break;
	}

	m_ilstatus = ilstatus;
	m_conflict = conflict;

	m_lights.loop( currmillis );
}

bool gatehandler::authorize( uint16_t code, bool inner )
{
	uint16_t	id( code >> 2 );
	return (id&1) == 0;
}