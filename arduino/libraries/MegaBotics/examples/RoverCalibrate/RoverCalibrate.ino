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

#define ROVER_CALIBRATE 0

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
Display & display = Display::getReference();
InputPanel inputPanel;
UPort uPort(1);
EepromStore & estore = EepromStore::getReference();

Rover::Config config;

void clearAndPrint(const __FlashStringHelper *msg) {
	uPort.serial().print(msg);
	display.clearScr();
	display.print(msg);
}

void print(String msg) {
	uPort.serial().print(msg);
	display.print(msg);
}

void println(String msg) {
	uPort.serial().println(msg);
	display.println(msg);
}

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
		clearAndPrint(F("Could not store Rover Config; Status = "));
		println(String((int)status));
	} else {
		clearAndPrint(F("Configuration stored to Eeprom Store\r\n"));
	}
}

void captureConfigValues() {
	const __FlashStringHelper *msg;

	msg = F("Turn OFF the remote and press the button\r\n");
	clearAndPrint(msg);
	inputPanel.waitForClick();
	config.idlePwm = throttleIn.getPulseWidth();


	msg = F("Turn ON the remote and press the button\r\n");
	clearAndPrint(msg);
	steerIn.reset();
	inputPanel.waitForClick();
	config.steerPwmMid = steerIn.getPulseWidth();


	msg = F("Steer left and right, press the button when done\r\n");
	clearAndPrint(msg);
	steerIn.reset();
	inputPanel.waitForClick();
	config.steerPwmMin = steerIn.getMinPulseWidth();
	config.steerPwmMax = steerIn.getMaxPulseWidth();
	config.steerMid = map(steerIn.getPulseWidth(), config.steerPwmMin, config.steerPwmMax, 0, 180);
	config.steerMin = 0;
	config.steerMax = 180;


	msg = F("Press the throttle to go at max speed, 1st forward and then reverse, press the button when done\r\n");
	clearAndPrint(msg);
	throttleIn.reset();
	inputPanel.waitForClick();
	config.fwdPwmMax = throttleIn.getMaxPulseWidth();
	config.revPwmMax = throttleIn.getMinPulseWidth();


	// configure the min values
	msg = F("Go forward at min speed using the remote, press the button while still keeping the throttle at min. speed\r\n");
	clearAndPrint(msg);
	throttleIn.reset();
	inputPanel.waitForClick();
	config.fwdPwmMin = throttleIn.getPulseWidth();


	msg = F("Go reverse at min speed using the remote, press the button while still keeping the throttle at min. speed\r\n");
	clearAndPrint(msg);
	throttleIn.reset();
	inputPanel.waitForClick();
	config.revPwmMin = min(throttleIn.getPulseWidth(), config.idlePwm - 100);

}

void displayConfig() {
	clearAndPrint(F("The final configuration is\r\n"));
	print("steerPwmMin = "); println(String(config.steerPwmMin));
	print("steerPwmMid = "); println(String(config.steerPwmMid));
	print("steerPwmMax = "); println(String(config.steerPwmMax));
	print("steerMin = "); println(String(config.steerMin));
	print("steerMid = "); println(String(config.steerMid));
	print("steerMax = "); println(String(config.steerMax));
	print("idlePwm = "); println(String(config.idlePwm));
	print("revPwmMin = "); println(String(config.revPwmMin));
	print("revPwmMax = "); println(String(config.revPwmMax));
	print("fwdPwmMin = "); println(String(config.fwdPwmMin));
	print("fwdPwmMax = "); println(String(config.fwdPwmMax));
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

	Display::Config dispConfig;
	dispConfig.rotation = 2;
	dispConfig.textSize= 2;
	display.setup(dispConfig);

	display.reset();

	captureConfigValues();
	printConfig();

	const __FlashStringHelper *msg = F("Press the button to store the config\r\n");
	clearAndPrint(msg);
	inputPanel.waitForClick();
	storeConfig();
	displayConfig();
}


void loop() {
}

#endif
