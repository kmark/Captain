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
#include "avr/io.h"
#include "avr/interrupt.h"

class Base {
public:
    void setup();
    void loop();
    void handleInterrupt();
private:
    bool controllerConnected; // true if controller is connected, false otherwise
    int stickSensitivity; // Prevents servo convulsions
    Button thrustLockButton; // Button to use for thrust locking
    Button thrustButton; // Analog button to use for thrust control
    AnalogHat directionalButton; // Joystick used for directional control
    unsigned int currentThrust; // Current thrust
    Button thrustDirectionButton; // Button to use for forward/reverse change
    bool thrustDirection; // true if forward, false if reverse
    unsigned int oldThrust;
    unsigned int currentDirection; // Current direction
    unsigned int oldDirection;
    bool oldThrustDirection;
    bool thrustLock; // true if thrust lock is on, false otherwise
    LED ds3Battery;

    void handleController(); // Handles all DS3 inputs
    void updateControllerBattery();
    
    void handleRx(); // Handles received data from the XBee
    bool rxEncode(char);
    bool rxTermComplete();
    bool rxInTerm;
    unsigned short rxTermNum;
    byte rxSentenceType;
    char rxBuffer[15];
    unsigned int rxBufferOffset;
    enum { CAPTAIN_SENTENCE_GPS, CAPTAIN_SENTENCE_CDT, CAPTAIN_SENTENCE_UNKNOWN };
    String rxLatitude;
    String rxLongitude;
    bool rxActive;
    unsigned long rxLastTerm;
    String rxSpeed;
    void rxSendTerm(int, char[][15], int);
    bool rxStale();
    unsigned int interruptCount;
    HardwareSerial *XBee;
    CaptainLCD *lcd;
};

#endif
