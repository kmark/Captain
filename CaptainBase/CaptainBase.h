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

#ifndef _CaptainBase_h
#define _CaptainBase_h

#include "Arduino.h"
#include "Base.h"

// Conditional compilation
#define USE_PS3BT // Comment out to use direct USB
#define XBEE_SERIAL Serial3 // Arduino HardwareSerial to use, set baud below
//

namespace config {
    const bool gps = false; // If the Ship module has a GPS onboard
    const unsigned int stickSensitivity = 3; // Prevents servo convulsions
    const ButtonEnum thrustLockButton = CIRCLE; // Button to use for thrust locking
    const ButtonEnum thrustButton = R2; // Analog button to use for thrust control
    const ButtonEnum thrustDirectionButton = SQUARE; // Button to use for forward/reverse change
    const AnalogHatEnum directionalButton = LeftHatX; // Joystick used for directional control
    const unsigned int xbeeBaud = 19200; // XBee baud
    const uint8_t lcdPins[] = { 62, 63, 64, 65, 66, 67, 61, 68, 69 }; // rs, enable, d4, d5, d6, d7, r, g, b
    
    // See CaptainLCD/CaptainLCD.h for static LCD configuration
    
    // Nothing to configure below
#ifdef USE_PS3BT
    const bool bluetooth = true;
#else
    const bool bluetooth = false;
#endif
}

void setup();
void loop();

#endif
