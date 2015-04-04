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
 * The calibration routine for calibrating the Rover. The details of the setup
 * and instructions can be found on the project Wiki.
 */
#include <Servo.h>
#include <MegaBotics.h>

HardwareSerial * terminal;

WiFi wifi;

#define INTERACT 1

#define TERMINAL_UPORT	1

void doConnect(Connection &connection)
{
	terminal->print("Inbound Connection: ");
	terminal->println(connection.toStr());
}

void doDisconnect(byte connectionId) {
	terminal->print("Disconnecting from: ");
	terminal->println(wifi.connection(connectionId));
}

void doReceive(char *data, int length) {
	terminal->print("Received data: <<<");
	for (int i = 0; i < length; i++) {
		terminal->print(data[i]);
	}
	terminal->println(">>>");
}

void setup() {
	UPort uPort = UPort(TERMINAL_UPORT);
	terminal = uPort.serial();
	terminal->begin(115200);

	wifi = WiFi(WIFI_DEFAULT_UPORT);
	wifi.setup();

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
	io();
}

void info() {
	terminal->println("-----------------------------");
	terminal->print("FIRMWARE VERSION: ");
	terminal->println(wifi.getFirmwareVersion());
	terminal->print("STA IP: ");
	terminal->println(wifi.getStaIp());
	terminal->print("AP IP: ");
	terminal->println(wifi.getApIp());
	terminal->print("STA MAC: ");
	terminal->println(wifi.getStaMac());
	terminal->print("AP MAC: ");
	terminal->println(wifi.getApMac());
	terminal->print("BAUD: ");
	terminal->println(wifi.getBaud());
	terminal->print("MUX: ");
	terminal->println(wifi.getMux());
	terminal->print("MODE: ");
	terminal->println(wifi.getMode());
	terminal->print("AP CONFIG: ");
	terminal->println(wifi.getApConfig());
	terminal->print("STA CONFIG: ");
	terminal->println(wifi.getStaConfig());
	terminal->print("ANALOG: ");
	terminal->println(wifi.readAnalog());
	terminal->print("READ DIGITAL 0: ");
	terminal->println(wifi.readGpio0());
	terminal->print("READ DIGITAL 2: ");
	terminal->println(wifi.readGpio2());
	terminal->println("WRITE DIGITAL 0: [LOW]");
	wifi.writeGpio(0, 0);
	terminal->print("READ DIGITAL 0: ");
	terminal->println(wifi.readGpio0());
}

void io(void) {
	byte id;
	WiFiStatus status;
	ListenHandlers handlers;

	handlers.connect = doConnect;
	handlers.disconnect = doDisconnect;
	handlers.receive = doReceive;

	status = wifi.listen(80, &handlers);	// Listen on port 80 for inbound connections
	if (status == SUCCESS) {
		terminal->println("Listening on port 80");
	} else {
		terminal->println("Listen Failed");
	}
	showConnections();

	status = wifi.connect(80, "216.58.217.46", id);		// Connect to Google
	if (status == SUCCESS) {
		terminal->print("Connected, ID = ");
		terminal->println(id);
	} else {
		terminal->println("Connect Failed");
	}
	showConnections();

	status = wifi.disconnect(id);
	if (status == SUCCESS) {
		terminal->println("Disconnected");
	} else {
		terminal->println("Disconnect Failed");
	}
	showConnections();
}

void showConnections(void) {
	for (byte i = 0; i < WIFI_MAX_CONNECTIONS; i++) {
		terminal->println(wifi.connection(i));
	}
}

void configAp() {
	struct ApConfig apConfig;

	apConfig.ssid = "MEGABOTICS";
	apConfig.password = "r@n$0mone";
	apConfig.channel = 1;
	apConfig.encryption = ENC_WPA_WPA2_PSK;
	wifi.setApConfig(apConfig);
}

void configSta() {
	struct StaConfig staConfig;

	staConfig.ssid = "YourSSID";
	staConfig.password = "YourPassword";
	wifi.setStaConfig(staConfig);
}

String save = "";

void loop() {
#if	INTERACT
	interact();
#else
	info();
	delay(5000);
#endif
}

void interact(void) {
	int ch;
	int ret;

	UPort uPort = UPort(WIFI_DEFAULT_UPORT);
	HardwareSerial * wifiSerial = uPort.serial();

	while (true) {
		if (terminal->available()) {
			ch = terminal->read();
			wifiSerial->write(ch);
		}

		if (wifiSerial->available()) {
			String resp = wifiSerial->readStringUntil('\n');
			if ((ret = resp.indexOf("OK")) >= 0) {
				terminal->println("GOT OK");
				save = "";
			} else if ((ret = resp.indexOf("ERROR")) >= 0) {
				terminal->println("GOT ERROR");
				save = "";
			} else if ((ret = resp.indexOf("+IPD,")) >= 0) {
				terminal->println("GOT DATA");
				terminal->print(resp);
				save = "";
			} else {
				save = save + resp;
				terminal->print(">");
				terminal->print(save);
			}
		}
	}
}
