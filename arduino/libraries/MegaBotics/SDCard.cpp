/*
 * SDCard.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: sraj
 */

#include <SDCard.h>

SDCard SDCard::_instance;

SDCard::SDCard() : _sport(SDCARD_DEFAULT_SPORT) {
	_status = NOT_INITIALIZED;
}

SDCard::SDCard(int port) : _sport(port) {
	_status = NOT_INITIALIZED;
}

void SDCard::setup() {
	if (_status == NOT_INITIALIZED) {
		if (!SD.begin(_sport.getSlaveSelectPin())) {
			_status = FAILURE;
		} else {
			_status = SUCCESS;
		}
	}
}

SDCard::~SDCard() {
}
