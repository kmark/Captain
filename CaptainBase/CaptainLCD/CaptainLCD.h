/*
 * This file is part of Captain.
 *
 * Captain is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Captain is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Captain.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CaptainLCD_h
#define _CaptainLCD_h

#include "LiquidCrystal.h"
#include "Arduino.h"

class CaptainLCD : public LiquidCrystal {
public:
    CaptainLCD(unsigned int, unsigned int, unsigned int, unsigned int,
               unsigned int, unsigned int, unsigned int, unsigned int,
               unsigned int);
    void begin();
    void setGPSActive(bool);
    void setThrust(unsigned int);
    void setThrustLock(bool);
    void setRxActive(bool);
private:
    void setBacklight(unsigned int, unsigned int, unsigned int);
    unsigned int _rgbPins[3];
    bool _gpsActive;
    unsigned int _thrust;
    bool _thrustLock;
    bool _rxActive;
};

#endif
