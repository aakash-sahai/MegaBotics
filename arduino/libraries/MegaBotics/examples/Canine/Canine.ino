#define CANINE 0

#if CANINE

#include <MegaBotics.h>
#include "Canine.h"

UPort uPort(1);
HardwareSerial& serial = uPort.serial();
Logger& logger = Logger::getReference();
Canine& k9 = Canine::getReference();

void setupLogging() {
	Logger::Config config;
	config.fileName = "log.txt";
	logger.setup(config);
	logger.autoFlush(false);
	logger.enable(Logger::LOG_SERIAL);
	logger.enable(Logger::LOG_SD);
	logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);
	logger.setLevel(Logger::LOG_SD, Logger::LEVEL_DEBUG);
}

//The setup function is called once at startup of the sketch
void setup() {
	serial.begin(115200);
	setupLogging();

	Canine::Config config;
	config.enableLogger = true;
	config.displayLogger = false;

	k9.setup(config);

	k9.runSetup();

	k9.loadWaypoints();
	k9.waitForGpsFix();
	k9.waitForClick();
	k9.autoRun();
}

// The loop function is called in an endless loop
void loop() {
}

#endif
