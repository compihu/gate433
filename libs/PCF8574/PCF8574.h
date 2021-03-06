//
//    FILE: PCF8574.H
//  AUTHOR: Rob Tillaart
//    DATE: 02-febr-2013
// VERSION: 0.1.04
// PURPOSE: I2C PCF8574 library for Arduino
//     URL:
//
// HISTORY:
// see PCF8574.cpp file
//

#ifndef _PCF8574_H
#define _PCF8574_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define PCF8574_LIB_VERSION "0.1.04"

class PCF8574
{
public:
    PCF8574(uint8_t deviceAddress);
    uint8_t read8(uint8_t &value);
    uint8_t read(uint8_t pin, uint8_t &value);
    uint8_t value();

    uint8_t write8(uint8_t value);
    uint8_t write(uint8_t pin, uint8_t value);

    uint8_t toggle(uint8_t pin);
    uint8_t shiftRight(uint8_t n=1);
    uint8_t shiftLeft(uint8_t n=1);

    int lastError();

private:
    uint8_t read8();
    uint8_t _address;
    uint8_t _data;
    int _error;
};

#endif
//
// END OF FILE
//
