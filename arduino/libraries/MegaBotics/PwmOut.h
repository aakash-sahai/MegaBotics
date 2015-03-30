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
 * Module:		PwmOut.h - The Pulse Width Modulation Output
 * Created on:	Mar 29, 2015
 * Author:		Aakash Sahai
 *
 * The MegabBotics platform has 6 PWM output channels. This file provides the
 * abstractions to easily access those PWM channels.
 */

#ifndef MEGABOTICS_PWMOUT_H_
#define MEGABOTICS_PWMOUT_H_

#include <Arduino.h>
#include <Servo.h>

#define MAX_PWMOUT_CHANNELS	6

class PwmOut {
public:
	PwmOut();
	virtual ~PwmOut();

	void setup(byte channel);
	byte getPin(void) { return _pinMappings[_channel-1]; }
	void writeAnalog(int value);
	int attachServo(void);
	int attachServo(int minPwm, int maxPwm);
	void detachServo();
	bool isServoAttached();
	int readServo();
	void writeServo(int value);
	void writeMicrosecondsServo(int value);

private:
	byte _channel;
	static const byte _pinMappings[MAX_PWMOUT_CHANNELS];
	Servo _servo;
};

#endif /* MEGABOTICS_PWMOUT_H_ */
