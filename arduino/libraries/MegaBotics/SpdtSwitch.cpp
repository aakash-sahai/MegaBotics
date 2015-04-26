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
 * SpdtSwitch.cpp
 *
 *  Created on: 25-Apr-2015
 *      Author: Eashan Sahai
 */


#include <SpdtSwitch.h>

SpdtSwitch::SpdtSwitch() {
	_pinUp = DEFAULT_SPDT_PIN_UP;
	_pinDown = DEFAULT_SPDT_PIN_DOWN;
	init();
}

SpdtSwitch::~SpdtSwitch() {
	// TODO Auto-generated destructor stub
}


SpdtSwitch::SpdtSwitch(byte pinUp, byte pinDown) {
		_pinUp = pinUp;
		_pinDown = pinDown;
		init();
}


SpdtSwitch::Position SpdtSwitch::getPosition(void) {
	byte upState = digitalRead(_pinUp);
	byte downState = digitalRead(_pinDown);
	if (upState == LOW) {
		return UP;
	}
	if (downState == LOW) {
		return DOWN;
	}
	return MID;
}

void SpdtSwitch::init(void) {
	pinMode(_pinUp, INPUT_PULLUP);
	pinMode(_pinDown, INPUT_PULLUP);
}
