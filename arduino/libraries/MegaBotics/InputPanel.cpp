/*
 * InputPanel.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: sraj
 */

#include <InputPanel.h>

InputPanel::InputPanel() :
		_uport(INPUT_PANEL_DEFAULT_UPORT), _pushButton(_uport.getAnalogPin()), _spdtSwitch(
				_uport.getDigitalPin(1), _uport.getDigitalPin(2)) {
}

InputPanel::InputPanel(int port) :
		_uport(port), _pushButton(_uport.getAnalogPin()), _spdtSwitch(
				_uport.getDigitalPin(1), _uport.getDigitalPin(2)) {
}

InputPanel::~InputPanel() {
}
