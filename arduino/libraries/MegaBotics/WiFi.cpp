/*
 * WiFi.cpp
 *
 *  Created on: Mar 27, 2015
 *      Author: aakash
 */

#include <WiFi.h>

WiFi::WiFi() {
	baud = WIFI_DEFAULT_BAUD;
	uport = UPort(WIFI_DEFAULT_UPORT);
	output = "";
}

WiFi::WiFi(int port) {
	baud = WIFI_DEFAULT_BAUD;
	uport = UPort(port);
	output = "";
}

WiFi::~WiFi() {
	// Nothing to do
}

WiFiStatus WiFi::setup(void) {
	pinMode(uport.getDigitalPin(WIFI_RST_PIN), OUTPUT);
	pinMode(uport.getDigitalPin(WIFI_ENABLE_PIN), OUTPUT);
	pinMode(uport.getAnalogPin(), INPUT);
	uport.serial()->begin(baud);
	hardReset();
	flushInput();
	enable();
	setMode(MODE_BOTH);
	setMux();
	return SUCCESS;
}

WiFiStatus WiFi::enable(void) {
	digitalWrite(uport.getDigitalPin(WIFI_ENABLE_PIN), HIGH);
	digitalWrite(uport.getDigitalPin(WIFI_RST_PIN), HIGH);
	return SUCCESS;
}

WiFiStatus WiFi::disable(void) {
	digitalWrite(uport.getDigitalPin(WIFI_ENABLE_PIN), LOW);
	return SUCCESS;
}

WiFiStatus WiFi::softReset(void) {
	return execCommand("AT+RST");
	delay(WIFI_RESET_DELAY);
}

WiFiStatus WiFi::hardReset(void) {
	byte rstPin = uport.getDigitalPin(WIFI_RST_PIN);
	digitalWrite(rstPin, LOW);
	delay(100);
	digitalWrite(rstPin, HIGH);
	delay(WIFI_RESET_DELAY);
	return SUCCESS;
}

String WiFi::parseResult(String pattern) {
	return parseResult(pattern, "\r");
}

WiFiStatus WiFi::setApConfig(ApConfig &config) {
	char cmd[100];
	sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",%d,%d",
			config.ssid.c_str(), config.password.c_str(),
			(int)config.channel, (int)config.encryption);
	Serial.println(cmd);
	return execCommand(cmd);
}

WiFiStatus WiFi::setStaConfig(StaConfig &config) {
	char cmd[100];
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"",
			config.ssid.c_str(), config.password.c_str());
	Serial.println(cmd);
	return execCommand(cmd);
}

String WiFi::getApConfig(void) {
	WiFiStatus resp = execCommand("AT+CWSAP?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CWSAP:");
	}
}

String WiFi::getStaConfig(void) {
	WiFiStatus resp = execCommand("AT+CWJAP?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CWJAP:");
	}
}

String WiFi::parseResult(String pattern, String terminator) {
	int begin;
	if ((begin = output.indexOf(pattern)) >= 0) {
		output.remove(0, begin + pattern.length());
	}
	begin = output.indexOf(terminator);
	output.remove(begin);
	return output;
}

String WiFi::getMode(void) {
	WiFiStatus resp = execCommand("AT+CWMODE?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CWMODE:");
	}
}

WiFiStatus WiFi::setMode(WiFiMode mode) {
	String cmd = "AT+CWMODE=" + String(mode);
	Serial.println(cmd);
	return execCommand(cmd);
}

String WiFi::getStaIp(void) {
	WiFiStatus resp = execCommand("AT+CIPSTA?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CIPSTA:\"", "\"");
	}
}

String WiFi::getApIp(void) {
	WiFiStatus resp = execCommand("AT+CIPAP?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CIPAP:\"", "\"");
	}
}

WiFiStatus WiFi::setStaIp(String ip) {
	String cmd = "AT+CIPSTA=" + String(ip);
	return execCommand(cmd);;
}

WiFiStatus WiFi::setApIp(String ip) {
	String cmd = "AT+CIPAP=" + String(ip);
	return execCommand(cmd);
}

String WiFi::getStaMac(void) {
	WiFiStatus resp = execCommand("AT+CIPSTAMAC?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CIPSTAMAC:\"", "\"");
	}
}

String WiFi::getApMac(void) {
	WiFiStatus resp = execCommand("AT+CIPAPMAC?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CIPAPMAC:\"", "\"");
	}
}

WiFiStatus WiFi::setBaud(long baud) {
	char buf[64];
	sprintf(buf, "AT+CIOBAUD=%ld", baud);
	return execCommand(buf);
}

String WiFi::getBaud(void) {
	WiFiStatus resp = execCommand("AT+CIOBAUD?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CIOBAUD:");
	}
}

WiFiStatus WiFi::setMux() {
	return execCommand("AT+CIPMUX=1");
}

WiFiStatus WiFi::clearMux() {
	return execCommand("AT+CIPMUX=0");
}

String WiFi::getMux() {
	WiFiStatus resp = execCommand("AT+CIPMUX?");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("+CIPMUX:");
	}
}

WiFiStatus WiFi::execCommand(const char *cmd) {
	uport.serial()->println(cmd);
	return response();
}

WiFiStatus WiFi::execCommand(String cmd) {
	uport.serial()->println(cmd);
	return response();
}

WiFiStatus WiFi::newConnection(byte &id)
{
	for (byte i = 0; i < WIFI_MAX_CONNECTIONS; i++) {
		if (connections[i].status == CLOSED) {
			id = i;
			connections[i].status = OPENING;
			return SUCCESS;
		}
	}
	return NOT_AVAILABLE;
}

WiFiStatus WiFi::response() {
	int ret;
	int times = 0;
	output = "";
	while(true) {
		if (uport.serial()->available()) {
			String resp = uport.serial()->readStringUntil('\n');
			if ((ret=resp.indexOf("OK")) >= 0) {
				return SUCCESS;
			} else if ((ret=resp.indexOf("ERROR")) >= 0) {
				return FAILURE;
			} else if ((ret=resp.indexOf("+IPD,")) >= 0) {
				return DATA;
			} else {
				output = output + resp;
			}
		} else {
			if (times++ > 100)
				return TIMEDOUT;
			delay(10);
		}
	}
}

void WiFi::flushInput() {
	while (uport.serial()->available()) {
		volatile int ch = uport.serial()->read();
	}
}

WiFiStatus WiFi::listen(Protocol proto, int port, ListenHandlers &handlers, byte &id) {
	WiFiStatus ret = newConnection(id);
	if (ret != SUCCESS)
		return ret;
	return SUCCESS;
}

WiFiStatus WiFi::connect(Protocol proto, int port, byte &id) {
	WiFiStatus ret = newConnection(id);
	if (ret != SUCCESS)
		return ret;
	return SUCCESS;
}

WiFiStatus WiFi::send(byte connectionId, char *data, int length) {
	return SUCCESS;
}

WiFiStatus WiFi::send(byte connectionId, String str) {
	return SUCCESS;
}
