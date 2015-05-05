/*
 * SDCard.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: sraj
 */

#include <SDCard.h>

SDCard SDCard::_instance;

SDCard::SDCard() : _sport(SDCARD_DEFAULT_SPORT) {
	if (!SD.begin(_sport.getSlaveSelectPin())) {
		_status = FAILURE;
	} else {
		_status = SUCCESS;
	}
}

SDCard::SDCard(int port) : _sport(port) {
	if (!SD.begin(_sport.getSlaveSelectPin())) {
		_status = FAILURE;
	} else {
		_status = SUCCESS;
	}
}

SDCard::~SDCard() {
}
