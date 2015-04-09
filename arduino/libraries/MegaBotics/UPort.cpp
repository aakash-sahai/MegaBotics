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

#include <UPort.h>

const byte UPort::digitalPins[MAX_PORTS][PORT_MAX_DIGITAL_PINS] = {
		{ 22, 23 },
		{ 24, 25 },
		{ 26, 27 },
		{ 28, 29 }
};

const byte UPort::analogPins[MAX_PORTS] = { A0, A1, A2, A3 };
const byte UPort::pwmInPins[MAX_PORTS]  = { A12, A13, A14, A15 };
HardwareSerial * const UPort::serialDrivers[MAX_PORTS] = { &Serial, &Serial1, &Serial2, &Serial3 };

UPort::UPort() : Port(1) {
	init();
}

UPort::UPort(int aPort) : Port(aPort) {
	init();
}

void UPort::init(void) {
	portInfo.type = UART;
	portInfo.digitalQty = 2;
	portInfo.analogQty = 1;
	portInfo.pwmInQty = 1;
	portInfo.pwmOutQty = 0;
}

UPort::~UPort() {
	// Nothing to do
}

HardwareSerial& UPort::serial() {
	return *serialDrivers[portInfo.portNumber - 1];
}

byte UPort::getDigitalPin(int number) {
	if (number > portInfo.digitalQty || number < 1) {
		return -1;
	} else {
		return digitalPins[portInfo.portNumber - 1][number - 1];
	}
}
