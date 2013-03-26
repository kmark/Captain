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

#include "CaptainLCD.h"

byte check[8] = {0,1,3,22,28,8,0,0};
byte xmark[8] = {0,27,14,4,14,27,0,0};

CaptainLCD::CaptainLCD(unsigned int rs, unsigned int enable, unsigned int d4,
                       unsigned int d5, unsigned int d6, unsigned int d7)
: LiquidCrystal(rs, enable, d4, d5, d6, d7) {
    LiquidCrystal::createChar(1, check);
    LiquidCrystal::createChar(2, xmark);
    _gpsActive = false;
    _thrust = 0;
    _thrustLock = false;
    _rxActive = false;
}

void CaptainLCD::begin() {
    LiquidCrystal::begin(16, 2);
    LiquidCrystal::clear();
    LiquidCrystal::setCursor(0, 0);
    LiquidCrystal::print("T  0% U RX");
    LiquidCrystal::write(2);
    LiquidCrystal::print(" GPS");
    LiquidCrystal::write(2);
}

void CaptainLCD::setGPSActive(bool active) {
    if(_gpsActive == active) {
        return;
    }
    unsigned int symbol = active ? 1 : 2;
    LiquidCrystal::setCursor(15, 0);
    LiquidCrystal::write(symbol);
    _gpsActive = active;
}

void CaptainLCD::setThrust(unsigned int thrust) {
    if(_thrust == thrust) {
        return;
    }
    _thrust = thrust;
    thrust = map(thrust, 0, 255, 0, 100);
    LiquidCrystal::setCursor(1, 0);
    char buffer[4];
    sprintf(buffer, "%3u", thrust);
    LiquidCrystal::print(buffer);
}

void CaptainLCD::setThrustLock(bool locked) {
    if(_thrustLock == locked) {
        return;
    }
    char text = locked ? 'L' : 'U';
    LiquidCrystal::setCursor(6, 0);
    LiquidCrystal::print(text);
    _thrustLock = locked;
}

void CaptainLCD::setRxActive(bool active) {
    if(_rxActive == active) {
        return;
    }
    unsigned int symbol = active ? 1 : 2;
    LiquidCrystal::setCursor(10, 0);
    LiquidCrystal::write(symbol);
    _rxActive = active;
    // If our Rx is inactive then our GPS can't be active either
    if(!active) {
        setGPSActive(false);
    }
}