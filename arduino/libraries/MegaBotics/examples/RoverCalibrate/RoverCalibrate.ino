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
#define CONTROL_CHANNEL		1		// Channel used to switch to manual mode
#define THROTTLE_CHANNEL	2		// Channel used for Throttle
#define WHEEL_ENCODER_CHANNEL	6		// Channel used for wheel encoder

PwmIn & steerIn = PwmIn::getReference(STEER_CHANNEL);
PwmIn & throttleIn = PwmIn::getReference(THROTTLE_CHANNEL);

WheelEncoder & encoder = WheelEncoder::getReference();
PwmMux & pwmMux = PwmMux::getReference();
Logger & logger = Logger::getReference();
InputPanel inputPanel;
UPort uPort(1);
EepromStore & estore = EepromStore::getReference();

Rover::Config config;


void printConfig() {
	logger.begin(Logger::LEVEL_DEBUG, F("CONFIG")).
			nv(F("steerChannel"), config.steerChannel).
			nv(F("throttleChannel"), config.throttleChannel).
			nv(F("controlChannel"), config.controlChannel).
			nv(F("steerMin"), config.steerMin).
			nv(F("steerMid"), config.steerMid).
			nv(F("steerMax"), config.steerMax).
			nv(F("steerPwmMin"), config.steerPwmMin).
			nv(F("steerPwmMin"), config.steerPwmMid).
			nv(F("steerPwmMax"), config.steerPwmMax).
			nv(F("idlePwm"), config.idlePwm).
			nv(F("fwdPwmMin"), config.fwdPwmMin).
			nv(F("fwdPwmMax"), config.fwdPwmMax).
			nv(F("revPwmMin"), config.revPwmMin).
			nv(F("revPwmMax"), config.revPwmMax);
	logger.endln().flush();
}

void storeConfig() {
	EepromStore::Status status = estore.storeSection(ROVER_CONFIG_NAME, &config, sizeof(config));
	if (status != EepromStore::SUCCESS) {
		uPort.serial().println(F("Could not store Rover Config; Status = "));
		uPort.serial().println((int)status);
	} else {
		uPort.serial().println(F("Configuration stored to Eeprom Store"));
	}
}

void captureConfigValues() {
	uPort.serial().println("Turn OFF the remote and press the button");
	inputPanel.waitForPush();
	config.idlePwm = throttleIn.getPulseWidth();
	uPort.serial().print("idlePwm = "); uPort.serial().println(config.idlePwm);

	uPort.serial().println("Turn ON the remote and press the button");
	steerIn.reset();
	inputPanel.waitForPush();
	config.steerPwmMid = steerIn.getPulseWidth();
	uPort.serial().print("steerPwmMid = "); uPort.serial().println(config.steerPwmMid);

	uPort.serial().println("Steer left and right, press the button when done");
	steerIn.reset();
	inputPanel.waitForPush();
	config.steerPwmMin = steerIn.getMinPulseWidth();
	config.steerPwmMax = steerIn.getMaxPulseWidth();
	uPort.serial().print("steerPwmMin = "); uPort.serial().println(config.steerPwmMin);
	uPort.serial().print("steerPwmMax = "); uPort.serial().println(config.steerPwmMax);

	config.steerMid = map(steerIn.getPulseWidth(), config.steerPwmMin, config.steerPwmMax, 0, 180);
	config.steerMin = 0;
	config.steerMax = 180;
	uPort.serial().print("steerMid = "); uPort.serial().println(config.steerMid);

	uPort.serial().println("Press the throttle to go at max speed, 1st forward and then reverse, press the button when done");
	throttleIn.reset();
	inputPanel.waitForPush();

	config.fwdPwmMax = throttleIn.getMaxPulseWidth();
	config.revPwmMax = throttleIn.getMinPulseWidth();
	uPort.serial().print("fwdPwmMax = "); uPort.serial().println(config.fwdPwmMax);
	uPort.serial().print("revPwmMax = "); uPort.serial().println(config.revPwmMax);

	// configure the min values
	uPort.serial().println("Go forward at min speed using the remote, press the button while still keeping the throttle at min. speed");
	throttleIn.reset();
	inputPanel.waitForPush();
	config.fwdPwmMin = throttleIn.getPulseWidth();
	uPort.serial().print("fwdPwmMin = "); uPort.serial().println(config.fwdPwmMin);

	uPort.serial().println("Go reverse at min speed using the remote, press the button while still keeping the throttle at min. speed");
	throttleIn.reset();
	inputPanel.waitForPush();
	config.revPwmMin = min(throttleIn.getPulseWidth(), config.idlePwm - 100);
	uPort.serial().print("revPwmMin = "); uPort.serial().println(config.revPwmMin);
}

void loggerSetup(void) {
	logger.setup();
	logger.autoFlush(true);
	logger.enable(Logger::LOG_SERIAL);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);
}

void setup() {
	uPort.serial().begin(115200);

	throttleIn.setName("THROTTLE");
	steerIn.setName("STEER");
	encoder.setup();
	estore.setup();

	loggerSetup();

	pwmMux.setup();
	pwmMux.setMode(PwmMux::PWMIN);

	config.steerChannel = STEER_CHANNEL;
	config.throttleChannel = THROTTLE_CHANNEL;
	config.controlChannel = CONTROL_CHANNEL;

	captureConfigValues();
	printConfig();

	uPort.serial().println("Press the button to store the config");
	inputPanel.waitForPush();
	storeConfig();
}

void loop() {
}

#endif
