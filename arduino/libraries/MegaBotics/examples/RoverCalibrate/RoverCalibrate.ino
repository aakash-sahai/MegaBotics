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
 * The calibration routine for calibrating the Rover. The details of the setup
 * and instructions can be found on the project Wiki.
 */
#include <Servo.h>
#include <MegaBotics.h>

#define ROVER_CALIBRATE 1

#if ROVER_CALIBRATE

UPort uPort(1);
HardwareSerial &serial = uPort.serial();

int steerVal = 90;
int throttleVal = 1400;
bool calibrateSteer = true;

const int BUTTON_UP_PIN = 2;
const int BUTTON_DOWN_PIN = 3;

PushButton up;
PushButton down;
Rover & rover = Rover::getReference();

void setup() {
	up = PushButton(BUTTON_UP_PIN);
	down = PushButton(BUTTON_DOWN_PIN);

	serial.begin(9600);
	serial.println("Click UP button to calibrate steering or DOWN to throttle.");
	rover.setup();
	selectMode();
	serial.println("Press UP/DOWN Button and note down the various values.");
}

void selectMode() {
	while (true) {
		up.check();
		down.check();
		if (up.clicked()) {
			calibrateSteer = true;
			break;
		}
		if (down.clicked()) {
			calibrateSteer = false;
			break;
		}
	}
	serial.print("Calibrating ");
	serial.println(calibrateSteer ? "Steering" : "Throttle");
}

void loop() {
	up.check();
	down.check();
	if (up.pressed()) {
		if (calibrateSteer)
			steerVal += 1;
		else
			throttleVal += 1;
		serial.print("Value: ");
		serial.println(calibrateSteer ? steerVal : throttleVal);
	}

	if (down.pressed()) {
		if (calibrateSteer)
			steerVal -= 1;
		else
			throttleVal -= 1;
		serial.print("Value: ");
		serial.println(calibrateSteer ? steerVal : throttleVal);
	}

	if (calibrateSteer)
		rover.steerRaw(steerVal);
	else
		rover.throttleRaw(throttleVal);

	delay(100);
}
#endif
