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
 * Module:		GPS.h
 * Created on:	May 25, 2015
 * Author:		Srinivas Raj
 */

#ifndef MEGABOTICS_GPS_H_
#define MEGABOTICS_GPS_H_

#include <MegaBotics.h>
#include <TinyGPS++.h>

#define GPS_DEFAULT_UPORT		4
#define GPS_DEFAULT_BAUD		9600L

class GPS {
public:
	struct Config {
		long baud;
		byte port;

		Config() {
			baud = GPS_DEFAULT_BAUD;
			port = GPS_DEFAULT_UPORT;
		}
	};

	GPS();
	GPS(byte port);
	GPS(byte port, long baud);
	virtual ~GPS();

	void setup(void);
	void setup(Config &config);
	void collect();
	void display();

	double getLatitude();
	double getLongitude();
	float getSpeed();
	float getHeading();
	TinyGPSPlus getRawGps() {return _gps;}
	UPort & getUPort(void) { return _uport; }

	static GPS * getInstance() { return &_instance; }
	static GPS & getReference() { return _instance; }

private:
	static GPS _instance;

	UPort _uport;
	Config _config;
	TinyGPSPlus _gps;
};

#endif /* MEGABOTICS_GPS_H_ */
