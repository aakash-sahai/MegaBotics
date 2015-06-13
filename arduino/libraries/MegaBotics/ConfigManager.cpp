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
	stcSection.addParam(&stcP5);

	struct ConfigParser::Section gpsSection("GPS", (char *)&gpsConfig);
	configFile.addSection(&gpsSection);
	struct ConfigParser::Param gpsP1("port", 'I', offsetof(struct GPS::Config, port));
	gpsSection.addParam(&gpsP1);
	struct ConfigParser::Param gpsP2("baud", 'I', offsetof(struct GPS::Config, baud));
	gpsSection.addParam(&gpsP2);

	load((char *)"CONFIG.TXT", configFile);
}

void ConfigManager::load(char *fileName, ConfigParser::Config & configFile) {
	char buf[513];
	ConfigParser parser;
	char *err;
	int	len;
	Display & display = Display::getReference();

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
