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
 *		may be used to endorse or promote products derived from this software
 *		without specific prior written permission.
 */

/*
 * Module:		RFXCVR.h - The driver for RF transceiver module
 * Created on:	Apr 28, 2015
 * Author:		Aakash Sahai
 *
 * Currently supoprts and tested with Nordic Semiconductor nRFL2401 XCVR but other
 * transceivers supported by the RadioHead libraries should work too.
 * The RF XCVR module can be plugged into any S-Port.
 *
 * The pin mapping from S-Port to nRFL2401 RF XCVR module is as follows:
 *
 * S-Port		Signal		nRFL2401 Module
 * 1 (BROWN)	GND			GND		(pin 1)
 * 2 (RED)		+3.3V		VCC		(pin 2)
 * 3 (ORANGE)	GPIO		CE		(pin 3) => Chip Enable
 * 4 (YELLOW)	SS			CSN		(pin 4) => Chip Select (SPI SS)
 * 5 (GREEN)	SCK			SCK		(pin 5) => SPI Clock
 * 6 (BLUE)		MOSI		MOSI	(pin 6) => SPI Master Out Slave In
 * 7 (PURPLE)	MISO		MISO	(pin 7) => SPI Master In Slave Out
 * 8 (GREY)		ANALOG		IRQ		(pin 8) => Interrupt (Not Used)
 * 9 (WHITE)	GND			Not connected
 * 10 (BLACK)	+5V			Not connected
 *
 * The nRFL2401 module operates at 3.3V but the I/O pins are 5V tolerant so it can
 * be connected to any SPI port. By default we use S-Port #1.
 */

#ifndef MEGABOTICS_RFXCVR_H_
#define MEGABOTICS_RFXCVR_H_

#include <SPI.h>
#include <RH_NRF24.h>
#include <RHReliableDatagram.h>
#include <MegaBotics.h>

#define RFXCVR_DEFAULT_SPORT		1
#define RFXCVR_DEFAULT_CHANNEL		1
#define RFXCVR_DEFAULT_NODE			2
#define RFXCVR_MAX_STREAMS			16
#define RFXCVR_MAX_CONNECTIONS		16

class RFXCVR {
public:
	enum Type {
		UNRELIABLE = 0,
		RELIABLE = 1
	};

	enum Status {
		SUCCESS = 0,
		FAILURE = 1,
		NOT_OPEN = 2,
		TIMEDOUT = 3,
		INVALID = 4
	};

	struct Config {
		byte sPort;
		byte channel;
		byte node;
	};

	typedef void (*Callback)(byte from, byte to, const byte *data, byte len);

	virtual ~RFXCVR();

	static RFXCVR * getInstance() { return &_instance; }
	static RFXCVR & getReference() { return _instance; }

	Status setup(void);
	Status setup(Config config);

	byte connect(Type type, byte streamId, byte nodeId);	// Opens a connection and returns the connection ID
	void disconnect(byte conectionId);
	Status send(byte conectionId, byte *buf, int len);
	Status onReceive(byte streamId, Callback fn);

	void poll(void);

private:
	enum ConnectionState {
		CLOSED = 0,
		OPEN = 1
	};

	struct Connection {
		byte		stream;
		byte		node;
		Type		type;
		ConnectionState	state;
	};

	static RFXCVR _instance;

	Config _config;
	SPort * _sPort;
	Connection _connections[RFXCVR_MAX_CONNECTIONS];
	Callback _recvCallback[RFXCVR_MAX_STREAMS];
	RH_NRF24 * _nrf24;
	RHReliableDatagram *_manager;

	Status init(void);
	RFXCVR();
	RFXCVR(RFXCVR const&);              // Don't Implement to disallow copy by assignment
    void operator=(RFXCVR const&);		// Don't implement to disallow copy by assignment
};

#endif /* MEGABOTICS_RFXCVR_H_ */
