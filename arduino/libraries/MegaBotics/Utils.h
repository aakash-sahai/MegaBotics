/*
 * Utils.h
 *
 *  Created on: May 11, 2015
 *      Author: sraj
 */

#ifndef MEGABOTICS_UTILS_H_
#define MEGABOTICS_UTILS_H_

class Utils {
public:
	static int8_t clamp(int8_t value, int8_t min, int8_t max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	static int8_t clamp(uint8_t value, uint8_t min, uint8_t max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	static int16_t clamp(int16_t value, int16_t min, int16_t max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	static float clamp(float value, float min, float max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	static void waitForEver() {
		while(true) {
			delay(1000);
		}
	}

	static float normalizeAngle(float angle) {
		if (angle < -180) {
			angle += 360;
		} else if (angle > 180) {
			angle -= 360;
		}
		return angle;
	}

	static void cross(UPort & port, UPort &terminalPort) {
		port.cross(terminalPort);
		terminalPort.interact();
		port.uncross();
	}

private:
	Utils() {;}
	virtual ~Utils() {;}
};

#endif /* MEGABOTICS_UTILS_H_ */
