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


#include <IPort.h>

const byte IPort::digitalPins[MAX_PORTS][PORT_MAX_DIGITAL_PINS] = {
		{ 30, 31 },
		{ 32, 33 },
		{ 34, 35 },
		{ 36, 37 }
};
const byte IPort::analogPins[MAX_PORTS] = { A8, A9, A10, A11 };
const byte IPort::pwmOutPins[MAX_PORTS] = { 10, 44, 45, 46 };

IPort::IPort() : Port(1) {
	init();
}

IPort::IPort(int aPort) : Port(aPort) {
	init();
}

void IPort::init(void) {
	portInfo.type = I2C;
	portInfo.digitalQty = 2;
	portInfo.analogQty = 1;
	portInfo.pwmInQty = 0;
	portInfo.pwmOutQty = 1;
}

IPort::~IPort() {
	// Nothing to do
}

byte IPort::getDigitalPin(int number) {
	if (number > portInfo.digitalQty || number < 1) {
		return -1;
	} else {
		return digitalPins[portInfo.portNumber - 1][number - 1];
	}
}
