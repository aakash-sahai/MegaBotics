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
 * This is a sample example to demonstrate the working of EEPROM Store. It loads and stores the
 * Logger Config.
 */
#include <Servo.h>
#include <MegaBotics.h>

#define EEPROM_STORE_TEST	0

#if EEPROM_STORE_TEST

UPort uPort(1);
HardwareSerial &serial = uPort.serial();
Logger & logger = Logger::getReference();
EepromStore & estore = EepromStore::getReference();

void setup() {
	Logger::Config config;
	bool doStore = false;

	serial.begin(115200);
	estore.setup();

	int len = sizeof(config);
	EepromStore::Status status = estore.loadSection("LOGR", &config, &len);

	if (status != EepromStore::SUCCESS) {
		serial.print(F("Could not load Logger config; Status = "));
		serial.println((int)status);
		doStore = true;
	}

	if (len != sizeof(config)) {
		serial.print(F("Configuration size mismatch for LOGR: Got "));
		serial.print(len);
		serial.print(" vs Expected ");
		serial.println(sizeof(config));
		doStore = true;
	}

	if (doStore) {
		config.bufsize = 128;
		strcpy(config.ip, "192.168.4.2");
		config.port = 0;
		status = estore.storeSection("LOGR", &config, sizeof(config));
		if (status != EepromStore::SUCCESS) {
			serial.println(F("Could not store Logger config; Status = "));
			serial.println((int)status);
		}
	}

	logger.setup(config);
	logger.enable(Logger::LOG_SERIAL);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_INFO);

	logger.begin(Logger::LEVEL_INFO, F("CONFIG")).
			nv(F("BUFSIZE"), config.bufsize).
			nv(F("HOST"), config.ip).
			nv(F("PORT"), config.port).
			endln().flush();


	serial.println(F("Free Blocks in EEPROM: "));
	estore.listFree();
	serial.println(F("Occupied Blocks in EEPROM: "));
	estore.list();
}

void loop() {
}

#endif
