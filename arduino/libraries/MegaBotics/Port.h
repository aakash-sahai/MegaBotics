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

#ifndef MEGABOTICS_PORT_H_
#define MEGABOTICS_PORT_H_

#include <Arduino.h>

enum PortType {
	PORT_UART = 1,
	PORT_SPI = 2,
	PORT_I2C = 3
};

#define MAX_PORTS	4				// Maximum number of ports of a given type
#define PORT_MAX_DIGITAL_PINS	2	// Maximum number of DIGITAL GPIO pins per port
#define PORT_MAX_ANALOG_PINS 1		// Maximum number of ANALOG ONLY pins per port
#define PORT_MAX_PWMIN_PINS 1		// Maximum number of ANALOG/PWMIN pins per port
#define PORT_MAX_PWMOUT_PINS 1		// Maximum number of PWMOUT pins per port

struct PortInfo {
	PortType type;
	int digitalQty;
	int analogQty;
	int pwmInQty;
	int pwmOutQty;
	int portNumber;
};

class Port {
public:
	Port();
	Port(int number);
	virtual ~Port();

	virtual void init(void) = 0;
	const PortInfo& getPortInfo();
	virtual byte getDigitalPin(int number);
	virtual byte getAnalogPin();
	virtual byte getPwmInputPin();
	virtual byte getPwmOutputPin();
	virtual int getPortType() = 0;
	int getPortNumber() { return portInfo.portNumber; }

protected:
	PortInfo portInfo;

};

#endif /* MEGABOTICS_PORT_H_ */
