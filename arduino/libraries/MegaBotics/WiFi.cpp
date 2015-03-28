/*
 * WiFi.cpp
 *
 *  Created on: Mar 27, 2015
 *      Author: aakash
 */

#include <MegaBotics.h>

WiFi::WiFi() {
	baud = WIFI_DEFAULT_BAUD;
	uport = UPort(2);
}

WiFi::WiFi(int port) {
	baud = WIFI_DEFAULT_BAUD;
	uport = UPort(port);
}

WiFi::~WiFi() {
	// Nothing to do
}

WiFiStatus WiFi::setup(void) {
	currentMode = MODE_BOTH;
	DBG_PRINT("PORT/GPIO1/GPIO2/ANALOG: ");
	DBG_PRINTLN(uport.getPortNumber());
	DBG_PRINTLN(uport.getDigitalPin(1));
	DBG_PRINTLN(uport.getDigitalPin(2));
	DBG_PRINTLN(uport.getAnalogPin());
	pinMode(uport.getDigitalPin(1), OUTPUT);
	pinMode(uport.getDigitalPin(2), OUTPUT);
	pinMode(uport.getAnalogPin(), OUTPUT);
	enable();
	hardReset();
	return SUCCESS;
}

WiFiStatus WiFi::setupAP(ApConfig config) {
	return SUCCESS;
}

WiFiStatus WiFi::setupSTA(StaConfig config) {
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
	uport.serial()->println("AT+RST");
	//flushTill("OK");
	return SUCCESS;
}

WiFiStatus WiFi::hardReset(void) {
	byte rstPin = uport.getDigitalPin(WIFI_RST_PIN);
	digitalWrite(rstPin, LOW);
	delay(100);
	digitalWrite(rstPin, HIGH);
	return SUCCESS;
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

WiFiStatus WiFi::setMode(WiFiMode mode) {
	execCommand("AT+CWMODE=" + mode);
	return SUCCESS;
}

const char *WiFi::getStaIp(void) {
	execCommand("AT+CIPSTA?");
	return NULL;
}

const char *WiFi::getApIp(void) {
	execCommand("AT+CIPAP?");
	return NULL;
}

WiFiStatus WiFi::setStaIp(char *ip) {
	execCommand(String("AT+CIPSTA=")+ ip);
	return SUCCESS;
}

WiFiStatus WiFi::setApIp(char *ip) {
	String cmd = "AT+CIPAP=" + String(ip);
	execCommand(cmd);
	return SUCCESS;
}

const char *WiFi::getStaMac(void) {
	execCommand("AT+CIPSTAMAC?");
	return NULL;
}

const char *WiFi::getApMac(void) {
	execCommand("AT+CIPAPMAC?");
	return NULL;
}

WiFiStatus WiFi::setBaud(long baud) {
	execCommand("AT+CIOBAUD=" + baud);
	uport.serial()->println(baud);
	return SUCCESS;
}

long WiFi::getBaud(void) {
	execCommand("AT+CIOBAUD?");
	return 0L;
}

WiFiStatus WiFi::execCommand(const char *cmd) {
	uport.serial()->println(cmd);
	return SUCCESS;
}


WiFiStatus WiFi::execCommand(String cmd) {
	uport.serial()->println(cmd);
	return SUCCESS;
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
