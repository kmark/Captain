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

#ifndef _Base_h
#define _Base_h

#include "Arduino.h"
#include "PS3BT.h"
#include "PS3USB.h"
#include "CaptainBase.h"
#include "CaptainLCD.h"
#include "Bee.h"
#include "avr/io.h"
#include "avr/interrupt.h"

void xbeeCallback(BeePointerFrame *frame);

namespace CaptainFrames {
    static const uint8_t GPS = 0x47;
    static const uint8_t CDT = 0x44;
}

class Base {
public:
    Base();
    void setup();
    void loop();
    void handleInterrupt();
    void handleRx(BeePointerFrame *); // Handles received data from the XBee
private:
    bool controllerConnected; // true if controller is connected, false otherwise
    unsigned int currentThrust; // Current thrust
    bool thrustDirection; // true if forward, false if reverse
    unsigned int oldThrust;
    unsigned int currentDirection; // Current direction
    unsigned int oldDirection;
    bool oldThrustDirection;
    bool thrustLock; // true if thrust lock is on, false otherwise
    LEDEnum ds3Battery;

    void handleController(); // Handles all DS3 inputs
    void updateControllerBattery();

    String rxLatitude;
    String rxLongitude;
    bool rxActive;
    unsigned long rxLastTerm;
    String rxSpeed;
    bool rxStale();
    unsigned int interruptCount;
    Bee XBee;
    CaptainLCD *lcd;
};

#endif
