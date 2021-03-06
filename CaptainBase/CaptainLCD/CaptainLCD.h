/*
 * Copyright © 2013-2014 Kevin Mark
 *
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

namespace config {
    const uint8_t backlightNoRx[3] = { 200, 0, 0 }; // R,G,B when there is no connection
    const uint8_t backlightNoGps[3] = { 200, 200, 0 }; // When there is a connection but no GPS
    const uint8_t backlightIdeal[3] = { 0, 200, 0 }; // When an ideal connection is present
    const uint8_t backlightOff[3] = { 0, 0, 0 }; // Off
}

class CaptainLCD : public LiquidCrystal {
public:
    CaptainLCD(uint8_t, uint8_t, uint8_t, uint8_t,
               uint8_t, uint8_t, uint8_t, uint8_t,
               uint8_t, const bool *);
    void begin();
    bool setGPSActive(bool);
    void setThrust(unsigned int);
    void setThrustLock(bool);
    void setThrustDirection(bool);
    bool setRxActive(bool);
    void setDirection(unsigned int);
    void setDS3Connected(bool);
private:
    void setBacklight(const uint8_t[3]);
    uint8_t _rgbPins[3];
    bool _gpsActive;
    unsigned int _thrust;
    bool _thrustLock;
    bool _thrustDirection;
    bool _rxActive;
    unsigned int _direction;
    bool _ds3Connected;
    const bool *_gps;
};

#endif
