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
PwmIn *PwmIn::_instance[MAX_PWMIN_CHANNELS] = { NULL, NULL, NULL, NULL, NULL, NULL };

PwmIn::PwmIn(int channelNum) {
	_channel = channelNum - 1;
	_intrCallback = NULL;
	_intrCallbackFreq = 0;
	_thresBelowCallback = NULL;
	_thresAboveCallback = NULL;
	_thresBelowValue = 0;
	_thresAboveValue = 0;
	_initialized = false;
	_name = "CH" + String(channelNum);
	reset();
}

PwmIn::~PwmIn() {
	_instance[_channel] = NULL;
}

PwmIn * PwmIn::getInstance(byte channelNum) {
	byte channelId = channelNum - 1;
	if (channelId < 0 || channelId >= MAX_PWMIN_CHANNELS) {
		channelId = 0;
	}
	if (_instance[channelId] == NULL) {
		_instance[channelId] = new PwmIn(channelNum);
		_instance[channelId]->init();
	}
	return _instance[channelId];
}

PwmIn & PwmIn::getReference(byte channelNum) {
	return *getInstance(channelNum);
}

/*
 * This routine must be called periodically to check if the pulse interrupts are
 * being generated. If a RC Radio is connected to the PWM input, lack of interrupts
 * signify that the transmitter may have been turned off, or gone out of range. If
 * a wheel encoder is connected to the PWM input then lack of interrupts signify that
 * the wheels have stopped rotating.
 */
bool PwmIn::isAliveSince(void) {
	 if (!_alive) {
		 // Invoke the callback if configured
		 if (_intrCallback) {
			 _intrCallback(_intrCount);
		 }
		 reset();
		 return false;
	 }
	 _alive = false;
	 return true;
}

void PwmIn::onInterrupt(int freq, IntrCallback fn) {
	_intrCallback = fn;
	_intrCallbackFreq = freq;
}

void PwmIn::onBelowThreshold(unsigned int value, ThresCallback fn) {
		_thresBelowCallback = fn;
		_thresBelowValue = value;
}

void PwmIn::onAboveThreshold(unsigned int value, ThresCallback fn) {
		_thresAboveCallback = fn;
		_thresAboveValue = value;
}

void PwmIn::init(void) {
	pinMode(_pinMappings[_channel], INPUT);
	attachPinChangeInterrupt(_pcintMappings[_channel], CHANGE);
	_initialized = true;
}

void PwmIn::reset(void) {
	_min = 0xFFFF;
	_max = 0;
	_current = 0;
	_intrCount = 0;
	_alive = false;
	_prevTime = 0;
	_intrCount = 0;
}

void PwmIn::pullup(void) {
	pinMode(_pinMappings[_channel], INPUT_PULLUP);
}

void PinChangeInterruptEvent(uint8_t pcintNum, bool rising) {
	PwmIn::PinChangeInterrupt(pcintNum, rising);
}

void PwmIn::PinChangeInterrupt(uint8_t pcintNum, bool rising) {
	byte index;
	PwmIn *aPwm = NULL;

	for (index = 0; index < MAX_PWMIN_CHANNELS; index++ ) {
		if (_pcintMappings[index] == pcintNum) {
			aPwm = _instance[index];
			break;
		}
	}

	if (aPwm == NULL) {
		return;
	}

	aPwm->_alive = true;

	if (rising) {
		aPwm->_prevTime = micros();
		unsigned long count = aPwm->_intrCount++;
		if (aPwm->_intrCallback) {
			if (count % aPwm->_intrCallbackFreq == 0) {
				aPwm->_intrCallback(aPwm->_intrCount);
			}
		}
	} else {
		aPwm->_current = micros() - aPwm->_prevTime;
		if (aPwm->_current < aPwm->_min) {
			aPwm->_min = aPwm->_current;
		}
		if (aPwm->_current > aPwm->_max) {
			aPwm->_max = aPwm->_current;
		}
		if (aPwm->_thresBelowCallback && aPwm->_current < aPwm->_thresBelowValue) {
				aPwm->_thresBelowCallback(aPwm->_current);
		} else if (aPwm->_thresAboveCallback && aPwm->_current > aPwm->_thresAboveValue) {
					aPwm->_thresAboveCallback(aPwm->_current);
		}
	}
}
