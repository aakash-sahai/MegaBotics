/*
 * WiFi.cpp
 *
 *  Created on: Mar 27, 2015
 *      Author: aakash
 */

#include <MegaBotics.h>

WiFi WiFi::_instance;

WiFi::WiFi() {
	baud = WIFI_DEFAULT_BAUD;
	uport = UPort(WIFI_DEFAULT_UPORT);
	txHead = 0;
	listenId = -1;
	listenPort = 0;
}

WiFi::WiFi(int port) {
	baud = WIFI_DEFAULT_BAUD;
	uport = UPort(port);
	txHead = 0;
	listenId = -1;
	listenPort = 0;
}

WiFi::~WiFi() {
	// Nothing to do
}

WiFiStatus WiFi::setup(void) {
	output = "";
	pinMode(uport.getDigitalPin(WIFI_RST_PIN), OUTPUT);
	pinMode(uport.getDigitalPin(WIFI_ENABLE_PIN), OUTPUT);
	pinMode(uport.getAnalogPin(), INPUT);
	pinMode(uport.getPwmInputPin(), INPUT);
	uport.serial().begin(baud);
	initConnections();
	DBG_PRINTLN("WIFI: RESET");
	hardReset();
	flushInput();
	DBG_PRINTLN("WIFI: ENABLE");
	enable();
	DBG_PRINTLN("WIFI: SET MODE AP+STA");
	setMode(MODE_BOTH);
	DBG_PRINTLN("WIFI: SET MUX");
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
	uport.serial().println(cmd);
	return response();
}

WiFiStatus WiFi::execCommand(String cmd) {
	uport.serial().println(cmd);
	return response();
}

#define STATE_IDLE			0
#define STATE_WAITOK		1
#define STATE_IPD_MSGLEN	3
#define STATE_LF			4
#define STATE_DATA			5
#define STATE_SEND			6

#define POLL_BUF_SIZE	64
static char pollbuf[POLL_BUF_SIZE+1];
static byte  pollsize = 0;
static int  msgLen = 0;
static byte msgConnId = 0;
static int  state = STATE_IDLE;

byte WiFi::replenish(void) {
	int ch;

	serialEventRun();
	while (pollsize < POLL_BUF_SIZE && (ch=uport.serial().read()) != -1) {
		pollbuf[pollsize++] = (char)ch;
	}
	pollbuf[pollsize] = 0;	// Null terminate
	return pollsize;
}

void WiFi::consume(int size) {
	pollsize -= size;
	if (pollsize) {
		memcpy(pollbuf, pollbuf+size, pollsize);
	}
}

int popState = -1;

void WiFi::poll() {
	int size;

	while (replenish()) {
		switch (state) {
		case STATE_SEND:
			if (pollsize < 9) {
				break;
			}
			if (strstr(pollbuf, "SEND OK\r\n") == pollbuf) {
				consume(9);
				checkBusy();
				state = STATE_IDLE;
			} else if (strstr(pollbuf, "+IPD,") == pollbuf) {
				msgConnId = pollbuf[5] - '0';	// Get the ID
				consume(7);						// advance by 7 ( + I P D , x , )
				popState = STATE_SEND;
				state = STATE_IPD_MSGLEN;
			} else {
				consume(1);
			}
			break;
		case STATE_DATA:		// In the middle of processing DATA (aka IPD) message
			size = min((int)pollsize, msgLen);
			msgLen -= size;

			if (listenHandlers.receive != 0)
				listenHandlers.receive(msgConnId, pollbuf, size, msgLen);

			consume(size);

			if (msgLen == 0) {
				state = STATE_WAITOK;
			}
			break;
		case STATE_WAITOK:
			if (pollsize < 6) {
				break;
			}
			if (strstr(pollbuf, "\r\nOK\r\n") == pollbuf) {
				consume(6);
				if (popState == -1)
					state = STATE_IDLE;
				else {
					state = popState;
					popState = -1;
				}
			} else {
				DBG_PRINT("O<"); DBG_PRINT(pollbuf);DBG_PRINT(">");
				consume(1);
				state = STATE_IDLE;
			}
			break;
		case STATE_IDLE:
			if (pollsize < 10) {
				break;
			}
			if (strstr(pollbuf, "+IPD,") == pollbuf) {
				msgConnId = pollbuf[5] - '0';	// Get the ID
				consume(7);						// advance by 7 ( + I P D , x , )
				state = STATE_IPD_MSGLEN;
			} else if (strstr(pollbuf, ",CONNECT\r") == pollbuf+1) {
				DBG_PRINT("CONN"); DBG_PRINTLN(pollbuf[0]);
				listenId = pollbuf[0] - '0';
				connections[listenId].id = listenId;
				connections[listenId].status = OPEN;
				connections[listenId].port = listenPort;
				strcpy(connections[listenId].host, "0.0.0.0");
				consume(10);
				if (listenHandlers.connect != 0)
					listenHandlers.connect(listenId);
				while (true) {
					if (!pollsize) {
						replenish();
					} else {
						if (pollbuf[0] == '\n') {
							consume(1);
							break;
						} else {
							DBG_PRINT("N<"); DBG_PRINT(pollbuf);DBG_PRINT(">");
							consume(1);
							break;
						}
					}
				}
			} else if (strstr(pollbuf, ",CLOSED\r\n") == pollbuf+1) {
				listenId = pollbuf[0] - '0';
				connections[listenId].status = CLOSED;
				connections[listenId].port = 0;
				connections[listenId].host[0] = 0;
				consume(10);
				if (listenHandlers.disconnect != 0)
					listenHandlers.disconnect(listenId);
				listenId = -1;
			} else if (strstr(pollbuf, "\r\n") == pollbuf) {
				consume(2);
			} else {
				consume(1);
			}
			break;
		case STATE_IPD_MSGLEN:
			msgLen = 0;
			while (true) {
				if (!pollsize) {
					replenish();
				} else {
					if (pollbuf[0] == ':') {
						consume(1);
						break;
					} else {
						msgLen = msgLen * 10 + (pollbuf[0] - '0');
						consume(1);
					}
				}
			}
			state = STATE_DATA;
			break;
		}
	}
}

WiFiStatus WiFi::response() {
	int ret;
	int times = 0;
	output = "";
	while(true) {
		if (uport.serial().available()) {
			String resp = uport.serial().readStringUntil('\n');
			if ((ret=resp.indexOf("OK")) == 0) {
				return SUCCESS;
			} else if ((ret=resp.indexOf("ERROR")) == 0) {
				return FAILURE;
			} else if ((ret=resp.indexOf("busy s...")) >= 0) {
				return BUSY;
			} else if ((ret=resp.indexOf(">")) == 0) {
				return SEND_DATA;
			} else {
				output = output + resp;
			}
		} else {
			if (times++ > 100)
				return TIMEDOUT;
			delay(1);
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
	while (uport.serial().read() != -1);
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

WiFiStatus WiFi::connect(Protocol proto, String host, int port, byte &id) {
	WiFiStatus ret = newConnection(id);
	String sproto;
	if (ret != SUCCESS)
		return ret;
	if (proto == TCP) {
		sproto = ",\"TCP\",\"";
	} else {
		sproto = ",\"UDP\",\"";
	}
	String cmd = "AT+CIPSTART" + String(id) + sproto + host + "\"," + String(port);
	String resp = String(id) + ",CONNECT";
	ret = execCommand(cmd);
	if (ret != SUCCESS) {
		connections[id].status = CLOSED;
		return ret;
	}
	String result = parseResult(cmd + "\r", "\r\r");
	if (result.equals(resp)) {
		connections[id].status = OPEN;
		connections[id].proto = proto;
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
		if (id == listenId) {
			listenId = -1;
		}
		return SUCCESS;
	} else if (result.equals("link not")) {
		return NOT_CONNECTED;
	} else {
		return FAILURE;
	}
}

WiFiStatus WiFi::checkBusy(void) {
	output="";
	uport.serial().println("AT");
	for (int attempt = 0; ; attempt++) {
		WiFiStatus ret = response();
		if (ret == BUSY) {
			if (attempt > 100) {
				return BUSY;
			} else {
				delay(10);
				uport.serial().println("AT");
			}
		} else {
			return ret;
		}
	}
}

WiFiStatus WiFi::send(byte connectionId, const char *data, int length) {
	//DBG_PRINT("SEND ID: "); DBG_PRINT((int)connectionId); DBG_PRINT(" LEN:");DBG_PRINTLN(length);
	if (connectionId < 0 || connections[connectionId].status != OPEN) {
		return NOT_CONNECTED;
	}
	while (state != STATE_IDLE) {
		poll();
	}
	String cmd = "AT+CIPSEND=" + String(connectionId) + "," + String(length);
	WiFiStatus ret = execCommand(cmd);
	if (ret != SEND_DATA) {
		return ret;
	}
	uport.serial().write(data, length);
	uport.serial().flush();
	state = STATE_SEND;
	poll();
	return SUCCESS;
}

WiFiStatus WiFi::send(byte connectionId, String str) {
	return send(connectionId, str.c_str(), str.length());
}

WiFiStatus WiFi::write(byte connectionId, char ch) {
	if (connectionId < 0 || connections[connectionId].status != OPEN) {
		return NOT_CONNECTED;
	}
	WiFiStatus ret = SUCCESS;
	txBuf[txHead++] = ch;
	if (txHead == WIFI_TXBUF_SIZE) {
		ret = flush(connectionId);
	}
	return ret;
}

WiFiStatus WiFi::flush(byte connectionId) {
	WiFiStatus ret = SUCCESS;
	if (txHead) {
		ret = send(connectionId, txBuf, txHead);
		txHead = 0;
	}
	return ret;
}
