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

void setup() {
	HardwareSerial & serial = terminalPort.serial();
	GPSRover& gpsRover = GPSRover::getReference();

	serial.begin(115200);

	gpsRover.setup();
	gpsRover.waitToStart();
	gpsRover.autoRun();
}

void loop() {
}

void cross(void) {
	GPS & gps = GPS::getReference();
	UPort & gpsUport = gps.getUPort();

	gpsUport.cross(terminalPort);
	terminalPort.interact();
	gpsUport.uncross();
}

#endif
