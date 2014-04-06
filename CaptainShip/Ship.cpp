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

Ship* shipInstance = NULL; // Used for interrupts

Ship::Ship() : latitude(TinyGPS::GPS_INVALID_F_ANGLE),
 longitude(TinyGPS::GPS_INVALID_F_ANGLE), fixAge(TinyGPS::GPS_INVALID_AGE),
 speed(TinyGPS::GPS_INVALID_F_SPEED), interruptCount(0),
 XBee(Bee(new SoftwareSerial(18, 19), 19200)) {
     shipInstance = this;
     thrust = 0;
     direction = 128;
     thrustDirection = true;
     gps = new TinyGPS();
     gpsSerial = &Serial;
     //XBee = new SoftwareSerial(18, 19);
     rudder = new Servo();
}

void Ship::setup() {
    XBee.setCallback(xbeeCallback);
    XBee.begin();
    //gpsSerial->begin(9600);
    // GPRMC data only
    //gpsSerial->println(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    // Update once per second (1Hz)
    // gpsSerial->println(PMTK_SET_NMEA_UPDATE_1HZ);

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

    pinMode(12, OUTPUT); // DIR
    digitalWrite(12, HIGH);
    analogWrite(5, 0);
    rudder->attach(9);
    Serial.begin(115200);
}

void Ship::loop() {
    //handleGPS();
    XBee.tick();
}

void Ship::handleGPS() {
    while(gpsSerial->available()) {
       if(gps->encode(gpsSerial->read())) {
           gps->f_get_position(&latitude, &longitude, &fixAge);
           speed = gps->f_speed_knots();
        }
    }
}

void Ship::handleRx(BeePointerFrame *frame) {
    switch(frame->data[0]) {
        case CaptainFrames::CDT:
            thrustDirection = frame->data[1] == 'F';
            digitalWrite(12, thrustDirection ? HIGH : LOW);
            direction = (uint8_t)frame->data[2];
            rudder->write(map(direction, 0, 255, 80, 150));
            thrust = (uint8_t)frame->data[3];
            analogWrite(5, thrust);
            break;
    }
}

// Set to fire every 1ms
void Ship::handleInterrupt() {
    interruptCount++;
    // This will be true every 1 second
    if(interruptCount > 999) {
        interruptCount = 0;

        // Push GPS data
        //char gps[2 + (sizeof(float) * 3)];
        // No GPS for now :(
        // Just using this for an activity ping
        char gps[] = { CaptainFrames::GPS,
            fixAge == TinyGPS::GPS_INVALID_AGE ? 'V' : 'A',
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00 };
        //gps[0] = CaptainFrames::GPS;
        //gps[1] = fixAge == TinyGPS::GPS_INVALID_AGE ? 'V' : 'A';
        //memcpy(gps + 2, &latitude, sizeof(float));
        //memcpy(gps + 2 + sizeof(float), &longitude, sizeof(float));
        //memcpy(gps + 2 + (sizeof(float) * 2), &speed, sizeof(float));
        XBee.sendData(gps, sizeof gps);

    }

    TCNT2 = 130; // Reset timer to 130 of 255
    TIFR2 = 0; // Clear overflow flag
}

ISR(TIMER2_OVF_vect) {
    shipInstance->handleInterrupt();
}

void xbeeCallback(BeePointerFrame *frame) {
    shipInstance->handleRx(frame);
}
