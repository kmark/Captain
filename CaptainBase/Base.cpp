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

#include "Base.h"


// I'd really like to move this into the Base class somehow...
USB Usb;
#ifdef USE_PS3BT
    BTD Btd(&Usb);
    PS3BT PS3(&Btd);
#else
    PS3USB PS3(&Usb);
#endif

//CaptainLCD lcd(62, 63, 64, 65, 66, 67);
Base *baseInstance = NULL;

Base::Base() : XBee(Bee(&XBEE_SERIAL, config::xbeeBaud)) {
    //
}

void Base::setup() {
    lcd = new CaptainLCD(config::lcdPins[0], config::lcdPins[1],
                         config::lcdPins[2], config::lcdPins[3],
                         config::lcdPins[4], config::lcdPins[5],
                         config::lcdPins[6], config::lcdPins[7],
                         config::lcdPins[8], &config::gps);
    lcd->begin();

    controllerConnected = false;
    thrustDirection = true;
    currentThrust = 0;
    oldThrust = 1;
    currentDirection = 128;
    oldDirection = 1;
    oldThrustDirection = false;
    thrustLock = false;

    rxSpeed = "";
    rxLatitude = "";
    rxLongitude = "";
    rxActive = false;
    rxLastTerm = 5000;

    Serial.begin(115200);

    if(Usb.Init() == -1) {
        lcd->print("OSC didn't start!");
        while(1);
    }

    interruptCount = 0;
    baseInstance = this;
    TCCR2A = 0; // Wave gen mode normal
    TCCR2B = 0; // Disable
    TCNT2 = 130; // set timer count to 130 of 255
    TIFR2 = 0; // Clear overflow flag
    TIMSK2 = 1; // Enable timer compare interrupt
    TCCR2B = 5; // Timer prescaler to 128

    XBee.setCallback(xbeeCallback);
    XBee.begin();
}

void Base::loop() {
    handleController();
    XBee.tick();
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
#ifdef USE_PS3BT
            PS3.disconnect();
#endif
            lcd->setDS3Connected(false);
            ds3Battery = LED1; // Anything other than 4, 7, 9, or 10
            return;
        }
        currentDirection = PS3.getAnalogHat(config::directionalButton);
        lcd->setDirection(currentDirection);
        if(!thrustLock) {
            currentThrust = PS3.getAnalogButton(config::thrustButton);
            lcd->setThrust(currentThrust);
            if(PS3.getButtonClick(config::thrustDirectionButton)) {
                thrustDirection = !thrustDirection;
                lcd->setThrustDirection(thrustDirection);
            }
        }
        if(PS3.getButtonClick(config::thrustLockButton)) {
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

void Base::handleRx(BeePointerFrame *frame) {
    rxLastTerm = millis();
    switch(frame->data[0]) {
        case CaptainFrames::GPS:
            rxActive = frame->data[1] == 'A';
            // rxLatitude, rxLongitude, and rxSpeed are currently unused
            break;
    }
    if(lcd->setGPSActive(rxActive) && controllerConnected) {
        PS3.setRumbleOn(25, 255, 0, 0);
    }
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

            char cdt[] = { CaptainFrames::CDT, thrustDirection ? 'F': 'R',
                currentDirection, currentThrust};

            XBee.sendData(cdt, sizeof cdt);

            Serial.print("$CDT,");
            Serial.print(thrustDirection ? 'F' : 'R');
            Serial.print(',');
            Serial.print(currentDirection);
            Serial.print(',');
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

void xbeeCallback(BeePointerFrame *frame) {
    baseInstance->handleRx(frame);
}
