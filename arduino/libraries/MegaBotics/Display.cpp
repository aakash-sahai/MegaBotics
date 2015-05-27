/*
 * Display.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: sraj
 */

#include <Display.h>

Display Display::_instance(SPort(DISPLAY_DEFAULT_SPORT));

Display::Display(SPort sport) : Adafruit_ILI9341(sport.getSlaveSelectPin(), sport.getDigitalPin(1), sport.getAnalogPin()) {
}

Display::~Display() {
}

void Display::setup(void) {
	setup(_config);
}

void Display::setup(Config& config) {
	begin();
	reset();
	setTextColor(ILI9341_WHITE, ILI9341_BLACK); // White on black
	setRotation(config.rotation);
	setTextSize(config.textSize);
}

void Display::reset() {
	setCursor(0, 0);
}
