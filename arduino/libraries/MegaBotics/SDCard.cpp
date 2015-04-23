/*
 * SDCard.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: sraj
 */

#include <SDCard.h>

SDCard::SDCard() : _sport(SDCARD_DEFAULT_SPORT) {
}

SDCard::SDCard(int port) : _sport(port) {
}

SDCard::~SDCard() {
}

SDCard::Status SDCard::setup(void) {
	if (!SD.begin(_sport.getSlaveSelectPin())) {
		return FAILURE;
	}

	return SUCCESS;
}
