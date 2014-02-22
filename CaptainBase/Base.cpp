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

#include "Base.h"


// I'd really like to move this into the Base class somehow...
USB Usb;
#ifdef USEPS3BT
    BTD Btd(&Usb);
    PS3BT PS3(&Btd);
#else
    PS3USB PS3(&Usb);
#endif

//CaptainLCD lcd(62, 63, 64, 65, 66, 67);
Base *baseInstance = NULL;

void Base::setup() {
    lcd = new CaptainLCD(62, 63, 64, 65, 66, 67, 61, 68, 69);
    lcd->begin();
    
    controllerConnected = false;
    stickSensitivity = 3;
    thrustLockButton = CIRCLE;
    thrustDirectionButton = SQUARE;
    thrustDirection = true;
    thrustButton = R2;
    directionalButton = LeftHatX;
    currentThrust = 0;
    oldThrust = 1;
    currentDirection = 128;
    oldDirection = 1;
    oldThrustDirection = false;
    thrustLock = false;
    
    rxTermNum = 0;
    rxSentenceType = CAPTAIN_SENTENCE_UNKNOWN;
    rxSpeed = NULL;
    rxLatitude = NULL;
    rxLongitude = NULL;
    rxActive = false;
    rxLastTerm = 5000;
    
    Serial.begin(115200);
    Serial3.begin(19200);

    if(Usb.Init() == -1) {
        lcd->print("OSC didn't start!");
        while(1);
    }

    interruptCount = 0;
    XBee = &Serial3;
    baseInstance = this;
    TCCR2A = 0; // Wave gen mode normal
    TCCR2B = 0; // Disable
    TCNT2 = 130; // set timer count to 130 of 255
    TIFR2 = 0; // Clear overflow flag
    TIMSK2 = 1; // Enable timer compare interrupt
    TCCR2B = 5; // Timer prescaler to 128
}

void Base::loop() {
    handleController();
    handleRx();
}

void Base::handleController() {
    Usb.Task();
    if(PS3.PS3Connected || PS3.PS3NavigationConnected) {
        if(!controllerConnected) {
            controllerConnected = true;
            lcd->setDS3Connected(true);
            updateControllerBattery();
        }
        if(PS3.getButtonClick(PS)) {
            PS3.disconnect();
            lcd->setDS3Connected(false);
            ds3Battery = LED1; // Anything other than 4, 7, 9, or 10
            return;
        }
        currentDirection = PS3.getAnalogHat(directionalButton);
        lcd->setDirection(currentDirection);
        if(!thrustLock) {
            currentThrust = PS3.getAnalogButton(thrustButton);
            lcd->setThrust(currentThrust);
            if(PS3.getButtonClick(thrustDirectionButton)) {
                thrustDirection = !thrustDirection;
                lcd->setThrustDirection(thrustDirection);
            }
        }
        if(PS3.getButtonClick(thrustLockButton)) {
            thrustLock = !thrustLock;
            lcd->setThrustLock(thrustLock);
        }
    }
    else if(controllerConnected) {
        controllerConnected = false;
        lcd->setDS3Connected(false);
        ds3Battery = LED1;
    }
}

void Base::updateControllerBattery() {
    if(PS3.PS3Connected || PS3.PS3NavigationConnected) {
        LEDEnum batt;
        if(PS3.getStatus(Dying)) batt = LED4;
        else if (PS3.getStatus(Low)) batt = LED7;
        else if (PS3.getStatus(High)) batt = LED9;
        else if (PS3.getStatus(Full)) batt = LED10;
        if(ds3Battery != batt) {
            PS3.setLedOn(batt);
            ds3Battery = batt;
        }
    }
}

void Base::handleRx() {
    while(Serial3.available()) {
        char lolz = Serial3.read();
        Serial.write(lolz);
        if(rxEncode(lolz)) {
            if(lcd->setGPSActive(rxActive) && controllerConnected) {
                PS3.setRumbleOn(25, 255, 0, 0);
            };
        }
    }
}

// The below will probably be put into some kind
// of "CaptainCommunication" library...
// Based on NMEA and TinyGPS

bool Base::rxEncode(char c) {
    bool valid = false;
    switch(c) {
        case ',': // term terminators
        case '\r':
        case '\n':
            // Ignores the LF or \n in CR+LF or \r\n terminators
            if(rxBufferOffset == 0) {
                return valid;
            }
            valid = rxTermComplete();
            rxBufferOffset = 0;
            memset(rxBuffer, 0, sizeof(rxBuffer));
            rxTermNum++;
            return valid;
        case '$': // sentence begin
            rxSentenceType = CAPTAIN_SENTENCE_UNKNOWN;
            rxTermNum = rxBufferOffset = 0;
            memset(rxBuffer, 0, sizeof(rxBuffer));
            return valid;
    }
    // Data. Not a $ or terminator. Add it to the buffer.
    rxBuffer[rxBufferOffset++] = c;
    return valid;
}

bool Base::rxTermComplete() {
    rxLastTerm = millis();
    if(rxTermNum == 0) {
        if(!strcmp(rxBuffer, "GPS")) {
            rxSentenceType = CAPTAIN_SENTENCE_GPS;
        }
    }
    switch (rxSentenceType) {
        case CAPTAIN_SENTENCE_GPS:
            switch(rxTermNum) {
                case 1:
                    rxActive = rxBuffer[0] == 'A';
                    break;
                case 2:
                    rxLatitude = String(rxBuffer);
                    break;
                case 3:
                    rxLongitude = String(rxBuffer);
                    break;
                case 4:
                    rxSpeed = String(rxBuffer);
                    break;
            }
            break;
        default:
            return false;
    }
    return true;
}

// dsize = sizeof(data)
void Base::rxSendTerm(int termType, char data[][15], int dsize) {
    String term = "$";
    switch(termType) {
        case CAPTAIN_SENTENCE_CDT:
            term += "CDT";
            break;
        default:
            return;
    }
    for(int i = 0; i < dsize; i++) {
        term += ",";
        term += data[i];
    }
    term += "\r\n";
    XBee->println(term);
}

// Convenience function
bool Base::rxStale() {
    unsigned long mil = millis();
    if(mil <= 10000 && rxLastTerm == 5000) {
        return true;
    }
    return mil - rxLastTerm >= 5000;
    // Shorthand for the above
    // return (mil <= 10000 && rxLastTerm == 5000) ? true : (mil - rxLastTerm >= 5000);
}

void Base::handleInterrupt() {
    interruptCount++;
    // For time-sensitive data
    if(interruptCount % 50 == 0) {
        bool stale = rxStale();
        if(lcd->setRxActive(!stale) && controllerConnected) {
            PS3.setRumbleOn(25, 255, 0, 0);
        }
        if(!stale && (oldThrust != currentThrust ||
                      oldDirection != currentDirection ||
                      oldThrustDirection != thrustDirection)) {
            oldThrust = currentThrust;
            oldDirection = currentDirection;
            oldThrustDirection = thrustDirection;
            XBee->print("$CDT,");
            Serial.print("$CDT,");
            XBee->print(thrustDirection ? 'F' : 'R');
            Serial.print(thrustDirection ? 'F' : 'R');
            XBee->print(',');
            Serial.print(',');
            XBee->print(currentDirection);
            Serial.print(currentDirection);
            XBee->print(',');
            Serial.print(',');
            XBee->println(currentThrust);
            Serial.println(currentThrust);
        }
    }
    // For things that only need to be run every once in awhile
    if(interruptCount % 10000 == 0) {
        updateControllerBattery();
    }
}

ISR(TIMER2_OVF_vect) {
    baseInstance->handleInterrupt();
}
