/*
 * AHRS.cpp
 *
 *  Created on: Apr 15, 2015
 *      Author: aakash
 */

#include <AHRS.h>

AHRS::AHRS() {
	_baud = AHRS_DEFAULT_BAUD;
	_uport = UPort(AHRS_DEFAULT_UPORT);
	_zeroYpr.yaw = 0.0;
	_zeroYpr.pitch = 0.0;
	_zeroYpr.roll = 0.0;
}

AHRS::AHRS(int port) {
	_baud = AHRS_DEFAULT_BAUD;
	_uport = UPort(port);
	_zeroYpr.yaw = 0.0;
	_zeroYpr.pitch = 0.0;
	_zeroYpr.roll = 0.0;
}

AHRS::~AHRS() {
	// Nothing to do
}

AHRS::Status AHRS::setup(void) {
	setBaud(AHRS_DEFAULT_BAUD);
	setMode(SINGLE);
	setErrorOutput(false);
	return SUCCESS;
}

AHRS::Status AHRS::setBaud(long baud) {
	_baud = baud;
	_uport.serial().begin(_baud);
	return SUCCESS;
}

AHRS::Status AHRS::setMode(Mode mode) {
	switch (mode) {
	case SINGLE:
		_uport.serial().print("#o0");
		break;
	case STREAMING:
		_uport.serial().print("#o1");
		break;
	}
	return SUCCESS;
}

AHRS::Status AHRS::setDataType(DataType dataType) {
	switch (dataType) {
	case YPR:
		_uport.serial().print("#ob#f");
		break;
	case SENSOR_RAW:
		_uport.serial().print("#osrb#f");
		break;
	case SENSOR_CALIBRATED:
		_uport.serial().print("#oscb#f");
		break;
	case SENSOR_BOTH:
		_uport.serial().print("#osbb#f");
		break;
	}
	return SUCCESS;
}

AHRS::Status AHRS::setErrorOutput(bool tf) {
	_uport.serial().print("#oe");
	_uport.serial().print(tf);
	return SUCCESS;
}

AHRS::ypr & AHRS::getYPR(void) {
	return _ypr;
}

AHRS::amg & AHRS::getRawAMG(void) {
	return _rawAmg;
}

AHRS::amg & AHRS::getCalibratedAMG(void) {
	return _calAmg;
}

void	AHRS::poll(void) {
	fetchYPR();
	fetchRawSensor();
	fetchCalibratedSensor();
}

void	AHRS::zero(void) {
	fetchYPR();
	_zeroYpr = _ypr;
}

void AHRS::readFloats(char *buf, int n) {
	_uport.serial().readBytes(buf, n*4);
}

void AHRS::fetchYPR(void) {
	setDataType(YPR);
	_ypr.millis = millis();
	readFloats((char *)&_ypr.yaw, 3);
}

void AHRS::fetchRawSensor(void) {
	setDataType(SENSOR_RAW);
	_rawAmg.millis = millis();
	readFloats((char *)&_rawAmg.accel[0], 9);
}

void AHRS::fetchCalibratedSensor(void) {
	setDataType(SENSOR_CALIBRATED);
	_calAmg.millis = millis();
	readFloats((char *)&_calAmg.accel[0], 9);

}

size_t AHRS::ypr::printTo(Print& p) const
{
    size_t n = p.print('[');
    n += p.print(millis);
    n += p.print("] YPR: ");
    n += p.print(yaw);
    n += p.print(',');
	n += p.print(pitch);
	n += p.print(',');
	n += p.println(roll);

    return n;
}

size_t AHRS::print3Floats(Print& p, const float buf[], String type) {
    size_t n = p.print(type);
    n += p.print(buf[0]);
    n += p.print(',');
	n += p.print(buf[1]);
	n += p.print(',');
	n += p.println(buf[2]);
	return n;
}

size_t AHRS::amg::printTo(Print& p) const
{
    size_t n = AHRS::print3Floats(p, accel, "ACCEL: ");
    n += AHRS::print3Floats(p, mag, "MAG: ");
    n += AHRS::print3Floats(p, gyro, "GYRO: ");

    return n;
}
