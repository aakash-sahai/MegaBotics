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

#define PWM_TEST				1

#define PWMIN_CONTROL_CHANNEL	(5-1)		// Channel used for Auto/Manual control
#define WHEEL_ENCODER_CHANNEL	(6-1)		// Channel used for wheel encoder

#if PWM_TEST

UPort uPort(1);

PwmIn pwmIns[MAX_PWMIN_CHANNELS] = { PwmIn(), PwmIn(), PwmIn(), PwmIn(), PwmIn(), PwmIn() };
PwmMux pwmMux;

void setup() {
	uPort.serial().begin(115200);
	for (byte ch = 0; ch < MAX_PWMIN_CHANNELS; ch++) {
		pwmIns[ch].setup(ch+1);
	}

	pwmIns[WHEEL_ENCODER_CHANNEL].pullup();
	pwmMux.setup();
}

void loop() {
	char buf[80];
	static unsigned long oldIntrCount[MAX_PWMIN_CHANNELS] = { 0, 0, 0, 0, 0, 0 };
	for (byte ch = 0; ch < MAX_PWMIN_CHANNELS; ch++) {
		unsigned long intrCount = pwmIns[ch].intrCount();
		if (pwmIns[ch].isAliveSince()) {
			// minimum uSec - current uSec - maximum uSec - interrupt count - interrupt rate
			sprintf(buf, "[ CH%d: %d - %d - %d - %lu - %lu ] ", (int)(ch+1),
					pwmIns[ch].minimum(), pwmIns[ch].current(), pwmIns[ch].maximum(), pwmIns[ch].intrCount(),
					intrCount - oldIntrCount[ch]);
					oldIntrCount[ch] = intrCount;

		} else {
			sprintf(buf, "[ CH%d: DEAD ]", (int)(ch+1));
		}
		uPort.serial().print(buf);
	}
	if (pwmIns[PWMIN_CONTROL_CHANNEL].current() < 1400) {
		pwmMux.setMode(PWMIN);
		uPort.serial().println(" MANUAL ");
	} else {
		pwmMux.setMode(PROGRAM);
		uPort.serial().println(" AUTOPILOT ");
	}

	delay(1000);
}

#endif
