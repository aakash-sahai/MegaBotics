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
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
 */

/*
 * An example sketch to test ESP8266 WiFi module.
 */
#include <Servo.h>
#include <MegaBotics.h>

#define WIFI_TEST 	0
#define GOOGLE_TEST	0

#if WIFI_TEST

#define TERMINAL_UPORT	1

UPort terminalPort = UPort(TERMINAL_UPORT);
HardwareSerial &terminal = terminalPort.serial();

WiFi & wifi = WiFi::getReference();
byte openId = 255;

void doConnect(byte connectionId)
{
	terminal.print("Inbound Connection: ");
	terminal.println(wifi.connection(connectionId));
	openId = connectionId;
}

void doDisconnect(byte connectionId) {
	terminal.print("Disconnecting from: ");
	terminal.println(wifi.connection(connectionId));
	openId = 255;
}

void doReceive(byte connectionId, const char *data, int length, int remaining) {
	terminal.print("RX ");
	terminal.print(length);
	terminal.print(" bytes on ");
	terminal.print(connectionId);
	terminal.print(" remaining ");
	terminal.print(remaining);

	terminal.print(" <<<");
	for (int i = 0; i < length; i++) {
		terminal.print(data[i]);
	}
	terminal.println(">>>");
}

void info() {
	terminal.println("-----------------------------");
	terminal.print("STA IP: ");
	terminal.println(wifi.getStaIp());
	terminal.print("AP IP: ");
	terminal.println(wifi.getApIp());
	terminal.print("STA MAC: ");
	terminal.println(wifi.getStaMac());
	terminal.print("AP MAC: ");
	terminal.println(wifi.getApMac());
	terminal.print("BAUD: ");
	terminal.println(wifi.getBaud());
	terminal.print("MUX: ");
	terminal.println(wifi.getMux());
	terminal.print("MODE: ");
	terminal.println(wifi.getMode());
	terminal.print("AP CONFIG: ");
	terminal.println(wifi.getApConfig());
	terminal.print("STA CONFIG: ");
	terminal.println(wifi.getStaConfig());
	terminal.print("ANALOG: ");
	terminal.println(wifi.readAnalog());
	terminal.print("READ DIGITAL 0: ");
	terminal.println(wifi.readGpio0());
	terminal.print("READ DIGITAL 2: ");
	terminal.println(wifi.readGpio2());
	terminal.println("WRITE DIGITAL 0: [LOW]");
	wifi.writeGpio(0, 0);
	terminal.print("READ DIGITAL 0: ");
	terminal.println(wifi.readGpio0());
}

void serve80(void) {
	WiFiStatus status;
	ListenHandlers handlers;

	handlers.connect = doConnect;
	handlers.disconnect = doDisconnect;
	handlers.receive = doReceive;

	status = wifi.listen(80, &handlers);	// Listen on port 80 for inbound connections
	if (status == SUCCESS) {
		terminal.println("Listening on port 80");
	} else {
		terminal.println("Listen Failed");
	}
}

void client(void) {
#if GOOGLE_TEST
	WiFiStatus status;
	byte id;

	status = wifi.connect(TCP, "216.58.217.46", 80, id);		// Connect to Google
	if (status == SUCCESS) {
		terminal.print("Connected, ID = ");
		terminal.println(id);
	} else {
		terminal.println("Connect Failed");
	}
	showConnections();

	status = wifi.send(id, "GET /\n");
	if (status == SUCCESS) {
		terminal.print("Sent Request");
	} else {
		terminal.println("Send Failed");
	}

	status = wifi.disconnect(id);
	if (status == SUCCESS) {
		terminal.println("Disconnected");
	} else {
		terminal.println("Disconnect Failed");
	}
	showConnections();
#endif
}

void showConnections(void) {
	for (byte i = 0; i < WIFI_MAX_CONNECTIONS; i++) {
		terminal.println(wifi.connection(i));
	}
}

void configAp() {
	struct ApConfig apConfig;

	apConfig.ssid = "MEGABOTICS";
	apConfig.password = "r@n$0mone";
	apConfig.channel = 6;
	apConfig.encryption = ENC_WPA_WPA2_PSK;
	wifi.setApConfig(apConfig);
}

void configSta() {
	struct StaConfig staConfig;

	staConfig.ssid = "YourSSID";
	staConfig.password = "YourPassword";
	wifi.setStaConfig(staConfig);
}

const char *sendStr = (const char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";

void sendData(void) {
	if (openId != 255) {
		int len = strlen(sendStr);
		terminal.print("Send returned: ");
		terminal.println(wifi.send(openId, sendStr, len));
	}
}

void writeData(void) {
	if (openId != 255) {
		int len = strlen(sendStr);
		for (int i = 0; i < len; i++) {
			wifi.write(openId, sendStr[i]);
		}
		terminal.print("Flush returned: ");
		terminal.println(wifi.flush(openId));
	}
}

Logger & logger = Logger::getReference();

void log(void) {
	logger.start("Starting the log");
	logger.log(Logger::LEVEL_INFO, F("INFO"), "Hello, World!");
	logger.flush();
	logger.begin(Logger::LEVEL_DEBUG, F("DEBUG")).nv(F("ONE"), 1).nv(F("TWO"), (char *)"22").nv(F("THREE"), 3.3333).endln();
	logger.flush();
	logger.finish("Finishing the log");
	logger.flush();
}

void setup() {
	terminal.begin(115200);
	wifi.setup();

	Logger::Config config;
	config.bufsize = 128;
	strcpy(config.ip, "192.168.4.2");
	config.port = 8080;
	logger.setup(config);
	logger.enable(Logger::LOG_SERIAL);
	logger.enable(Logger::LOG_UDP);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);
	logger.setLevel(Logger::LOG_UDP, Logger::LEVEL_DEBUG);

	DBG_PRINTLN("WIFI: CONFIG AP");
	configAp();
	delay(5000);

	/*
	 * Uncomment the following two lines after configuring your SSID and Password in configSta()
	 * Note that you need to configure your SSID only once as the WiFi chip persists it in internal Flash/EEPROM.
	 *
	 * configSta();
	 * delay(5000);
	 *
	 */

	info();
	serve80();
	client();
}

void loop() {
	wifi.poll();
	if (terminal.available()) {
		int ch = terminal.read();
		switch(ch) {
		case 'w':
			writeData();
			break;
		case 's':
			sendData();
			break;
		case 'r':
			wifi.setup();
			serve80();
			break;
		case 'c':
			showConnections();
			break;
		case 'i':
			info();
			break;
		case 'l':
			log();
			break;
		case 'x':
			cross();
			break;
		}
	}
}

void cross(void) {
	wifi.getUPort().cross(terminalPort);
	terminalPort.interact();
	wifi.getUPort().uncross();
}

#endif
