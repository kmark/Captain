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

#include "CaptainBase.h"

// Completely isolate all functionality within a class
Base base;

void setup() {
    base.setup();
}

void loop() {
    base.loop();
}
