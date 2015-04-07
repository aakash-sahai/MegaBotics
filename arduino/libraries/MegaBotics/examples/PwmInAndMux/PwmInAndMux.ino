/*
 * This file is part of the support libraries for the Arduino based MegaBotics
 * platform.
 *
 * Copyright (c) 2015, Aakash Sahai and other contributors, a list of which is
 * maintained in the associated repository. All rights reserved.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *    + Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    + Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    + Neither the name of the copyright holder nor the names of its contributors
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
 */

/*
 * An example sketch to test the PWM Input and Multiplexer module.
 */
#include <Servo.h>
#include <MegaBotics.h>

#define PWM_TEST	0

#define PWMIN_CONTROL_CHANNEL	5

#if PWM_TEST

UPort uPort;
HardwareSerial * serial;

void setup() {
	uPort = UPort(1);
	serial = uPort.serial();
	serial->begin(115200);
	PwmIn::setup();
	PwmMux::setup();
}

void loop() {
	char buf[64];
	for (byte ch = 1; ch <= MAX_PWMIN_CHANNELS; ch++) {
		sprintf(buf, "[ CH%d: %d - %d - %d ] ", (int)ch,
				PwmIn::minimum(ch), PwmIn::current(ch), PwmIn::maximum(ch));
		serial->print(buf);
	}
	serial->println("");
	if (PwmIn::current(PWMIN_CONTROL_CHANNEL) < 1400) {
		serial->println("Switching to Manual mode");
		PwmMux::setMode(PWMIN);
	} else {
		serial->println("Switching to Autopilot mode");
		PwmMux::setMode(PROGRAM);
	}

	delay(1000);
}

#endif
