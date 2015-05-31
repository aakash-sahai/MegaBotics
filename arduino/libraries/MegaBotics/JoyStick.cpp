/*
 * JoyStick.cpp
 *
 *  Created on: May 29, 2015
 *      Author: sraj
 */

#include <JoyStick.h>

JoyStick JoyStick::_instance;

JoyStick::JoyStick() : UPort(JOY_STICK_DEFAULT_UPORT), PushButton(getDigitalPin(1)) {
	_verPin = getAnalogPin();
	_horPin = getPwmInputPin();
}

JoyStick::JoyStick(int port) : UPort(port), PushButton(getDigitalPin(1)) {
	_verPin = getAnalogPin();
	_horPin = getPwmInputPin();
}

JoyStick::~JoyStick() {
}

JoyStick::Input JoyStick::waitForInput(long duration) {
	long start = millis();

	while (true) {
		int xPos = analogRead(_verPin);
		int yPos = analogRead(_horPin);
		bool click = clicked();

		if (xPos < DEFAULT_JOY_STICK_MIN_THRES) return RIGHT;
		if (xPos > DEFAULT_JOY_STICK_MAX_THRES) return LEFT;
		if (yPos < DEFAULT_JOY_STICK_MIN_THRES) return UP;
		if (yPos > DEFAULT_JOY_STICK_MAX_THRES) return DOWN;
		if (click) return CENTER;

		if (millis() - start >= duration) {
			break;
		}
	}

	return NONE;
}
