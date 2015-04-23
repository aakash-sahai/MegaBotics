/*
 * Display.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: sraj
 */

#include <Display.h>

Display::Display() : _sport(DISPLAY_DEFAULT_SPORT), _tft(_sport.getSlaveSelectPin(), _sport.getDigitalPin(1), _sport.getAnalogPin()) {
}

Display::Display(int port) : _sport(port), _tft(_sport.getSlaveSelectPin(), _sport.getDigitalPin(1), _sport.getAnalogPin()) {
}

Display::~Display() {
}

void Display::setup(void) {
	_tft.begin();
	_tft.setTextColor(ILI9341_BLACK);
	_tft.setRotation(3);
	_tft.setTextSize(2);
}
