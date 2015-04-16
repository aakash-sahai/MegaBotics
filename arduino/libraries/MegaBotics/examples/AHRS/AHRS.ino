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
 * An example sketch to test AHRS module.
 */
#include <Servo.h>
#include <MegaBotics.h>

#define AHRS_TEST 	1
#define GOOGLE_TEST	0

#if AHRS_TEST

#define AHRS_UPORT		2
#define TERMINAL_UPORT	1

UPort terminalPort(TERMINAL_UPORT);

AHRS ahrs(AHRS_UPORT);

void setup() {
	terminalPort.serial().begin(115200);
	ahrs.setup();
}

void loop() {
	ahrs.poll();

	if (terminalPort.serial().available()) {
		int ch = terminalPort.serial().read();
		switch(ch) {
		case 'y':	// Get Yaw/Pitch/Roll data
		{
			AHRS::ypr &_ypr = ahrs.getYPR();
			terminalPort.serial().print(_ypr);
			break;
		}
		case 'c':	// Get calibrated sensor data
		{
			AHRS::amg & _amg = ahrs.getCalibratedAMG();
			terminalPort.serial().print(_amg);
			break;
		}
		case 'r':	// Get raw sensor data
		{
			AHRS::amg & _amg = ahrs.getRawAMG();
			terminalPort.serial().print(_amg);
			break;
		}
		case 'x':
			cross();
			break;
		}
	}
}

void cross(void) {
	ahrs.getUPort().cross(terminalPort);
	terminalPort.interact();
	ahrs.getUPort().uncross();
}

#endif
