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
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
 */

/*
 * SpdtSwitch.ino
 *
 * This is an example sketch to demonstrate the functionality of
 * the LED and SpdtSwitch objects.
 *
 *  Created on: 25-Apr-2015
 *      Author: Eashan Sahai
 */
#include <MegaBotics.h>

const int SWITCH_UP_PIN = 4;    // the switch up pin
const int SWITCH_DOWN_PIN = 5;	// The switch down pin
const int LED_RED =  10;     	// the RED LED pin - turned on when switch is in DOWN position
const int LED_BLUE =  11;    	// the BLUE LED pin - turned on when switch is in MID position
const int LED_GREEN = 12;		// the BLUE LED pin - turned on when switch is in UP position

SpdtSwitch sw;
LED ledRed;
LED ledBlue;
LED ledGreen;

void setup() {
  sw = SpdtSwitch(SWITCH_UP_PIN, SWITCH_DOWN_PIN);
  ledRed = LED(LED_RED);
  ledBlue = LED(LED_BLUE);
  ledGreen = LED(LED_GREEN);


  Serial.begin(9600);
  Serial.println("LED1 remains on while you keep button pressed");
  Serial.println("LED2 blinks when you click the button");
}

void loop() {
	SpdtSwitch::Position position;
	position = sw.getPosition();
  	if (position == SpdtSwitch::MID) {
  		ledBlue.on();
  		ledRed.off();
  		ledGreen.off();
  	}
  	if (position == SpdtSwitch::UP) {
  	  		ledBlue.off();
  	  		ledRed.off();
  	  		ledGreen.on();
  	  	}
  	if (position == SpdtSwitch::DOWN) {
  	  		ledBlue.off();
  	  		ledRed.on();
  	  		ledGreen.off();
  	  	}
}
