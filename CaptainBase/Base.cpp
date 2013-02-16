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

void Base::setup() {
    Serial.begin(115200);
    if(Usb.Init() == -1) {
        Serial.println("OSC did not start!");
        while(1);
    }
    Serial.println("PS3 Bluetooth Library Initialized");
}

void Base::loop() {
    Usb.Task();
    if(PS3.PS3Connected || PS3.PS3NavigationConnected) {
        if(PS3.getButtonClick(PS)) {
            Serial.println("Disconnecting controller...");
            PS3.disconnect();
        }
    }
}
