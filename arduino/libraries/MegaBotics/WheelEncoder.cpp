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
 * WheelEncoder.cpp
 *
 *  Created on: Apr 20, 2015
 *      Author: Aakash Sahai
 */

#include "WheelEncoder.h"

WheelEncoder WheelEncoder::_instance;

WheelEncoder::WheelEncoder() {
	_config.wheelEncoderChannel = DEF_WHEEL_ENCODER_CHANNEL;
	_config.pulsesPerFeet = DEF_PULSES_PER_FEET;
	_speed = 0; _distance = 0;
	_prevPulses = 0;
}

WheelEncoder::~WheelEncoder() {
	// Nothing to do
}

void WheelEncoder::setup(void) {
	_wheelEncoderIn = PwmIn::getInstance(_config.wheelEncoderChannel);
	_wheelEncoderIn->pullup();
}

void WheelEncoder::setup(byte wheelEncoderChannel, byte pulsesPerFeet) {
	_config.wheelEncoderChannel = wheelEncoderChannel;
	_config.pulsesPerFeet = pulsesPerFeet;
	setup();
}

void WheelEncoder::setup(Config &aConfig)
{
	_config = aConfig;
	setup();
}

