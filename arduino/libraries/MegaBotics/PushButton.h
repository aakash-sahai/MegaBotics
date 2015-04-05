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
 * PushButton.h
 *
 *  Created on: 14-Mar-2015
 *      Author: eashan
 */

#include <Arduino.h>

#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

#define DEFAULT_PUSHBUTTON_PIN	2

class PushButton {
private:
	bool _isPressed;
	bool _wasClicked;
	int _clickQty;
	byte	_pin;

	void init(void);

public:

	PushButton();
	PushButton(byte);

	virtual ~PushButton();

	void check(void);
	int timesClicked() { check(); return _clickQty; }
	void clear() { this->_clickQty = 0; }
	bool pressed(void) { check(); return _isPressed; }
	bool clicked(void);
	byte getPin() { return _pin; }
};

#endif /* PUSHBUTTON_H_ */
