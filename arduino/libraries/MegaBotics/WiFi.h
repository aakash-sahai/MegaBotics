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
 * Module:		WiFi.h - The driver for ESP8266 WiFi module
 * Created on:	Mar 27, 2015
 * Author:		Aakash Sahai
 *
 * The WiFi module can be plugged into any U-Port. Care should be taken not to plug
 * into U-Port 1 if MegaBotics platform is connected to the USB port to a computer
 * for the purpose of programming.
 *
 * The pin mapping from U-Port to WiFi module is as follows:
 *
 * U-Port		Signal		WiFi Module
 * 1 (BROWN)	 GND		Not connected
 * 2 (RED)		+3.3V		VCC		(pin 1)
 * 3 (ORANGE)	RXIN		UTXD	(pin 7) => UART Communication
 * 4 (YELLOW)	TXOUT		URXD	(pin 2) => UART Communication
 * 5 (GREEN)	GPIO1		RST		(pin 3) => HIGH; Pull LOW to hard reset
 * 6 (BLUE)		GPIO2		CH_PD	(pin 5) => HIGH; Pull HIGH to chip select i.e. enable module
 * 7 (PURPLE)	ANALOG		GPIO0	(pin 4) => Floating; PULL LOW to program Flash
 * 8 (GREY)		PWMIN		GPIO2	(pin 6) => Floating; may use for communication with module
 * 9 (WHITE)	GND			GND		(pin 8)
 * 10 (BLACK)	+5V			Not connected
 *

 */

#ifndef MEGABOTICS_WIFI_H_
#define MEGABOTICS_WIFI_H_

#include <String.h>
#include <Uport.h>

#define WIFI_DEFAULT_BAUD		115200L
#define WIFI_RST_PIN			1
#define WIFI_ENABLE_PIN			2
#define WIFI_MAX_CONNECTIONS	5

enum WiFiMode {
	MODE_STA = 1,
	MODE_AP = 2,
	MODE_BOTH = 3
};

enum WiFiConnectionStatus {
	AVAILABLE = 0,
	OPENING = 1,
	OPEN = 2,
	IDLE = 3,
	DATA_RECEIVED = 4,
	CLOSING = 5,
	CLOSED = 6
};


enum WiFiStatus {
	SUCCESS = 0,
	FAILURE = 1,
	NOT_AVAILABLE = 2,
	COMM_ERROR = 3
};

enum Protocol {
	TCP = 1,
	UDP = 2
};

struct ListenHandlers {
	void *(connect)(int connectionId);
	void *(disconnect)(int connectionId);
	void *(receive)(char *data, int length);
};

struct ApConfig {
	char *ssid;
	char *password;
};

struct StaConfig {
	char *ssid;
	char *password;
};

struct Connection {
	byte	id;
	byte	status;
};

class WiFi {
public:
	WiFi();
	WiFi(int port);
	virtual ~WiFi();

	WiFiStatus setup(void);
	WiFiStatus setupAP(ApConfig config);
	WiFiStatus setupSTA(StaConfig config);

	WiFiStatus enable(void);
	WiFiStatus disable(void);

	WiFiStatus reset(void) { return hardReset(); }
	WiFiStatus softReset(void);
	WiFiStatus hardReset(void);

	WiFiStatus listen(Protocol proto, int port, ListenHandlers &handlers, byte &conectionId);
	WiFiStatus connect(Protocol proto, int port, byte &conectionId);
	WiFiStatus disconnect(int connectionId);
	WiFiStatus send(byte connectionId, char *data, int length);
	WiFiStatus send(byte connectionId, String str);

	WiFiStatus setMode(WiFiMode mode);

	const char *getStaIp(void);
	const char *getApIp(void);
	WiFiStatus setStaIp(char *ip);
	WiFiStatus setApIp(char *ip);

	const char *getStaMac(void);
	const char *getApMac(void);

	const char *listAp(void);

	WiFiStatus setBaud(long baud);
	long getBaud(void);

private:
	UPort uport;
	WiFiMode currentMode;
	long baud;
	Connection connections[WIFI_MAX_CONNECTIONS];
	WiFiStatus execCommand(const char *cmd);
	WiFiStatus execCommand(String cmd);
	WiFiStatus newConnection(byte &id);
};

#endif /* MEGABOTICS_WIFI_H_ */
