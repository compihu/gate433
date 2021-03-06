/*
 * extdb.h
 *
 *  Created on: Nov 11, 2015
 *      Author: abody
 */

#ifndef _I2CDB_H_
#define _I2CDB_H_

#include "database.h"
#include <sg/I2cEeprom.h>

class i2cdb: public database
{
public:
	i2cdb(sg::I2cEEPROM &eeprom, uint16_t offset);
	virtual ~i2cdb();
	bool	init();

	virtual bool getParams( int code, dbrecord &recout );
	virtual bool setParams( int code, const dbrecord &recin );
	virtual bool setInfo( int code, const dbrecord& recin );
	virtual bool setStatus( int code, dbrecord::POSITION pos );
	virtual void cleanstatuses();
private:
	sg::I2cEEPROM	&m_eeprom;
	uint16_t		m_offset;
};

#endif /* FLASH_H_ */
