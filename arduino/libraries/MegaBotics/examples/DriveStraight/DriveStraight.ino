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
 * The calibration routine for calibrating the Rover. The details of the setup
 * and instructions can be found on the project Wiki.
 */

#include <MegaBotics.h>
#include <Rover.h>

#define DRIVE_STRAIGHT 1

#if DRIVE_STRAIGHT

#define KP		2.0
#define KI		0.1
#define KD		0.01
#define SCALE	(RAD_TO_DEG * 20 / 36)
#define ICLAMP	(PI / 8)		// Clamp Steering integral correction to PI / 8

Rover& rover = Rover::getReference();
WheelEncoder& encoder = WheelEncoder::getReference();
EepromStore& estore = EepromStore::getReference();
Logger& logger = Logger::getReference();
PID steeringPid(KP, KI, KD, ICLAMP);

AHRS ahrs;
Display display;
InputPanel panel;
float totalDistance = 0.0;
float error;
int8_t steerVal;
float currentHdg;

float calcDistAndHdg(float & D, float & hdg) {
	float d = encoder.getDistance();
	totalDistance += d;
	encoder.reset();
	currentHdg = DEG2RAD(ahrs.getOrientation());
	float theta = currentHdg - hdg;

	float dsin = fabs(d * sin (theta));
	float dcos = fabs(d * cos (theta));
	float phi = atan2(dsin, (D - dcos));
	float sinphi = sin(phi);
	if (sinphi != 0.0) {
		D = dsin / sinphi;
	} else {
		D -= d;
	}
	float error = -(theta + phi);
	hdg = currentHdg +  error;
	return error;
}

float relativeYaw(float yaw, float steer) {
	if (steer < 0 && yaw > 5.0) {
		Serial.print("left correction, "); Serial.println(yaw - 360);
		return yaw - 360;
	} else if (steer > 0 && yaw < -5.0) {
		Serial.print("right correction, "); Serial.println(yaw + 360);
		return yaw + 360;
	}

	Serial.print("no correction, "); Serial.println(yaw);
	return yaw;
}

float calcSpeed(float speed, float distance) {
	float rampDownDistance = 10.0f;

	float error = distance - encoder.getDistance();
	if (error > rampDownDistance) {
		return speed;
	}

	float calcSpeed = (error / rampDownDistance) * speed * 0.5;
	return calcSpeed;
}

float calcSteer() {
	float steerError = ahrs.getRelativeYPR().yaw;

	if (steerError < 5) {
		Serial.print(ahrs.getRelativeYPR().yaw);Serial.print("|");Serial.println(0.0f);
		return 0.0f;
	}

	float calcSteer = steerError * -4;
	Serial.print(ahrs.getRelativeYPR().yaw);Serial.print("|");Serial.println(calcSteer);
	return calcSteer;
}




void straight(float distance, float speed, boolean correction) {
	ahrs.resetYPR();
	encoder.reset();

	rover.throttle(speed);
	float hdg = 0.0f;

	while (distance > 1.0f) {
		if (correction) {
			error = calcDistAndHdg(distance, hdg);
			steerVal = (int8_t)steeringPid.getPid(error, SCALE);
			logger.begin(Logger::LEVEL_DEBUG, F("RUN")).
							nv(F(" TOTALDIST"), totalDistance).
							nv(F("DIST"), distance).
							nv(F(" CURHDG "), currentHdg).
							nv(F(" HDG "), hdg).
							nv(F(" ERROR "), error).
							nv(F(" STEER "), (int)steerVal).endln();
			rover.steer(steerVal);
			rover.throttle(speed);
		}

		delay(5);
	}
}

void turn(float degrees, float steer, float speed) {
	ahrs.resetYPR();
	encoder.reset();

	rover.throttle(speed);
	rover.steer(steer);

	while (abs(relativeYaw(ahrs.getRelativeYPR().yaw, steer)) < degrees) {
		rover.throttle(speed);
		rover.steer(steer);

		delay(5);
		ahrs.poll();
	}

	rover.straight();
}

//The setup function is called once at startup of the sketch
void setup() {
	Serial.begin(57600);
	// estore.setup() should be called before rover.setup()
	estore.setup();

	ahrs.setup();
	display.setup();
	encoder.setup();
	panel.setup();
	logger.setup();
	logger.autoFlush(true);
	logger.enable(Logger::LOG_SERIAL);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);

	rover.setup();
	rover.setControlMode(Rover::MANUAL);

	panel.waitForClick();
	rover.setControlMode(Rover::AUTO);

	straight(50, 10, true);

	rover.stop();

	rover.setControlMode(Rover::MANUAL);
}

// The loop function is called in an endless loop
void loop() {

}

#endif
