/*
 * outputs.h
 *
 *  Created on: Mar 8, 2016
 *      Author: compi
 */

#ifndef ARDUINOOUTPUTS_H_
#define ARDUINOOUTPUTS_H_
#include <Arduino.h>
#include <outputs.h>

class arduinooutputs : public outputs
{
public:
	arduinooutputs() {}
	arduinooutputs(const uint8_t pins[], uint8_t value) { init(pins, value); }
	bool init(const uint8_t pins[], uint8_t value);

	virtual uint8_t set(uint8_t pin, uint8_t value);
	virtual uint8_t set(uint8_t value);
private:
	uint8_t	m_pins[8];
};

#endif /* ARDUINOOUTPUTS_H_ */
