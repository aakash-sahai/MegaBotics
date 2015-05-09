/*
 * RFXCVR.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: aakash
 */

#if 0

#include <RFXCVR.h>

RFXCVR RFXCVR::_instance;

RFXCVR::RFXCVR() {
	_sPort = 0;
	_nrf24 = 0;
	_manager = 0;
	for (int i = 0; i < RFXCVR_MAX_CONNECTIONS; i++) {
		_connections[i].state = CLOSED;
	}
	for (int i = 0; i < RFXCVR_MAX_STREAMS; i++) {
		_recvCallback[i] = 0;
	}
}

RFXCVR::~RFXCVR() {
}

RFXCVR::Status RFXCVR::setup(void) {
	_config.channel = RFXCVR_DEFAULT_CHANNEL;
	_config.node = RFXCVR_DEFAULT_NODE;
	_config.sPort = RFXCVR_DEFAULT_SPORT;
	return init();
}

RFXCVR::Status RFXCVR::setup(Config config) {
	_config = config;
	return init();
}

RFXCVR::Status RFXCVR::init(void) {
	Logger &logger = Logger::getReference();
	_sPort = new SPort(_config.sPort);
	_nrf24 = new RH_NRF24(_sPort->getSlaveSelectPin(),
			_sPort->getDigitalPin(1));
	if (!_nrf24->init()) {
		logger.log(Logger::LEVEL_CRITICAL, F("RFXCVR"), "RH_NRF24 init failed");
		return FAILURE;
	}
	if (!_nrf24->setChannel(_config.channel)) {
		logger.log(Logger::LEVEL_CRITICAL, F("RFXCVR"), "RH_NRF24 setChannel failed");
		return FAILURE;
	}
	if (!_nrf24->setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
		logger.log(Logger::LEVEL_CRITICAL, F("RFXCVR"), "RH_NRF24 setRF failed");
		return FAILURE;
	}
	_manager = new RHReliableDatagram(*_nrf24, _config.node);
	if (!_manager->init()) {
		logger.log(Logger::LEVEL_CRITICAL, F("RFXCVR"), "RHReliableDatagram init failed");
		return FAILURE;
	}
	return SUCCESS;
}

byte RFXCVR::connect(Type type, byte streamId, byte nodeId) {
	byte i;

	for (i = 0; i < RFXCVR_MAX_CONNECTIONS; i++) {
		if (_connections[i].state == CLOSED) {
			break;
		}
	}
	if (i == RFXCVR_MAX_CONNECTIONS)
		return (byte)-1;
	_connections[i].state = OPEN;
	_connections[i].node = nodeId;
	_connections[i].stream = streamId;
	return i;
}

void RFXCVR::disconnect(byte connectionId) {
	if (connectionId < 0 || connectionId >= RFXCVR_MAX_CONNECTIONS)
		return;
	_connections[connectionId].state = CLOSED;
}

RFXCVR::Status RFXCVR::send(byte connectionId, byte *buf, int len) {
	if (connectionId < 0 || connectionId >= RFXCVR_MAX_CONNECTIONS)
		return INVALID;
	if (_connections[connectionId].state == CLOSED) {
		return NOT_OPEN;
	}
	byte *ptr = buf;
	while (len > 0) {
		int sendLen = (len > RH_NRF24_MAX_MESSAGE_LEN) ? RH_NRF24_MAX_MESSAGE_LEN : len;
		if (_connections[connectionId].type == UNRELIABLE) {
			if (!_manager->sendto(ptr, sendLen, _connections[connectionId].node))
				return FAILURE;
		} else {
			if (!_manager->sendtoWait(ptr, sendLen, _connections[connectionId].node))
				return FAILURE;
		}
		len -= sendLen;
		ptr += sendLen;
	}
	return SUCCESS;
}

RFXCVR::Status RFXCVR::onReceive(byte streamId, Callback fn) {
	if (streamId < 0 || streamId >= RFXCVR_MAX_STREAMS)
		return INVALID;
	_recvCallback[streamId] = fn;
	return SUCCESS;

}

void RFXCVR::poll(void) {
	static byte buf[RH_NRF24_MAX_MESSAGE_LEN];
	byte len = sizeof(buf);
	byte from, to, id, flags;

	if (_manager->available()) {
		if (_manager->recvfrom(buf, &len, &from, &to, &id, &flags)) {
			byte streamId = flags & 0x0F;
			if (_recvCallback[streamId]) {
				_recvCallback[streamId](from, to, buf, len);
			}
		}
	}
}

#endif
