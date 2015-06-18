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
 * JoyStick.h
 *
 *  Created on: May 29, 2015
 *      Author: Srinivas Raj
 */

#ifndef MEGABOTICS_JOYSTICK_H_
#define MEGABOTICS_JOYSTICK_H_

#define JOY_STICK_DEFAULT_UPORT				3

#define DEFAULT_JOY_STICK_MIN_THRES			10
#define DEFAULT_JOY_STICK_MAX_THRES			1000

#include <MegaBotics.h>

class JoyStick : public UPort, public PushButton {
public:
	enum Input {
		NONE = 0,
		LEFT = 1,
		RIGHT = 2,
		UP = 3,
		DOWN = 4,
		CENTER = 5
	};

	JoyStick();
	JoyStick(int port);

	virtual ~JoyStick();
	Input waitForInput(long duration);

	static JoyStick * getInstance() { return & _instance; }
	static JoyStick & getReference() { return _instance; }

private:
	static JoyStick _instance;
	byte _horPin;
	byte _verPin;
};

#endif /* MEGABOTICS_JOYSTICK_H_ */
