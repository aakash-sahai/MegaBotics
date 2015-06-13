/*
 * FUI.h
 *
 *  Created on: May 31, 2015
 *      Author: aakash
 */

#ifndef FUI_H_
#define FUI_H_
#include <MegaBotics.h>

class GPSRover {
public:
	GPSRover();
	virtual ~GPSRover();

	struct Config {
		float steerKp;
		float steerKi;
		float steerKd;
		float steerClamp;
		float steerScale;

		float curiseDistThres;
		float wpProximRadius;
		uint8_t navLoopDelay;

		bool enableLogger;

	    Config() {
			steerKp = 4.0f;
			steerKi = 0.0f;
			steerKd = 0.0f;
			steerClamp = 10;		// Clamp Steering integral correction to 10 degrees
			steerScale = 10.0/36.0;

			curiseDistThres = 15.0f;
			wpProximRadius = 5.0f;
			navLoopDelay = 0;

			enableLogger = true;
		}
	};

	void setup();
	void setup(Config& config);
	void autoRun();

	void waitForClick();

	static GPSRover * getInstance() { return &_instance; }
	static GPSRover & getReference() { return _instance; }
private:
	static GPSRover _instance;
	Config _config;

	AHRS* _ahrs;
	Display* _display;
	Rover* _rover;
	JoyStick* _joystick;
	Logger* _logger;
	EepromStore* _estore;
	Route* _route;
	PID* _steeringPid;

	float _steer;
	float _throttle;

	void doNavigate();
	float calcSteering(Route::Location & loc);
	float calcThrottle(Route::Location & loc, float steer);
};

#endif /* FUI_H_ */
