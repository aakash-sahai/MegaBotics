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
 * Route.h
 *
 *  Created on: May 29, 2015
 *      Author: Srinivas Raj
 */

#ifndef MEGABOTICS_ROUTE_H_
#define MEGABOTICS_ROUTE_H_

#include "MegaBotics.h"

#define MAX_WAYPOINTS	30

class GPS;
class Logger;
class JoyStick;
class Dispaly;

#define DEFAULT_WAYPOINT_FILE	"WP.TXT"
#define DEFAULT_INVALID_REF_HED	-999

class Route {
public:
	struct Config {
		float gpsStaleThres;	// Time in msec when GPS reading is considered too stale
		float distTooFarThres;	// Distance in meter from next WP that is considered too far
		float refHeading;		// reference heading

		Config() {
			gpsStaleThres = 2000;
			distTooFarThres = 100;
			refHeading = DEFAULT_INVALID_REF_HED;	// invalid value to indicate if the ref. heading is set in the config
		}
	};

	struct Location {
		double lat;		// Current Latitude
		double lon;		// Current Longitude
		float distance;	// Distance to next waypoint
		float hdg;			// Heading to next waypoint
		float refHdg;		// Reference Heading
		unsigned int age;	// Age of GPS reading
		float speed;		// Speed reported by GPS


		Location() {
			hdg = 0.0f;
			distance = 0.0f;
			refHdg = 0.0f;
			lat = 0.0;
			lon = 0.0;
			speed = 0.0;
			age = 0;
		};
	};

	Route();
	virtual ~Route();

	void setup();

	int configWaypoints();
	void loadWaypoints();
	void addWaypoint(Waypoint &wp);
	int nextWaypoint();
	bool reachedNextWaypoint() {
		float pr = _waypoints[_currentWaypoint].getProximRadius();
		return (_currentLocation.distance < pr);
	}
	Location & updateLocation();
	Location & updateLocation(byte waypoint);
	void waitForGpsFix();
	void display();

	Location & getCurrentLocation() { return _currentLocation; }
	Waypoint & getCurrentWaypoint() { return _waypoints[_currentWaypoint];}
	void clearWaypoints(void) { _waypointQty = 0; _currentWaypoint = -1; }

	static Route * getInstance() { return &_instance; }
	static Route & getReference() { return _instance; }

private:
	static Route _instance;

	Waypoint _waypoints[MAX_WAYPOINTS];
	byte _waypointQty;

	byte _currentWaypoint;
	Location _currentLocation;

	GPS* _gps;
	Display* _display;
	Logger* _logger;
	JoyStick* _joyStick;
	File _file;

	void open();
	void close();
	void logWaypoints();
};

#endif /* MEGABOTICS_ROUTE_H_ */
