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
 *		may be used to endorse or promote products derived from this software
 *		without specific prior written permission.
 */

/*
 * BaseRover.cpp
 *
 *  Created on: Jun 14, 2015
 *      Author: Aakash Sahai
 */

#include "AutonomousRover.h"
#include "ConfigManager.h"

AutonomousRover::AutonomousRover() {
	_rover = Rover::getInstance();
	_logger = Logger::getInstance();
	_ahrs = new AHRS();
	_display = Display::getInstance();
	_estore = EepromStore::getInstance();
	_joystick = JoyStick::getInstance();
	_route = Route::getInstance();
	_steeringPid = NULL;
}

void AutonomousRover::setup() {
	setup(_config);
}

/*
 * Setup order is critical.
 * First EEPROM is setup to allow other components to read configuration stored in EEPROM
 * Display is setup next to allow error displays
 * ConfigManager is setup next to read any configuration from SD card, including the AutonomousRover configuration
 * Logger is setup next to allow logging of messages from other setups
 * Other setups follow
 * The route setup is called the last as it may prompt the user to manually configure waypoints
 */
void AutonomousRover::setup(AutonomousRover::Config& config) {
	ConfigManager * cfgMgr = ConfigManager::getInstance();

	_config = config;
	_estore->setup();
	_display->setup();
	cfgMgr->setup();
	_config = cfgMgr->autoRoverConfig;
	_throttleConfig = cfgMgr->throttleConfig;
	if (_config.logging) {
		setupLogging();
	}
	_rover->setup();
	_ahrs->setup();
	_rover->setControlMode(Rover::MANUAL);
	_steeringPid = new PID(cfgMgr->steeringPidConfig);
	_route->setup();
}

AutonomousRover::~AutonomousRover() {
	// Nothing to do
}

void AutonomousRover::setupLogging() {
	Logger::Config config;
	config.fileName = "LOG.TXT";
	_logger->setup(config);

	_logger->autoFlush(false);
	_logger->enable(Logger::LOG_SERIAL);
	_logger->enable(Logger::LOG_SD);
	_logger->setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);
	_logger->setLevel(Logger::LOG_SD, Logger::LEVEL_DEBUG);
}

void AutonomousRover::waitToStart() {
	_joystick->clear();
	_display->clearScr();
	_ahrs->resetIMU();
	_ahrs->resetYPR();
	while (_joystick->waitForInput(10) == JoyStick::NONE) {
		_route->updateLocation(0);
		_display->reset();
		_display->print("YAW: ");_display->println(_ahrs->getOrientation());
		_route->updateLocation(0);
		_route->display();
		_display->println(F("Press the button to start"));
	}
	_display->clearScr();
}
