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
 * Module		SDCard.h
 *
 * Created on:	Apr 22, 2015
 * Author:		Srinivas Raj
 */

#ifndef MEGABOTICS_SDCARD_H_
#define MEGABOTICS_SDCARD_H_

#include <SD.h>
#include <SPort.h>

#define SDCARD_DEFAULT_SPORT		4

class SDCard {
public:
	enum Status {
		SUCCESS = 0,
		FAILURE = 1
	};

	static SDCard * getInstance() { return & _instance; }
	static SDCard & getReference() { return _instance; }

	virtual ~SDCard();
	Status setup(void);
	File open(const char *filepath, uint8_t mode) {return SD.open(filepath, mode);}

private:
	static SDCard _instance;
	SPort _sport;

	SDCard();
	SDCard(int port);
	SDCard(SDCard const&);              // Don't Implement to disallow copy by assignment
    void operator=(SDCard const&);		// Don't implement to disallow copy by assignment
};

#endif /* MEGABOTICS_SDCARD_H_ */
