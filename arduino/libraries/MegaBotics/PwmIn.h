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
	typedef void (*IntrCallback)(unsigned long gePulseCount);
	typedef void (*ThresCallback)(unsigned int uSec);

	virtual ~PwmIn();

	static PwmIn * getInstance(byte channelNum);
	static PwmIn & getReference(byte channelNum);

	void reset(void);
	bool isAliveSince(void);	// Checks if the channel is alive since last checked
	byte getPin() { return _pinMappings[_channel]; }
	byte getPcint() { return _pcintMappings[_channel]; }

	String name() { return _name; }
	void setName(String aName) { _name = aName; }
	unsigned int getPulsePeriod() { return _period; }
	unsigned int getPulseWidth() { return _width; }
	unsigned int getMinPulseWidth() { return _minWidth; }
	unsigned int getMaxPulseWidth() { return _maxWidth; }
	unsigned long getPulseCount() { return _pulseCount; }
	unsigned int getElapsedTime() { return _elapsedTime; }

	void onInterrupt(int everySoMany, IntrCallback fn);
	void onBelowThreshold(unsigned int value, ThresCallback fn);
	void onAboveThreshold(unsigned int value, ThresCallback fn);

	void pullup();		// Call to pullup the Pin - needed for wheel encoder
	static void PinChangeInterrupt(byte pcintNum, bool rising);

	static PwmIn *_instance[MAX_PWMIN_CHANNELS];
	static const byte _pinMappings[MAX_PWMIN_CHANNELS];
	static const byte _pcintMappings[MAX_PWMIN_CHANNELS];


private:
	String _name;
	byte _channel;
	bool _alive;
	bool _initialized;
	unsigned int _minWidth;
	unsigned int _maxWidth;
	unsigned int _width;
	unsigned int _period;
	unsigned long _prevTime;
	unsigned long _elapsedTime;
	unsigned long _pulseCount;
	IntrCallback _intrCallback;
	unsigned int _intrCallbackFreq;
	ThresCallback _thresBelowCallback;
	ThresCallback _thresAboveCallback;
	unsigned int _thresBelowValue;
	unsigned int _thresAboveValue;
	void init(void);

	PwmIn();
	PwmIn(int channelNum);
	PwmIn(PwmIn const&);          		// Don't Implement to disallow copy by assignment
    void operator=(PwmIn const&);		// Don't implement to disallow copy by assignment
};

#endif /* MEGABOTICS_PWMIN_H_ */
