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
 * WheelEncoder.h
 *
 *  Created on: Apr 20, 2015
 *      Author: Aakash Sahai
 */

#ifndef MEGABOTICS_WHEELENCODER_H_
#define MEGABOTICS_WHEELENCODER_H_

#include <PwmIn.h>

class WheelEncoder {
public:
	static const byte  DEF_WHEEL_ENCODER_CHANNEL = 6;	// default PwmIn channel connected to wheel encoder
	static const byte  DEF_PULSES_PER_REV = 12;			// default number of pulses generated per revolution
	static const float DEF_FEET_PER_REV = (14.5 / 12);	// default feet of linear travel per revolution of the wheel

	struct Config {
		byte	wheelEncoderChannel;
		byte	pulsesPerRev;
		float	feetsPerRev;
	};

	static WheelEncoder * getInstance() { return &_instance; }
	static WheelEncoder & getReference() { return _instance; }

	virtual ~WheelEncoder();

	void setup(Config & config);
	void setup(void);

	void poll(void);
	void reset(void) { _pwmIn->reset(); }

	float getRevolutions() { return ((float)_pwmIn->getPulseCount() / (float)_config.pulsesPerRev); }
	float getRps();			// Instantaneous Revs/sec calculated from EMA of pulse period
	float getDistance();	// Total distance traveled since last reset
	float getSpeed();		// Instantaneous speed in feet/sec

private:
	static WheelEncoder _instance;

	Config		 _config;
	PwmIn	*	 _pwmIn;

	WheelEncoder();
	WheelEncoder(WheelEncoder const&);          // Don't Implement to disallow copy by assignment
    void operator=(WheelEncoder const&);		// Don't implement to disallow copy by assignment
};

#endif /* MEGABOTICS_WHEELENCODER_H_ */
