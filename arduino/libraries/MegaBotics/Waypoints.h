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

/*
 * Waypoints.h
 *
 *  Created on: May 29, 2015
 *      Author: Srinivas Raj
 */

#ifndef MEGABOTICS_WAYPOINTS_H_
#define MEGABOTICS_WAYPOINTS_H_

#include <MegaBotics.h>

class GPS;
class Logger;
class JoyStick;
class Dispaly;

#define DEFAULT_WAYPOINT_FILE	"wp.txt"

class Waypoints {
public:
	Waypoints();
	virtual ~Waypoints();

	int configWaypoints();
	bool nextWaypoint(Geo2D& waypoint);
	void open();
	void close();

	static Waypoints * getInstance() { return &_instance; }
	static Waypoints & getReference() { return _instance; }

private:
	static Waypoints _instance;

	GPS* _gps;
	Display* _display;
	Logger* _logger;
	JoyStick* _joyStick;
	File _file;
};

#endif /* MEGABOTICS_WAYPOINTS_H_ */