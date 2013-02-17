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

#ifndef _Ship_h
#define _Ship_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "TinyGPS.h"

class Ship {
public:
    Ship();
    void setup();
    void loop();
private:
    float latitude;
    float longitude;
    unsigned long fixAge;
    float speed; // In knots
    float altitude; // In meters
};

#endif