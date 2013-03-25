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
Ship* shipInstance = NULL; // Used for interrupts

Ship::Ship() : latitude(TinyGPS::GPS_INVALID_F_ANGLE),
 longitude(TinyGPS::GPS_INVALID_F_ANGLE), fixAge(TinyGPS::GPS_INVALID_AGE),
 speed(TinyGPS::GPS_INVALID_F_SPEED), interruptCount(0) {
     shipInstance = this;
}

void Ship::setup() {
    Serial.begin(19200); // XBee
    gpsSerial.begin(9600);
    // GPRMC data only
    gpsSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    // Update once per second (1Hz)
    gpsSerial.println(PMTK_SET_NMEA_UPDATE_1HZ);
    
    /* Initialize the data output interrupt. It'll fire every millisecond
     * * Timer2 is 8-bits wide, so it can count to 255
     * * 255 - 130 gives us 125 cycles left
     * * (125 cycles) * (128 prescaler) / (16MHz clock speed) = 1ms
     * * Setting the prescalar resets the interrupt
     * * We must reset the count and overflow flag after each interrupt
     * * http://arduinomega.blogspot.com/2011/05/timer2-and-overflow-interrupt-lets-get.html
     */
    
    TCCR2A = 0; // Wave gen mode normal
    TCCR2B = 0; // Disable
    TCNT2 = 130; // set timer count to 130 of 255
    TIFR2 = 0; // Clear overflow flag
    TIMSK2 = 1; // Enable timer compare interrupt
    TCCR2B = 5; // Timer prescaler to 128
}

void Ship::loop() {
    handleGPS();
}

void Ship::handleGPS() {
    while(gpsSerial.available()) {
       if(gps.encode(gpsSerial.read())) {
           gps.f_get_position(&latitude, &longitude, &fixAge);
           speed = gps.f_speed_knots();
        }
    }
}

// Set to fire every 1ms
void Ship::handleInterrupt() {
    interruptCount++;
    // This will be true every 1 second
    if(interruptCount > 999) {
        interruptCount = 0;
        // Push GPS data
        Serial.print("$GPS,");
        Serial.print(fixAge == TinyGPS::GPS_INVALID_AGE ? "V" : "A");
        Serial.print(",");
        Serial.print(latitude, 10);
        Serial.print(",");
        Serial.print(longitude, 10);
        Serial.print(",");
        Serial.println(speed, 2);
    }
    TCNT2 = 130; // Reset timer to 130 of 255
    TIFR2 = 0; // Clear overflow flag
}

ISR(TIMER2_OVF_vect) {
    shipInstance->handleInterrupt();
}