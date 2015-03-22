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
 * PushButton.cpp
 *
 *  Created on: 14-Mar-2015
 *      Author: eashan
 */

#include "PushButton.h"

PushButton::PushButton() {
	_config.pin = DEF_PIN;
	_config.type = DEF_TYPE;
	init();
}

PushButton::PushButton(uint8_t aPin) {
	_config.pin = aPin;
	_config.type = DEF_TYPE;
	init();
}

void PushButton::setup(PushButtonConfig &aConfig) {
	if (aConfig.type == OUTPUT) {
		Serial.println("Silly you! Button can't be OUTPUT type.");
		aConfig.type = INPUT;
	}
	_config = aConfig;
	init();
}

PushButton::~PushButton() {
	// Nothing to do
}


void PushButton::init(void) {
	this->_clickQty = 0;
	this->_isPressed = false;
	this->_wasClicked = false;
	pinMode(_config.pin, _config.type);
}

void PushButton::check(void) {
	int state = digitalRead(_config.pin);
	if (state == HIGH) {
		this->_isPressed = true;
	} else {
		if (this->_isPressed == true) {
			this->_wasClicked = true;
			this->_clickQty++;
			this->_isPressed = false;
		}
	}
}

bool PushButton::clicked(void) {
	this->check();
	if (this->_wasClicked) {
		this->_wasClicked = false;
		return true;
	}
	return false;
}

