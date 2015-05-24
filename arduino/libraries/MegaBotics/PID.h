#ifndef PID_H_
#define PID_H_

#include <inttypes.h>
#include <math.h>

#define IDLE_TIME	1000	// Time in msec after which PID is considered IDLE if not used

class PID {
public:
	PID();

    PID(float kp, float ki, float kd, float iClamp);

    virtual ~PID();

    float  getPid(float error, float scaler = 1.0);

    void   resetIntegrator();

    float  getKp() const {
        return _Kp;
    }

    float  getKi() const {
        return _Ki;
    }

    float   getKd() const {
        return _Kd;
    }

    float  getIntegratorClamp() const {
        return _integratorClamp;
    }

    float        getDerivative() const {
        return _lastDerivative;
    }

    float        getIntegrator() const {
        return _integrator;
    }

    void  setIntegratorClamp(float clamp) {
        _integratorClamp = clamp;
    }

    void setParam(float kp, float ki, float kd, float clamp) {
    	_Kp = kp;
    	_Ki = ki;
    	_Kd = kd;
    	_integratorClamp = clamp;
    }

private:
    float        _Kp;
    float        _Ki;
    float        _Kd;
    float        _integratorClamp;

    float        _integrator;
    float        _lastError;
    float        _lastDerivative;
    uint32_t     _lastTime;

    static const uint8_t        _fCut = 20;
};


#endif /* PID_H_ */
