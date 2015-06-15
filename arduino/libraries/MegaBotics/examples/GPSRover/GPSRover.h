/*
 * FUI.h
 *
 *  Created on: May 31, 2015
 *      Author: aakash
 */

#ifndef FUI_H_
#define FUI_H_
#include <MegaBotics.h>
#include "ConfigManager.h"

class GPSRover : public AutonomousRover {
public:
	GPSRover();
	virtual ~GPSRover();

	void autoRun();

	static GPSRover * getInstance() { return &_instance; }
	static GPSRover & getReference() { return _instance; }
private:
	static GPSRover _instance;

	ConfigManager *_configMgr;

	float _steer;
	float _throttle;

	void doNavigate();
	float calcSteering(Route::Location & loc);
	float calcThrottle(Route::Location & loc, float steer);
};

#endif /* FUI_H_ */
