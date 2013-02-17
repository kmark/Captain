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

#include "Ship.h"

TinyGPS gps;
SoftwareSerial gpsSerial(14, 15);
unsigned int loops = 0;

Ship::Ship() : latitude(TinyGPS::GPS_INVALID_F_ANGLE),
 longitude(TinyGPS::GPS_INVALID_F_ANGLE), fixAge(TinyGPS::GPS_INVALID_AGE),
 speed(TinyGPS::GPS_INVALID_F_SPEED) {
    
}

void Ship::setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);
    Serial.print("Starting up GPS communications using TinyGPS v");
    Serial.println(TinyGPS::library_version());
    // GPRMC data only
    gpsSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    // Update once per second (1Hz)
    gpsSerial.println(PMTK_SET_NMEA_UPDATE_1HZ);
}

void Ship::loop() {
    handleGPS();
    
    // For testing
    loops++;
    if(loops % 10000 == 0) {
        if(fixAge == TinyGPS::GPS_INVALID_AGE) {
            Serial.println("Invalid GPS signal...");
            return;
        }
        Serial.print("Location: ");
        Serial.print(latitude, 8);
        Serial.print(", ");
        Serial.print(longitude, 8);
        Serial.print(" Speed: ");
        Serial.print(speed, 2);
        Serial.println(" knots");
    }
}

void Ship::handleGPS() {
    while(gpsSerial.available()) {
        if(gps.encode(gpsSerial.read())) {
            gps.f_get_position(&latitude, &longitude, &fixAge);
            speed = gps.f_speed_knots();
        }
    }
}
