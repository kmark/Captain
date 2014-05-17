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

#ifndef _Ship_h
#define _Ship_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "TinyGPS.h"
#include "Servo.h"
#include "Bee.h"
#include "avr/io.h"
#include "avr/interrupt.h"

// NMEA commands
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

void xbeeCallback(BeePointerFrame *frame);

namespace CaptainFrames {
    static const uint8_t GPS = 0x47;
    static const uint8_t CDT = 0x44;
}

class Ship {
    float latitude;
    float longitude;
    unsigned long fixAge;
    float speed; // In knots
    unsigned int interruptCount;

    unsigned int thrust;
    unsigned int direction;
    bool thrustDirection;

    //SoftwareSerial *XBee;
    Bee XBee;
    HardwareSerial *gpsSerial;
    TinyGPS *gps;
    Servo *rudder;
public:
    Ship();
    void setup();
    void loop();
    void handleInterrupt();
    void handleRx(BeePointerFrame *frame);
private:
    void handleGPS();
};

#endif
