/*
 * Waypoint.cpp
 *
 *  Created on: Jun 7, 2015
 *      Author: aakash
 */

#include <Waypoint.h>

Waypoint::Waypoint() {
	_type = LAT_LON;
	_latitude = 0.0;
	_longitude = 0.0;
	_distance = 0.0;
	_hdg = 0.0;
	_proximRadius = defaultProximRadius;
	_maxThrottle = defaultMaxThrottle;
}

Waypoint::Waypoint(Type type, double lat_or_dist, double lon_or_hdg) {
	_maxThrottle = defaultMaxThrottle;
	_proximRadius = defaultProximRadius;
	init(type, lat_or_dist, lon_or_hdg);
}

void Waypoint::init(Type type, double lat_or_dist, double lon_or_hdg) {
	_type = type;
	if (type == LAT_LON) {
		_latitude = lat_or_dist;
		_longitude = lon_or_hdg;
	} else {
		_distance = lat_or_dist;
		_hdg = lon_or_hdg;
	}
}

Waypoint::Waypoint(Type type, double lat_or_dist, double lon_or_hdg, int8_t maxThrottle) {
	_maxThrottle = maxThrottle;
	_proximRadius = defaultProximRadius;
	init(type, lat_or_dist, lon_or_hdg);
}

Waypoint::Waypoint(Type type, double lat_or_dist, double lon_or_hdg, int8_t maxThrottle, double proximRadius) {
	_maxThrottle = maxThrottle;
	_proximRadius = proximRadius;
	init(type, lat_or_dist, lon_or_hdg);
}

Waypoint::~Waypoint() {
	// Nothing to do
}

