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

#define PWM_TEST				0

#define PWMIN_STEER_CHANNEL		1		// Channel used for Steering
#define PWMIN_THROTTLE_CHANNEL	2		// Channel used for Throttle
#define PWMIN_CONTROL_CHANNEL	5		// Channel used for Auto/Manual control
#define WHEEL_ENCODER_CHANNEL	6		// Channel used for wheel encoder

#if PWM_TEST

PwmIn & steer = PwmIn::getReference(PWMIN_STEER_CHANNEL);
PwmIn & throttle = PwmIn::getReference(PWMIN_THROTTLE_CHANNEL);
PwmIn & control = PwmIn::getReference(PWMIN_CONTROL_CHANNEL);
WheelEncoder & encoder = WheelEncoder::getReference();
PwmMux & pwmMux = PwmMux::getReference();
Logger & logger = Logger::getReference();

void setManual(unsigned int val) {
	pwmMux.setMode(PwmMux::PWMIN);
}

void setAuto(unsigned int val) {
	pwmMux.setMode(PwmMux::PROGRAM);
}

void checkChannel(PwmIn &chan) {
	logger.begin(Logger::LEVEL_DEBUG, F("PWMIN")).
			nv(F("NAME"), (char *)chan.name().c_str());
	if (chan.isAliveSince()) {
			logger.nv(F("MIN"), chan.getMinPulseWidth()).
			nv(F("CURR"), chan.getPulseWidth()).
			nv(F("MAX"), chan.getMaxPulseWidth());
	} else {
		logger.nv(F("STATE"), '*');
	}
	logger.space();
}

void checkEncoder(void) {
	encoder.poll();
	logger.begin(Logger::LEVEL_DEBUG, F("ENCODER")).
			nv(F("REVS"), encoder.getRevolutions()).
			nv(F("DISTANCE"), encoder.getDistance()).
			nv(F("SPEED"), encoder.getSpeed()).
			space();
}

void loggerSetup(void) {
	logger.setup();
	logger.autoFlush(true);
	logger.enable(Logger::LOG_SERIAL);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);
}

void setup() {
	UPort uPort(1);
	uPort.serial().begin(115200);

	throttle.setName("THROTTLE");
	steer.setName("STEER");
	encoder.setup();

	loggerSetup();

	/*
	 * Uncomment the following three lines to use the RC receiver channel 5 to
	 * control the auto/manual mode.
	 * 		control.setName("CONTROL");
	 * 		control.onBelowThreshold(1200, setManual);
	 * 		control.onAboveThreshold(1800, setAuto);
	 */
	pwmMux.setup();
	pwmMux.setMode(PwmMux::PWMIN);
}

void loop() {
	const char * mode;
	checkChannel(throttle);
	checkChannel(steer);
	checkChannel(control);
	mode = (pwmMux.getMode() == PwmMux::PWMIN) ? " MANUAL " : " AUTO";
	logger.nv(F("PWMMUX"), mode);
	checkEncoder();
	logger.endln().flush();
	delay(1000);
}

#endif
