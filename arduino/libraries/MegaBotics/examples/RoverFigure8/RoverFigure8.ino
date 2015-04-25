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
 * This is a sample example to drive Rover in figure 8. It doesn't necessarily function
 * properly - you need to find the issue and make it work.
 */
#include <Servo.h>
#include <MegaBotics.h>

#define ROVER_FIGURE_8	0

#if ROVER_FIGURE_8

UPort uPort(1);
HardwareSerial & serial = uPort.serial();
AHRS ahrs;
WheelEncoder &encoder = WheelEncoder::getReference();
Logger & logger = Logger::getReference();
Rover & rover = Rover::getReference();

void setup() {
	Logger::Config config;

	serial.begin(115200);
	rover.setup();
	ahrs.setup();
	encoder.setup();
	config.bufsize = 128;
	config.host = "";
	config.port = 0;


	logger.setup(config);
	logger.autoFlush(true);
	logger.enable(Logger::LOG_SERIAL);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);

	logger.start("Starting Rover log");
	figure8();
}

void checkKeyPress(void) {
	if (serial.available()) {
		int ch = serial.read();
		switch (ch) {
		case 'z': // Reset/zero the distance and heading
			encoder.reset();
			ahrs.resetYPR();
			break;
		}
	}
}

void poll() {
	serialEventRun();
	checkKeyPress();
	ahrs.poll();
	encoder.poll();
}

void turnAndMove(int tDeg, int tRots) {
	AHRS::YPR ypr;
	int rots, steer, throttle;
	ahrs.resetYPR();
	encoder.reset();
	rover.straight();
	for(;;) {
		log();
		poll();
		ypr = ahrs.getRelativeYPR();
		rots = encoder.getRevolutions();
		if (tDeg)
			steer = 100 - (int)(100.0 * ypr.yaw / tDeg);
		else
			steer = -ypr.yaw;
		if (tDeg)
			throttle = 100 - (int)(100.0 * rots / tRots);
		else
			throttle = 50;
		logger.begin(Logger::LEVEL_DEBUG, F("RUN")).
				nv(F("DEG"), tDeg).
				nv(F("STEER"), steer).
				nv(F("ROT"), tRots).
				nv(F("THROTTLE"), throttle).endln();
		rover.steer(steer);
		rover.throttle(throttle);

		if ((tDeg != 0) && abs(ypr.yaw - tDeg) < 10.0) {
			break;
		}
		if ((tRots != 0) && (tRots - rots) < 1) {
			break;
		}
	}

	rover.idle();
}

void log() {
	AHRS::YPR ypr = ahrs.getRelativeYPR();
	logger.begin(Logger::LEVEL_DEBUG, F("ROVER")).
		nv(F("REVS"), encoder.getRevolutions()).
		nv(F("RPS"), encoder.getRps()).
		nv(F("DISTANCE"), encoder.getDistance()).
		nv(F("SPEED"), encoder.getSpeed()).
		nv(F("REL YAW"), ypr.yaw).
		endln();
}

void loop() {
}

void figure8() {
	for (;;) {
		rover.straight();
		rover.idle();
		while (rover.getControlMode() == Rover::MANUAL) {
			serial.println("Waiting to Run");
			poll();
			delay(1000);
		}
		turnAndMove(120, 0);
		turnAndMove(0, 10);
		turnAndMove(-120, 0);
		turnAndMove(-120, 0);
		turnAndMove(0, 10);
		turnAndMove(120, 0);
		while (rover.getControlMode() != Rover::MANUAL) {
			serial.println("Toggle Control to run again");
			poll();
			delay(1000);
		}
	}
}
#endif
