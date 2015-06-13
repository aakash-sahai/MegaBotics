#ifndef PID_H_
#define PID_H_

#include <inttypes.h>
#include <math.h>

#define IDLE_TIME	1000	// Time in msec after which PID is considered IDLE if not used

class PID {
public:
	struct Config {
		float Kp;
		float Ki;
		float Kd;
		float clamp;		// Integrator clamp
		float scale;
		int idleTime;

		// The defaults are for a Steering PID controller
		Config() {
			Kp = 4.0;
			Ki = 0.01;
			Kd = 0.0001;
			clamp = 10.0;
			scale = 10.0 / 36.0;
			idleTime = IDLE_TIME;
		}
	};

	PID();
    PID(float kp, float ki, float kd, float iClamp);
    PID(Config &config);
    virtual ~PID();

    void init(void);
    float  getPid(float error, float scaler = 1.0);
    float  getPid(float error);
    void   resetIntegrator();

    float  getKp() const {
        return _config.Kp;
    }

    float  getKi() const {
        return _config.Ki;
    }

    float   getKd() const {
        return _config.Kd;
    }

    float  getIntegratorClamp() const {
        return _config.clamp;
    }

    float        getDerivative() const {
        return _lastDerivative;
    }

    float        getIntegrator() const {
        return _integrator;
    }

    void  setIntegratorClamp(float clamp) {
        _config.clamp = clamp;
    }

    void setParam(float kp, float ki, float kd, float clamp) {
    	_config.Kp = kp;
    	_config.Ki = ki;
    	_config.Kd = kd;
    	_config.clamp = clamp;
    }

private:
    Config		_config;

    float		_integrator;
    float		_lastError;
    float		_lastDerivative;
    uint32_t	_lastTime;

    static const uint8_t        _fCut = 20;
};


#endif /* PID_H_ */
