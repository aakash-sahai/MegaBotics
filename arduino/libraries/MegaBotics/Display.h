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
 * Module		Display.h - TFT display
 * Uses:		Adafruit_ILI9341 (https://github.com/adafruit/Adafruit_ILI9341)
 * 				Adafruit_GFX libraries (https://github.com/adafruit/Adafruit-GFX-Library)
 *
 * Created on:	Apr 22, 2015
 * Author:		Srinivas Raj
 */

#ifndef MEGABOTICS_DISPLAY_H_
#define MEGABOTICS_DISPLAY_H_

#include <SPort.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define DISPLAY_DEFAULT_SPORT		3

class Display : public Adafruit_ILI9341 {
public:
	struct Config {
		uint8_t textSize;
		uint8_t rotation;

		Config() {
			textSize = 3;
			rotation = 0;
		}
	};

	Display(SPort sport);
	virtual ~Display();
	void setup(void);
	void setup(Config& config);
	void reset();
	void clearScr() {fillScreen(ILI9341_BLACK);}

	static Display * getInstance() { return &_instance; }
	static Display & getReference() { return _instance; }

private:
	static Display _instance;
	Config _config;
};

#endif /* MEGABOTICS_DISPLAY_H_ */
