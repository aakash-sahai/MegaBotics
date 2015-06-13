#include "PID.h"
#include <Arduino.h>

PID::PID() {
	init();
}

void PID::init(void) {
	_lastDerivative = NAN;
    _integrator = 0.0;
    _lastError = 0.0;
    _lastDerivative = 0.0;
    _lastTime = 0;
}

PID::PID(float kp, float ki, float kd, float clamp) {
    _config.Kp = kp;
    _config.Ki = ki;
    _config.Kd = kd;
    _config.clamp = clamp;
    init();
}

PID::PID(Config & config) {
	_config = config;
	init();
}

PID::~PID() {
}

float PID::getPid(float error)
{
	return getPid(error, _config.scale);
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

    output = error * _config.Kp;

    if ((fabs(_config.Kd) > 0) && (dt > 0)) {
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

        output += _config.Kd * derivative;
    }

    output *= scaler;

    if ((fabs(_config.Ki) > 0) && (dt > 0)) {
        _integrator += (error * _config.Ki) * scaler * delta_time;

        float scaledIntegratorClamp = _config.clamp * scaler;
        if (_integrator < -scaledIntegratorClamp) {
            _integrator = -scaledIntegratorClamp;
        } else if (_integrator > scaledIntegratorClamp) {
            _integrator = scaledIntegratorClamp;
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
