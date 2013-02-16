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

void Base::setup() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("Loading...      ");
    
    controllerConnected = false;
    stickSensitivity = 3;
    thrustLockButton = CIRCLE;
    thrustButton = R2;
    directionalButton = LeftHatX;
    currentThrust = 0;
    currentDirection = 0;
    thrustLock = false;
    
    analogWrite(69, 255);
    
    Serial.begin(115200);

    if(Usb.Init() == -1) {
        lcd.print("OSC didn't start!");
        while(1);
    }
    lcd.setCursor(0, 0);
    lcd.print("Ready for DS3...");
    lcd.setCursor(0, 1);
    lcd.print("Awaiting XBee...");
}

void Base::loop() {
    handleController();
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
