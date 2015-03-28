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

#include <SPort.h>

const byte SPort::digitalPins[MAX_PORTS][PORT_MAX_DIGITAL_PINS] = {
		{ 40, -1 },
		{ 41, -1 },
		{ 42, -1 },
		{ 43, -1 }
};

const byte SPort::analogPins[MAX_PORTS] = { A4, A5, A6, A7 };
const byte SPort::ssPins[MAX_PORTS] = { 39, 47, 48, 49 };

SPort::SPort() : Port(1) {
	init();
}

SPort::SPort(int aPort) : Port(aPort) {
	init();
}

void SPort::init(void) {
		portInfo.type = SPI;
		portInfo.digitalQty = 1;
		portInfo.analogQty = 1;
		portInfo.pwmInQty = 0;
		portInfo.pwmOutQty = 0;
}

SPort::~SPort() {
	// Nothing to do
}

byte SPort::getDigitalPin(int number) {
	if (number > portInfo.digitalQty || number < 1) {
		return -1;
	} else {
		return digitalPins[portInfo.portNumber - 1][number - 1];
	}
}
