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
 * Module:		PwmIn.h - The decoder for 6 channels of PWM Input (e.g. from RC receiver)
 * Created on:	Mar 30, 2015
 * Author:		Aakash Sahai
 */

#ifndef MEGABOTICS_PWMIN_H_
#define MEGABOTICS_PWMIN_H_

#include <Arduino.h>

#define MAX_PWMIN_CHANNELS	6

class PwmIn {
public:
	virtual ~PwmIn();

	static void setup();
	static void reset();
	static byte getPin(byte channel) { return _pinMappings[channel-1]; }
	static byte getPcint(byte channel) { return _pcintMappings[channel-1]; }
	static int current(byte channel) { return _pwmCurrent[channel-1]; }
	static int minimum(byte channel) { return _pwmMin[channel-1]; }
	static int maximum(byte channel) { return _pwmMax[channel-1]; }

	static unsigned long _prevTime[MAX_PWMIN_CHANNELS];
	static unsigned int _pwmCurrent[MAX_PWMIN_CHANNELS];
	static unsigned int _pwmMin[MAX_PWMIN_CHANNELS];
	static unsigned int _pwmMax[MAX_PWMIN_CHANNELS];
	static const byte _pinMappings[MAX_PWMIN_CHANNELS];
	static const byte _pcintMappings[MAX_PWMIN_CHANNELS];

	static int _intCount;

private:
	PwmIn();
	static void init(void);
	static bool _initialized;
};

#endif /* MEGABOTICS_PWMIN_H_ */
