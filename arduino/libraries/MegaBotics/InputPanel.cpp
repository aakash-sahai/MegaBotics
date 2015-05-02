/*
 * InputPanel.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: sraj
 */

#include <InputPanel.h>

InputPanel::InputPanel() :
		UPort(INPUT_PANEL_DEFAULT_UPORT), PushButton(getAnalogPin()), SpdtSwitch(
				getDigitalPin(1), getDigitalPin(2)) {
}

InputPanel::InputPanel(int port) :
		UPort(port), PushButton(getAnalogPin()), SpdtSwitch(
						getDigitalPin(1), getDigitalPin(2)) {
}

InputPanel::~InputPanel() {
}
