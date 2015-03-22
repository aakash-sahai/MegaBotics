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
 * Sonar.cpp
 *
 *  Created on: Jun 14, 2014
 *      Author: aakash
 */

#include "MegaBotics.h"

Sonar::Sonar()
{
	_config.echoPin = DEF_ECHO_PIN;
	_config.trigPin = DEF_TRIG_PIN;
}

void Sonar::setup(SonarConfig &aConfig)
{
	_config = aConfig;
	setup();
}

void Sonar::setup(void) {
	 pinMode(_config.trigPin, OUTPUT);
	 pinMode(_config.echoPin, INPUT);
}

int Sonar::getDistance()
{
	int	i;
	int dist = 0;

	// Filter the sonar input (average of four readings)
	for (i=0; i<4; i++) {
		dist += getDistanceSingle();
	}
	dist /= 4;

	return dist;
}

int Sonar::getDistanceSingle() {
	 digitalWrite(_config.trigPin, LOW);
	 delayMicroseconds(2);

	 digitalWrite(_config.trigPin, HIGH);
	 delayMicroseconds(10);

	 digitalWrite(_config.trigPin, LOW);
	 long duration = pulseIn(_config.echoPin, HIGH);

	 //Calculate the distance (in cm) based on the speed of sound.
	 return  (int)(duration/58.28);
}
