/*
 * Waypoint.h
 *
 *  Created on: Jun 7, 2015
 *      Author: aakash
 */

#ifndef MEGABOTICS_WAYPOINT_H_
#define MEGABOTICS_WAYPOINT_H_

#include "Coord.h"
#include <Arduino.h>

class Waypoint : public Geo2D {
public:
	enum Type {
		LAT_LON = 0,	// Specified as GPS Coordinates
		DIST_HDG = 1	// Specified in terms of Relative Distance and Heading
	};

	Waypoint();
	Waypoint(Type type, double lat_or_dist, double lon_or_hdg);
	Waypoint(Type type, double lat_or_dist, double lon_or_hdg, int8_t maxThrottle);
	Waypoint(Type type, double lat_or_dist, double lon_or_hdg, int8_t maxThrottle, double proximRadius);

	void setProximRadius(double val) {  _proximRadius = val; }
	double getProximRadius() {  return _proximRadius; }
	void setMaxThrottle(int8_t val) { _maxThrottle = val; }
	int8_t getMaxThrottle() {  return _maxThrottle; }

	virtual ~Waypoint();

private:
	void init(Type type, double lat_or_dist, double lon_or_hdg);
	byte _type;
	double _distance;
	double _hdg;
	double _proximRadius;
	int8_t _maxThrottle;

	static const int8_t defaultMaxThrottle = 30;	// 10% max throttle
	static const double defaultProximRadius = 3.0;	// 3 meter proxim radius

};

#endif /* MEGABOTICS_WAYPOINT_H_ */
