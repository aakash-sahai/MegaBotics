/*
 * RoverConfig.h
 *
 *  Created on: Jun 13, 2015
 *      Author: aakash
 */

#ifndef MEGABOTICS_CONFIGMANAGER_H_
#define MEGABOTICS_CONFIGMANAGER_H_

#include "MegaBotics.h"
#include "AutonomousRover.h"

class ConfigManager {
public:
	ConfigManager();
	virtual ~ConfigManager();
	void setup(void);

	GPS::Config gpsConfig;
	PID::Config steeringPidConfig;
	Rover::ThrottleConfig throttleConfig;
	AutonomousRover::Config autoRoverConfig;

	static ConfigManager * getInstance() { return &_instance; }
	static ConfigManager & getReference() { return _instance; }

private:
	static ConfigManager _instance;

	void load(char *fileName, ConfigParser::Config & configFile);
};

#endif /* MEGABOTICS_CONFIGMANAGER_H_ */
