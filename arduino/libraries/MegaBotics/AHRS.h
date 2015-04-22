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
 * Module:		AHRS.h - The Automated Heading and Reference System
 * 						 Supports Razor IMU. Support for other IMUs is planned.
 * Created on:	Apr 15, 2015
 * Author:		Aakash Sahai
 */

#ifndef MEGABOTICS_AHRS_H_
#define MEGABOTICS_AHRS_H_

#include <String.h>
#include <Uport.h>
#include <Print.h>

#define AHRS_DEFAULT_UPORT		2
#define AHRS_DEFAULT_BAUD		57600L

class AHRS {
public:
	enum Status {
		SUCCESS = 0,
		FAILURE = 1,
		TIMEDOUT = 2,
		BUSY = 3,
		NOT_CONNECTED = 4
	};

	enum Mode {
		SINGLE = 0,
		STREAMING = 1
	};

	enum DataType {
		YAW_PITCH_ROLL = 0,
		SENSOR_CALIBRATED = 1,
		SENSOR_RAW = 2,
		SENSOR_BOTH = 3
	};

	typedef struct YPR : Printable {
		float	yaw;
		float	pitch;
		float	roll;
		long	millis;

		virtual size_t printTo(Print& p) const;
	} YPR;

	typedef struct AMG : Printable {
		float	accel[3];
		float	mag[3];
		float	gyro[3];
		long	millis;

		virtual size_t printTo(Print& p) const;
	} AMG;

	AHRS();
	AHRS(int port);
	virtual ~AHRS();
	Status setup(void);

	Status setMode(Mode mode);
	Status setDataType(DataType dataType);
	Status setErrorOutput(bool tf);
	Status setBaud(long baud);

	YPR & getYPR(void);
	YPR & getRelativeYPR(void);
	AMG & getRawAMG(void);
	AMG & getCalibratedAMG(void);
	UPort & getUPort(void) { return _uport; }

	void poll(void);
	void resetYPR(void);
	float getOrientation(void);
	static float normalize(float val);

private:
	UPort _uport;
	long _baud;
	YPR	_ypr;
	AMG	_rawAmg;
	AMG	_calAmg;
	YPR	_zeroYpr;
	YPR	_relativeYpr;

	YPR &fetchYPR(void);
	AMG &fetchRawSensor(void);
	AMG &fetchCalibratedSensor(void);
	void readFloats(char *buf, int n);
	static size_t print3Floats(Print& p, const float buf[], String type);
};

#endif /* MEGABOTICS_AHRS_H_ */
