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

#define WIFI_DEFAULT_UPORT		4
#define WIFI_DEFAULT_BAUD		115200L
#define WIFI_RST_PIN			1
#define WIFI_ENABLE_PIN			2
#define WIFI_MAX_CONNECTIONS	5
#define WIFI_RESET_DELAY		2000
#define WIFI_MAX_HOSTLEN		16

enum WiFiMode {
	MODE_STA = 1,
	MODE_AP = 2,
	MODE_BOTH = 3
};

enum WiFiEncryption {
	ENC_NONE = 0,
	ENC_EWP = 1,
	ENC_WPA_PSK = 2,
	ENC_WAP2_PSK = 3,
	ENC_WPA_WPA2_PSK = 4
};

enum WiFiConnectionStatus {
	CLOSED = 0,
	OPENING = 1,
	OPEN = 2,
	IDLE = 3,
	DATA_RECEIVED = 4,
	CLOSING = 5
};

enum WiFiStatus {
	SUCCESS = 0,
	FAILURE = 1,
	SEND_DATA = 2,
	NOT_AVAILABLE = 3,
	TIMEDOUT = 4,
	BUSY = 5,
	NOT_CONNECTED = 6
};

enum Protocol {
	TCP = 1,
	UDP = 2
};

struct Connection {
	byte	id;
	byte	status;
	int		port;
	char	host[WIFI_MAX_HOSTLEN+1];

	String toStr(void) {
		return String(id) + ",\"TCP\",\"" + String(host) + "\","  + String(port) + ':' + String(status);
	}
};

struct ListenHandlers {
	void (* connect)(byte connectionId);
	void (* disconnect)(byte connectionId);
	void (* receive)(byte connectionId, const char *data, int length, int remaining);
};

struct ApConfig {
	String ssid;
	String password;
	byte channel;
	WiFiEncryption encryption;
};

struct StaConfig {
	String ssid;
	String password;
};

class WiFi {
public:
	WiFi();
	WiFi(int port);
	virtual ~WiFi();

	WiFiStatus setup(void);

	WiFiStatus enable(void);
	WiFiStatus disable(void);

	WiFiStatus reset(void) { return hardReset(); }
	WiFiStatus softReset(void);
	WiFiStatus hardReset(void);

	WiFiStatus listen(int port, ListenHandlers *handlers);
	WiFiStatus connect(int port, String host, byte &conectionId);
	WiFiStatus disconnect(byte connectionId);
	WiFiStatus send(byte connectionId, const char *data, int length);
	WiFiStatus send(byte connectionId, String str);
	String	   connection(byte id) { return connections[id].toStr(); }

	String	 getApConfig(void);
	String	 getStaConfig(void);
	String	 getMode(void);
	String	 getStaIp(void);
	String	 getApIp(void);
	String	 getBaud(void);
	String	 getMux(void);
	String	 getStaMac(void);
	String	 getApMac(void);
	String	 getFirmwareVersion(void);

	WiFiStatus setApConfig(ApConfig &config);
	WiFiStatus setStaConfig(StaConfig &config);
	WiFiStatus setMode(WiFiMode mode);
	WiFiStatus setStaIp(String ip);
	WiFiStatus setApIp(String ip);
	WiFiStatus setBaud(long baud);
	WiFiStatus setMux();
	WiFiStatus clearMux();

	String		listAp(void);

	byte		readAnalog(void);
	String		readGpio0(void);
	String		readGpio2(void);
	WiFiStatus	writeGpio(byte pin, bool value);

	void		poll(void);

	String output;

private:
	UPort uport;
	long baud;
	int listenPort;
	ListenHandlers listenHandlers;
	Connection connections[WIFI_MAX_CONNECTIONS];
	WiFiStatus execCommand(const char *cmd);
	WiFiStatus execCommand(String cmd);
	WiFiStatus newConnection(byte &id);
	WiFiStatus response(void);
	String parseResult(String pattern);
	String parseResult(String pattern, String terminator);
	String parseResultDbg(String pattern, String terminator);
	void initConnections(void);
	void flushInput(void);
	String readGpio(byte pin);
	void process_data(const char *ptr, int len);
	byte replenish(void);
	void consume(int size);
	WiFiStatus checkBusy(void);
};

#endif /* MEGABOTICS_WIFI_H_ */
