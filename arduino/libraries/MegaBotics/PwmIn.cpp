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
 * Module:		PwmIn - The decoder for 6 channels of PWM Input (e.g. from RC receiver)
 * Created on:	Mar 30, 2015
 * Author:		Aakash Sahai
 */

#include <PwmIn.h>
#include <Arduino.h>
#include <PinChangeInterrupt.h>

const byte PwmIn::_pinMappings[MAX_PWMIN_CHANNELS] = { A12, A13, A14, A15, 11, 12 };
const byte PwmIn::_pcintMappings[MAX_PWMIN_CHANNELS] = { 20, 21, 22, 23, 5, 6 };
unsigned long PwmIn::_prevTime[MAX_PWMIN_CHANNELS] = { 0L, 0L, 0L, 0L, 0L, 0L };
unsigned int PwmIn::_pwmCurrent[MAX_PWMIN_CHANNELS] = { 0L, 0L, 0L, 0L, 0L, 0L };
unsigned int PwmIn::_pwmMax[MAX_PWMIN_CHANNELS] = { 0L, 0L, 0L, 0L, 0L, 0L };
unsigned int PwmIn::_pwmMin[MAX_PWMIN_CHANNELS] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
bool PwmIn::_initialized = false;
int PwmIn::_intCount = 0;

PwmIn::PwmIn() {
}

PwmIn::~PwmIn() {
}

void PwmIn::setup() {
	if (!_initialized) {
		init();
		reset();
	}
}

void PwmIn::init(void) {
	for (int i = 0; i < MAX_PWMIN_CHANNELS; i++ ) {
		pinMode(_pinMappings[i], INPUT);
		attachPinChangeInterrupt(_pcintMappings[i], CHANGE);
	}
	_initialized = true;
}

void PwmIn::reset(void) {
	for (int i = 0; i < MAX_PWMIN_CHANNELS; i++ ) {
		_pwmMin[i] = 0xFFFF;
		_pwmMax[i] = 0;
		_pwmCurrent[i] = 0;
	}
	_initialized = true;
}

void PinChangeInterruptEvent(uint8_t pcintNum, bool rising) {
	byte index;

	PwmIn::_intCount++;

	for (index = 0; index < MAX_PWMIN_CHANNELS; index++ ) {
		if (PwmIn::_pcintMappings[index] == pcintNum)
			break;
	}
	if (index == MAX_PWMIN_CHANNELS) {
		return;		// No matching PCINT found
	}
	if (rising) {
		PwmIn::_prevTime[index] = micros();
	} else {
		PwmIn::_pwmCurrent[index] = micros() - PwmIn::_prevTime[index];
		if (PwmIn::_pwmCurrent[index] < PwmIn::_pwmMin[index]) {
			PwmIn::_pwmMin[index] = PwmIn::_pwmCurrent[index];
		}
		if (PwmIn::_pwmCurrent[index] > PwmIn::_pwmMax[index]) {
			PwmIn::_pwmMax[index] = PwmIn::_pwmCurrent[index];
		}
	}
}
