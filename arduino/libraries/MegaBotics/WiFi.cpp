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
	listenPort = 0;
}

WiFi::WiFi(int port) {
	baud = WIFI_DEFAULT_BAUD;
	uport = UPort(port);
	output = "";
	listenPort = 0;
}

WiFi::~WiFi() {
	// Nothing to do
}

WiFiStatus WiFi::setup(void) {
	pinMode(uport.getDigitalPin(WIFI_RST_PIN), OUTPUT);
	pinMode(uport.getDigitalPin(WIFI_ENABLE_PIN), OUTPUT);
	pinMode(uport.getAnalogPin(), INPUT);
	uport.serial()->begin(baud);
	initConnections();
	hardReset();
	flushInput();
	enable();
	setMode(MODE_BOTH);
	setMux();
	return SUCCESS;
}

void WiFi::initConnections(void) {
	for (byte i = 0; i < WIFI_MAX_CONNECTIONS; i++) {
			connections[i].id = i;
			connections[i].status = CLOSED;
			connections[i].port = 0;
			connections[i].host[0] = 0;					// To ensure that host string is empty
			connections[i].host[WIFI_MAX_HOSTLEN] = 0;	// To ensure that all host names remain null terminated
	}
	listenHandlers.connect = 0;
	listenHandlers.disconnect = 0;
	listenHandlers.receive = 0;
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

WiFiStatus WiFi::execCommand(const char *cmd) {
	uport.serial()->println(cmd);
	return response();
}

WiFiStatus WiFi::execCommand(String cmd) {
	uport.serial()->println(cmd);
	return response();
}

void WiFi::process_data(String resp) {
	const char *ptr;
	int len;
	byte id;
	int msgLen = 0;

	ptr = resp.c_str();
	len = resp.length();

	ptr += 5; len -= 5;		// Point past "+IPD,"
	id = *ptr - '0';		// Decode ID

	ptr += 2; len -= 2;  	// Point past "+IPD,x"

	for (;*ptr != ':'; ptr++, len--) {		// Decode message length
		msgLen = msgLen * 10 + (*ptr - '0');
	}
	ptr++; len--;

	if (listenHandlers.receive != 0)
		listenHandlers.receive(id, ptr, len);

	msgLen -= len;			// msgLen is now the number of unread bytes

	if (msgLen > 0) {		// Read the rest of the data
		char *ptr = (char *)malloc(msgLen);
		while (msgLen > 0) {
			len = uport.serial()->readBytes(ptr, msgLen);
			msgLen -= len;
			if (listenHandlers.receive != 0)
				listenHandlers.receive(id, ptr, len);
		}
		free((void *)ptr);
	}
}

void WiFi::poll() {
	int ret;

	while (uport.serial()->available()) {
		String resp = uport.serial()->readStringUntil('\n');
		if ((ret=resp.indexOf("+IPD,")) == 0) {
			process_data(resp);
		} else {
			output = output + resp;
		}
	}
}

WiFiStatus WiFi::response() {
	int ret;
	int times = 0;
	output = "";
	while(true) {
		if (uport.serial()->available()) {
			String resp = uport.serial()->readStringUntil('\n');
			if ((ret=resp.indexOf("OK")) == 0) {
				return SUCCESS;
			} else if ((ret=resp.indexOf("ERROR")) == 0) {
				return FAILURE;
			} else if ((ret=resp.indexOf(">")) == 0) {
				return SEND_DATA;
			} else if ((ret=resp.indexOf("+IPD,")) == 0) {
				process_data(resp);
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

String WiFi::parseResult(String pattern) {
	return parseResult(pattern, "\r");
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

String WiFi::parseResultDbg(String pattern, String terminator) {
	int begin;
	Serial.println((String)("1> " + output + "<1"));
	if ((begin = output.indexOf(pattern)) >= 0) {
		output.remove(0, begin + pattern.length());
	}
	Serial.println((String)("2> " + output + "<2"));
	begin = output.indexOf(terminator);
	output.remove(begin);
	Serial.println((String)("3> " + output + "<3"));
	return output;
}

String WiFi::readGpio(byte pin) {
	String cmd = "AT+CIOREAD=" + String(pin);
	WiFiStatus resp = execCommand(cmd);
	if (resp != SUCCESS) {
		return "";
	} else {
		cmd = cmd + "\r";
		return parseResult(cmd, "\r\r");
	}
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

WiFiStatus WiFi::setApConfig(ApConfig &config) {
	char cmd[100];
	sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",%d,%d",
			config.ssid.c_str(), config.password.c_str(),
			(int)config.channel, (int)config.encryption);
	return execCommand(cmd);
}

WiFiStatus WiFi::setStaConfig(StaConfig &config) {
	char cmd[100];
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"",
			config.ssid.c_str(), config.password.c_str());
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

String WiFi::getFirmwareVersion(void) {
	WiFiStatus resp = execCommand("AT+GMR");
	if (resp != SUCCESS) {
		return "";
	} else {
		return parseResult("AT+GMR\r", "AI-THINKER");
	}
}

byte WiFi::readAnalog(void) {
	WiFiStatus resp = execCommand("AT+CIOADC");
	if (resp != SUCCESS) {
		return 255;
	} else {
		return (byte)(parseResult("AT+CIOADC\r").toInt());
	}
}

String WiFi::readGpio0() {
	return readGpio(0);
}

String WiFi::readGpio2() {
	return readGpio(2);
}

WiFiStatus	WiFi::writeGpio(byte pin, bool value) {
	String cmd = "AT+CIOWRITE=" + String(pin) + "," + String(value);
	return execCommand(cmd);
}

void WiFi::flushInput() {
	while (uport.serial()->available()) {
		volatile int ch = uport.serial()->read();
	}
}

WiFiStatus WiFi::listen(int port, ListenHandlers *handlers) {
	if (listenPort != 0)
		return NOT_AVAILABLE;
	String cmd = "AT+CIPSERVER=1," + String(port);
	WiFiStatus ret = execCommand(cmd);
	if (ret != SUCCESS) {
		return ret;
	}
	listenPort = port;
	memcpy((void *)&listenHandlers, (void *)handlers, sizeof(ListenHandlers));
	return SUCCESS;
}

WiFiStatus WiFi::connect(int port, String host, byte &id) {
	WiFiStatus ret = newConnection(id);
	if (ret != SUCCESS)
		return ret;
	String cmd = "AT+CIPSTART" + String(id) + ",\"TCP\",\"" + host + "\"," + String(port);
	String resp = String(id) + ",CONNECT";
	ret = execCommand(cmd);
	if (ret != SUCCESS) {
		connections[id].status = CLOSED;
		return ret;
	}
	String result = parseResult(cmd + "\r", "\r\r");
	if (result.equals(resp)) {
		connections[id].status = OPEN;
		connections[id].port = port;
		strncpy(connections[id].host, host.c_str(), WIFI_MAX_HOSTLEN);
		return SUCCESS;
	} else {
		connections[id].status = CLOSED;
		return FAILURE;
	}
}

WiFiStatus WiFi::disconnect(byte id) {
	String cmd = "AT+CIPCLOSE=" + String(id);
	String resp = String(id) + ",CLOSED";
	WiFiStatus ret = execCommand(cmd);
	if (ret != SUCCESS) {
		return ret;
	}
	String result = parseResult(cmd + "\r", "\r\r");
	if (result.equals(resp)) {
		connections[id].status = CLOSED;
		connections[id].port = 0;
		connections[id].host[0] = 0;
		return SUCCESS;
	} else if (result.equals("link not")) {
		return NOT_CONNECTED;
	} else {
		return FAILURE;
	}
}

WiFiStatus WiFi::send(byte connectionId, const char *data, int length) {
	String cmd = "AT+CIPSEND=" + String(connectionId) + "," + String(length);
	WiFiStatus ret = execCommand(cmd);
	if (ret != SEND_DATA) {
		return ret;
	}
	uport.serial()->write(data, length);
	WiFiStatus status = response();
	return status;
}

WiFiStatus WiFi::send(byte connectionId, String str) {
	return send(connectionId, str.c_str(), str.length());
}
