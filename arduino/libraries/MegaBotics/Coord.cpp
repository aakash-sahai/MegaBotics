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

#include <MegaBotics.h>

Polar2D::Polar2D(double r, double theta) {
	_r = r;
	_theta = theta;
}

Cartesian2D::Cartesian2D(float x, float y) {
	_x = x;
	_y = y;
}

void Cartesian2D::add(Cartesian2D &to, Cartesian2D &result) {
	result._x = _x + to._x;
	result._y = _y + to._y;
}

void Cartesian2D::sub(Cartesian2D &to, Cartesian2D &result) {
	result._x = _x - to._x;
	result._y = _y - to._y;
}

void Cartesian2D::mul(float factor, Cartesian2D &result) {
	result._x = factor * _x;
	result._y = factor * _y;
}

void Cartesian2D::getRTheta(Cartesian2D &to, Polar2D &result) {
	double deltaX = to._x - _x;
	double deltaY = to._y - _y;
	result._r = sqrt(deltaX*deltaX + deltaY*deltaY);
	result._theta = atan2(deltaY, deltaX);
}

Geo2D::Geo2D() {
	_latitude = 0.0;
	_longitude = 0.0;
}


Geo2D::Geo2D(double latitude, double longitude) {
	_latitude = latitude;
	_longitude = longitude;
}

Geo2D::~Geo2D() {
}

void Geo2D::getRTheta(Geo2D &to, Polar2D &result) {
	/*
	 * Since we are dealing with small distances, we will use Equi-rectangular
	 * formula to reduce computation, instead of more accurate Haversine
	 * formula which is compute intensive.
	 */
	double toLat = to._latitude * DEG_TO_RAD;
	double toLon = to._longitude * DEG_TO_RAD;
	double fromLat = _latitude * DEG_TO_RAD;
	double fromLon = _longitude * DEG_TO_RAD;

	double deltaX = (toLon - fromLon) * cos((fromLat + toLat) / 2.0);
	double deltaY = toLat - fromLat;
	result._r = sqrt(deltaX*deltaX + deltaY*deltaY) * R_EARTH_METER;
	result._theta = atan2(deltaY, deltaX);
}
