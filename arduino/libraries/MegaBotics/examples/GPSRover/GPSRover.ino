#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <examples/GPSRover/GPSRover.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <GPS.h>
#include <MegaBotics.h>


#define GPS_ROVER	1

#if GPS_ROVER

UPort terminalPort(1);
GPS& gps = GPS::getReference();
UPort& gpsUport = gps.getUPort();
HardwareSerial& serial = terminalPort.serial();
Logger& logger = Logger::getReference();
GPSRover& gpsRover = GPSRover::getReference();

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

void setup() {
	serial.begin(115200);
	setupLogging();

	GPSRover::Config config;
	config.enableLogger = true;

	gpsRover.setup();
	// cross();	- Uncomment to loop GPS to serial terminal for troubleshoting purpose
	gpsRover.waitForClick();
	gpsRover.autoRun();
}

void loop() {
}

void cross(void) {
	gpsUport.cross(terminalPort);
	terminalPort.interact();
	gpsUport.uncross();
}

#endif
