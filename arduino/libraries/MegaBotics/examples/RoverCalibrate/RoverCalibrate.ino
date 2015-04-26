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
 
#include <MegaBotics.h>

#define ROVER_CALIBRATE 1

#if ROVER_CALIBRATE

#define STEER_CHANNEL		1		// Channel used for Steering
#define THROTTLE_CHANNEL	2		// Channel used for Throttle
#define WHEEL_ENCODER_CHANNEL	6		// Channel used for wheel encoder

#define SERVO_MAX_MICRO_SECS	2000
#define SERVO_MIN_MICRO_SECS	1000

PwmIn & steerIn = PwmIn::getReference(STEER_CHANNEL);
PwmIn & throttleIn = PwmIn::getReference(THROTTLE_CHANNEL);

WheelEncoder & encoder = WheelEncoder::getReference();
PwmMux & pwmMux = PwmMux::getReference();
Logger & logger = Logger::getReference();
InputPanel inputPanel;
UPort uPort(1);

Rover::Config config;

void printConfig() {
	logger.begin(Logger::LEVEL_DEBUG, F("CONFIG")).
			nv(F("steerChannel"), config.steerChannel).
			nv(F("throttleChannel"), config.throttleChannel).
			nv(F("steerMin"), config.steerMin).
			nv(F("steerMid"), config.steerMid).
			nv(F("steerMax"), config.steerMax).
			nv(F("idlePwm"), config.idlePwm).
			nv(F("fwdPwmMin"), config.fwdPwmMin).
			nv(F("fwdPwmMax"), config.fwdPwmMax).
			nv(F("revPwmMin"), config.revPwmMin).
			nv(F("revPwmMax"), config.revPwmMax);
	logger.endln().flush();
}

void configThrottle() {
	uPort.serial().println("Turn OFF the remote and press the button");
	inputPanel.waitForPush();
	inputPanel.clear();

	config.idlePwm = throttleIn.getPulseWidth();

	uPort.serial().println("Turn ON the remote and press the button");
	inputPanel.waitForPush();
	inputPanel.clear();

	config.fwdPwmMin = throttleIn.getPulseWidth();

	uPort.serial().println("Press the throttle to go at max speed, 1st forward and then reverse, press the button when done");
	throttleIn.reset();
	inputPanel.clear();

	while (!inputPanel.clicked()) {
		delay(200);
	}

	config.fwdPwmMax = throttleIn.getMaxPulseWidth();
	config.revPwmMax = throttleIn.getMinPulseWidth();
	config.revPwmMin = config.idlePwm - 100;
}

void configSteer() {
	config.steerMid = map(steerIn.getPulseWidth(), SERVO_MIN_MICRO_SECS, SERVO_MAX_MICRO_SECS, 0, 180);
	config.steerMin = 0;
	config.steerMax = 180;
}

void setup() {
	uPort.serial().begin(115200);

	throttleIn.setName("THROTTLE");
	steerIn.setName("STEER");
	encoder.setup();
	loggerSetup();

	pwmMux.setup();
	pwmMux.setMode(PwmMux::PWMIN);

	config.steerChannel = STEER_CHANNEL;
	config.throttleChannel = THROTTLE_CHANNEL;

	configThrottle();
	configSteer();
	printConfig();
}

void loop() {
}

void loggerSetup(void) {
	Logger::Config config;
	config.bufsize = 128;
	config.host = "";
	config.port = 0;
	logger.setup(config);
	logger.autoFlush(true);
	logger.enable(Logger::LOG_SERIAL);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);
}

#endif
