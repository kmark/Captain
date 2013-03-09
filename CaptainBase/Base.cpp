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

LiquidCrystal lcd(62, 63, 64, 65, 66, 67);
Base *baseInstance = NULL;

void Base::setup() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("Loading...");
    
    controllerConnected = false;
    stickSensitivity = 3;
    thrustLockButton = CIRCLE;
    thrustButton = R2;
    directionalButton = LeftHatX;
    currentThrust = 0;
    currentDirection = 0;
    thrustLock = false;
    
    rxTermNum = 0;
    rxSentenceType = CAPTAIN_SENTENCE_UNKNOWN;
    rxSpeed = NULL;
    rxLatitude = NULL;
    rxLongitude = NULL;
    rxActive = false;
    rxLastTerm = 5000;
    
    analogWrite(69, 255);
    
    Serial.begin(115200);
    Serial3.begin(19200);

    if(Usb.Init() == -1) {
        lcd.print("OSC didn't start!");
        while(1);
    }
    lcd.setCursor(0, 0);
    lcd.print("Ready for DS3...");
    lcd.setCursor(0, 1);
    lcd.print("Awaiting XBee...");

    interruptCount = 0;
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
            lcd.setCursor(0, 0);
            lcd.print("DS3 Connected   ");
        }
        if(PS3.getButtonClick(PS)) {
            PS3.disconnect();
            lcd.setCursor(0, 0);
            lcd.println("DS3 Disconnected");
            return;
        }
        currentDirection = PS3.getAnalogHat(directionalButton);
        if(!thrustLock) {
            currentThrust = PS3.getAnalogButton(thrustButton);
        }
        if(PS3.getButtonClick(thrustLockButton)) {
            thrustLock = !thrustLock;
        }
    }
    else if(controllerConnected) {
        controllerConnected = false;
    }
}

void Base::handleRx() {
    while(Serial3.available()) {
        if(rxEncode(Serial3.read())) {
            lcd.setCursor(0, 1);
            lcd.print(rxActive ? "GPS: Active   " : "GPS: Not Active ");
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
    Serial.println(term);
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
    if(interruptCount % 100 == 0 && !rxStale()) {
        char data[2][15] = { NULL, NULL };
        sprintf(data[0], "%u", currentDirection);
        sprintf(data[1], "%u", currentThrust);
        rxSendTerm(CAPTAIN_SENTENCE_CDT, data, 2);
    }
}

ISR(TIMER2_OVF_vect) {
    baseInstance->handleInterrupt();
}
