/*
 * RoverConfig.cpp
 *
 *  Created on: Jun 13, 2015
 *      Author: aakash
 */

#include "ConfigManager.h"

ConfigManager ConfigManager::_instance;

ConfigManager::ConfigManager() {
}

ConfigManager::~ConfigManager() {
}

void ConfigManager::setup(void) {
	ConfigParser::Config configFile;

	struct ConfigParser::Section stcSection("STEERING_PID", (char *)&steeringPidConfig);
	configFile.addSection(&stcSection);
	struct ConfigParser::Param stcP1("Kp", 'F', offsetof(struct PID::Config, Kp));
	stcSection.addParam(&stcP1);
	struct ConfigParser::Param stcP2("Ki", 'F', offsetof(struct PID::Config, Ki));
	stcSection.addParam(&stcP2);
	struct ConfigParser::Param stcP3("Kd", 'F', offsetof(struct PID::Config, Kd));
	stcSection.addParam(&stcP3);
	struct ConfigParser::Param stcP4("scale", 'F', offsetof(struct PID::Config, scale));
	stcSection.addParam(&stcP4);
	struct ConfigParser::Param stcP5("clamp", 'F', offsetof(struct PID::Config, clamp));
	stcSection.addParam(&stcP5);
	struct ConfigParser::Param stcP6("idleTime", 'I', offsetof(struct PID::Config, idleTime));
	stcSection.addParam(&stcP6);

	struct ConfigParser::Section gpsSection("GPS", (char *)&gpsConfig);
	configFile.addSection(&gpsSection);
	struct ConfigParser::Param gpsP1("port", 'I', offsetof(struct GPS::Config, port));
	gpsSection.addParam(&gpsP1);
	struct ConfigParser::Param gpsP2("baud", 'I', offsetof(struct GPS::Config, baud));
	gpsSection.addParam(&gpsP2);
	struct ConfigParser::Param gpsP3("interruptRead", 'I', offsetof(struct GPS::Config, interruptRead));
	gpsSection.addParam(&gpsP3);

	struct ConfigParser::Section throttleSection("THROTTLE", (char *)&throttleConfig);
	configFile.addSection(&throttleSection);
	struct ConfigParser::Param throttleP1("minimum", 'I', offsetof(struct Rover::ThrottleConfig, minimum));
	throttleSection.addParam(&throttleP1);
	struct ConfigParser::Param throttleP2("maximum", 'I', offsetof(struct Rover::ThrottleConfig, maximum));
	throttleSection.addParam(&throttleP2);
	struct ConfigParser::Param throttleP3("turn", 'I', offsetof(struct Rover::ThrottleConfig, turn));
	throttleSection.addParam(&throttleP3);

	struct ConfigParser::Section routeSection("ROUTE", (char *)&routeConfig);
	configFile.addSection(&routeSection);
	struct ConfigParser::Param routeP1("proximRadius", 'I', offsetof(struct Route::Config, proximRadius));
	routeSection.addParam(&routeP1);
	struct ConfigParser::Param routeP2("refHeading", 'F', offsetof(struct Route::Config, refHeading));
	routeSection.addParam(&routeP2);

	struct ConfigParser::Section autoRoverSection("AUTO_ROVER", (char *)&autoRoverConfig);
	configFile.addSection(&autoRoverSection);
	struct ConfigParser::Param autoRoverP1("logging", 'I', offsetof(struct AutonomousRover::Config, logging));
	autoRoverSection.addParam(&autoRoverP1);
	struct ConfigParser::Param autoRoverP2("cruiseDistance", 'I', offsetof(struct AutonomousRover::Config, cruiseDistance));
	autoRoverSection.addParam(&autoRoverP2);
	struct ConfigParser::Param autoRoverP3("navLoopDelay", 'I', offsetof(struct AutonomousRover::Config, navLoopDelay));
	autoRoverSection.addParam(&autoRoverP3);
	struct ConfigParser::Param autoRoverP4("gpsStaleThres", 'I', offsetof(struct AutonomousRover::Config, gpsStaleThres));
	autoRoverSection.addParam(&autoRoverP4);
	struct ConfigParser::Param autoRoverP5("waypointTooFarThres", 'I', offsetof(struct AutonomousRover::Config, waypointTooFarThres));
	autoRoverSection.addParam(&autoRoverP5);

	load((char *)"CONFIG.TXT", configFile);
}

void ConfigManager::load(char *fileName, ConfigParser::Config & configFile) {
	char buf[513];
	ConfigParser parser;
	char *err;
	int	len;
	Display & display = Display::getReference();
	SDCard::getReference().setup();

	display.clearScr();
	display.print(F("Reading Configuration file: "));
	display.println(fileName);
	File file = SD.open(fileName);
	if (!file) {
		display.println(F("Failed to open file"));
		return;
	}

	len = file.read(buf, 512);
	buf[len] = 0;
	if (len <=0) {
		display.print(F("Failed to read file"));
		return;
	}

	err = parser.parse(configFile, buf);
	if (err != 0) {
		display.print(F("Failed to parse file"));
		return;
	}

	file.close();

	display.println(F("Configuration loaded"));
	return ;
}
