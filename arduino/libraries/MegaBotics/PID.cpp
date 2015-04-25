#include "PID.h"
#include <Arduino.h>

PID::PID() {
    _Kp = 1.0;
    _Ki = 1.0;
    _Kd = 1.0;
    _integratorClamp = 0;
	_lastDerivative = NAN;
    _integrator = 0.0;
    _lastError = 0.0;
    _lastDerivative = 0.0;
    _lastTime = 0;
}

PID::PID(float kp, float ki, float kd, int16_t iClamp) {
    _Kp = kp;
    _Ki = ki;
    _Kd = kd;
    _integratorClamp = iClamp;
	_lastDerivative = NAN;
    _integrator = 0.0;
    _lastError = 0.0;
    _lastDerivative = 0.0;
    _lastTime = 0;
}

PID::~PID() {
}

float PID::getPid(float error, float scaler)
{
    uint32_t tnow = millis();
    uint32_t dt = tnow - _lastTime;
    float output;
    float delta_time;

    if (_lastTime == 0 || dt > IDLE_TIME) {
        dt = 0;
		resetIntegrator();
    }
    _lastTime = tnow;

    delta_time = (float)dt / 1000.0f;

    output = error * _Kp;

    if ((fabs(_Kd) > 0) && (dt > 0)) {
        float derivative;

		if (isnan(_lastDerivative)) {
			derivative = 0;
			_lastDerivative = 0;
		} else {
			derivative = (error - _lastError) / delta_time;
		}

        float RC = 1/(2 * PI * _fCut);
        derivative = _lastDerivative +
                     ((delta_time / (RC + delta_time)) *
                      (derivative - _lastDerivative));

        _lastError = error;
        _lastDerivative = derivative;

        output += _Kd * derivative;
    }

    output *= scaler;

    if ((fabs(_Ki) > 0) && (dt > 0)) {
        _integrator += (error * _Ki) * scaler * delta_time;
        if (_integrator < -_integratorClamp) {
            _integrator = -_integratorClamp;
        } else if (_integrator > _integratorClamp) {
            _integrator = _integratorClamp;
        }
        output += _integrator;
    }

    return output;
}

void
PID::resetIntegrator()
{
    _integrator = 0;
    _lastDerivative = NAN;
}
