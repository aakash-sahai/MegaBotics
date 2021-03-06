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

#ifndef MEGABOTICS_UPORT_H_
#define MEGABOTICS_UPORT_H_

#include <Port.h>
#include <pins_arduino.h>
#include <HardwareSerial.h>

class UPort: public Port {
public:
	UPort();
	UPort(int number);
	virtual ~UPort();

	HardwareSerial& serial();

	void init(void);
	const PortInfo& getPortInfo();
	byte getAnalogPin() { return analogPins[portInfo.portNumber - 1]; }
	byte getDigitalPin(int number);
	byte getPwmInputPin() { return pwmInPins[portInfo.portNumber - 1]; }
	int getPortType() { return PORT_UART; }
	void cross(UPort &uport);
	void uncross();
	void interact(void);
	bool doEscape(int ch);

private:
	UPort *_crossPort;
	static const byte digitalPins[MAX_PORTS][PORT_MAX_DIGITAL_PINS];
	static const byte analogPins[MAX_PORTS];
	static const byte pwmInPins[MAX_PORTS];
	static HardwareSerial * const serialDrivers[MAX_PORTS];
};

#endif /* MEGABOTICS_UPORT_H_ */
