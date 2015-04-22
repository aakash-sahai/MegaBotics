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
 * Module:		PwmMux.cpp - The multiplexer for PWM Output - allows the PWM Output channels 1-4
 * 				on the MegaBotics platform to be driven either by Arduino pins or by the
 * 				PWM Input channels 1-4. This way servos/motors attached to PWM Output
 * 				channels 1-4 can be controlled either programmatically, or directly from
 * 				a RC radio receiver connected to channels 1-4.
 *
 * Created on:	Mar 30, 2015
 * Author:		Aakash Sahai
 */

#ifndef PWMMUX_H_
#define PWMMUX_H_

#define PWMMUX_SEL_PIN	38

class PwmMux {
public:
	enum Mode {
		PWMIN = 0,
		PROGRAM = 1
	};

	virtual ~PwmMux();

	static PwmMux * getInstance() { return &_instance; }
	static PwmMux & getReference() { return _instance; }

	void setup(void) { setup(PWMIN); }
	void setup(Mode mode);
	void setMode(Mode mode);
	Mode getMode(void) { return _currentMode; }

private:
	Mode _currentMode;
	static PwmMux _instance;

	PwmMux();
	PwmMux(PwmMux const&);              // Don't Implement to disallow copy by assignment
    void operator=(PwmMux const&);		// Don't implement to disallow copy by assignment
};

#endif /* PWMMUX_H_ */
