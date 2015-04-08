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
	PwmIn();
	virtual ~PwmIn();

	void setup(byte channel);
	void reset();
	byte getPin() { return _pinMappings[_channel-1]; }
	byte getPcint() { return _pcintMappings[_channel-1]; }
	int current() { return _pwmCurrent[_channel-1]; }
	int minimum() { return _pwmMin[_channel-1]; }
	int maximum() { return _pwmMax[_channel-1]; }

	static int _intCount;

	static unsigned long _prevTime[MAX_PWMIN_CHANNELS];
	static unsigned int _pwmCurrent[MAX_PWMIN_CHANNELS];
	static unsigned int _pwmMin[MAX_PWMIN_CHANNELS];
	static unsigned int _pwmMax[MAX_PWMIN_CHANNELS];
	static const byte _pinMappings[MAX_PWMIN_CHANNELS];
	static const byte _pcintMappings[MAX_PWMIN_CHANNELS];

private:
	byte _channel;
	bool _initialized;
	void init(void);
};

#endif /* MEGABOTICS_PWMIN_H_ */
