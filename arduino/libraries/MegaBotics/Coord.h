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

#ifndef _COORD_H_
#define _COORD_H_

class Coord2D
{
public:

	enum CoordType {
		CARTESIAN	= 0,
		POLAR		= 1,
		GEO			= 2
	};

	Coord2D() { }
	virtual ~Coord2D() { }

	CoordType type() { return _type; }

private:
	const static CoordType _type;
};

class Geo2D;
class Cartesian2D;

class Polar2D : public Coord2D {
	friend class Geo2D;
	friend class Cartesian2D;

public:
	Polar2D() { _r = 0; _theta = 0; };
	Polar2D(double r, double theta);
	virtual ~Polar2D() { };

	double getR() { return _r; }
	double getTheta() { return _theta; }

private:
	double _r;
	double _theta;
	const static  CoordType _type = POLAR;
};

class Cartesian2D : public Coord2D {
public:
	Cartesian2D() { _x = 0; _y = 0; };
	Cartesian2D(float x, float y);
	virtual ~Cartesian2D() { };

	void add(Cartesian2D &to, Cartesian2D &result);
	void sub(Cartesian2D &to, Cartesian2D &result);
	void mul(float factor, Cartesian2D &result);

	void getRTheta(Cartesian2D &to, Polar2D &result);

	float getX() { return _x; }
	float getY() { return _y; }

private:
	float _x;
	float _y;
	const static CoordType _type = CARTESIAN;
};

class Geo2D : public Coord2D {
	friend class WaypointHelper;
public:
	Geo2D();
	Geo2D(double latitude, double longitude);
	virtual ~Geo2D();

	void getRTheta(Geo2D &to, Polar2D &result);

	double getLatitude() { return _latitude; }
	double getLongitude() { return _longitude; }

	void setLatitude(double val) {  _latitude = val; }
	void setLongitude(double val) { _longitude = val; }

private:
	double _latitude;
	double _longitude;
	const static  CoordType _type = GEO;
	const static double R_EARTH_METER = 6371000.0;	// Radius of Earth in Meters
	const static double METER_TO_FEET = 3.28084;
};

#endif /* _COORD_H_ */
