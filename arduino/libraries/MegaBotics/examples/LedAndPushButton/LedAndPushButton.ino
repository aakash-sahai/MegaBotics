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
 * This is an example sketch to demonstrate the functionality of
 * the LED abd PushButton objects.
 */
#include <Servo.h>
#include <MegaBotics.h>

const int BUTTON_PIN = 2;     // the pushbutton pin
const int LED1_PIN =  13;     // the first LED pin
const int LED2_PIN =  12;     // the second LED pin

PushButton button;
LED led1;
LED led2;

void setup() {
  button = PushButton(BUTTON_PIN);
  led1 = LED();		// By default LED uses pin 13!
  led2 = LED(LED2_PIN);

  Serial.begin(9600);
  Serial.println("LED1 remains on while you keep button pressed");
  Serial.println("LED2 blinks when you click the button");
}

void loop() {
  button.check();
  
  if (button.pressed()) {
	led1.on();
	Serial.println("Button Pressed: " );
  } else {
	led1.off();
  }
  
  bool clicked = button.clicked();
  
  if (clicked) {
	led2.blink();
	Serial.print("Button Clicked: " );
	Serial.print(button.timesClicked());
	Serial.println(" times." );
  } else {
	led2.off();
  }
  
  /*
   * The PushButton object keeps track of the number of times the button
   * has been clicked. This count can be programmatically reset as follows.
   */
  if (button.timesClicked() >= 10) {
	button.clear();
  }
}
