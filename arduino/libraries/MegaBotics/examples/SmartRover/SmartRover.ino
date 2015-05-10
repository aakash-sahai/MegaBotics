#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <MegaBotics.h>

UPort uPort(1);
HardwareSerial & serial = uPort.serial();
Logger & logger = Logger::getReference();
InputPanel panel;
SmartRover smartRover = SmartRover::getReference();

#define STRAIGHT_LENGTH 16.0
#define TO_PIN_LENGTH 80.0
#define LITTLE_TURN_LENGTH 5.5
#define BIG_TURN_LENGTH 17.0

#define TURN_THROTTLE 5
#define STRAIGHT_THROTTLE 30
#define TO_PIN_THROTTLE 10

void setup() {
        serial.begin(115200);
        SmartRover::Config config;
        config.wpProximRadius = 2.0;
        smartRover.setup(config);
        logger.setup();
        logger.autoFlush(true);
        logger.enable(Logger::LOG_SERIAL);
        logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);

        logger.start("Starting Rover log");
        runRoutine();
}

void runRoutine() {
  while(true) {
    panel.waitForClick();
    switch (panel.getPosition()) {
    case SpdtSwitch::DOWN:
      figure8();
      break;
    case SpdtSwitch::MID:
      goStraight();
      break;
    case SpdtSwitch::UP:
      break;
    }
  }
}

void loop() {
}

void figure8() {
  smartRover.addWaypoint(3, 0, TURN_THROTTLE, 2.0);
  smartRover.addWaypoint(15,-100, TURN_THROTTLE);
  smartRover.addWaypoint(10, -100, STRAIGHT_THROTTLE, 3.0);
  smartRover.addWaypoint(10, 0, TURN_THROTTLE, 3.0);
  smartRover.addWaypoint(15, 120, TURN_THROTTLE, 3.0);
  smartRover.addWaypoint(10, 110, STRAIGHT_THROTTLE, 3.0);
  smartRover.addWaypoint(10, 0, TURN_THROTTLE, 3.0);

//  smartRover.addWaypoint(3, 0, TURN_THROTTLE, 2.0);
//  smartRover.addWaypoint(15,-100, TURN_THROTTLE);
//  smartRover.addWaypoint(10, -100, STRAIGHT_THROTTLE, 2.0);
//  smartRover.addWaypoint(10, -10, TURN_THROTTLE, 3.0);
//  smartRover.addWaypoint(5, 90, TURN_THROTTLE, 2.0);
//  smartRover.addWaypoint(15, 110, STRAIGHT_THROTTLE);
//  smartRover.addWaypoint(5, 90, STRAIGHT_THROTTLE, 3.0);
//  smartRover.addWaypoint(5, 0, STRAIGHT_THROTTLE, 3.0);
  smartRover.autoRun();
  smartRover.clearWaypoints();
}

void goStraight() {
  smartRover.addWaypoint(80, 0, TO_PIN_THROTTLE);
  smartRover.addWaypoint(4, 0, -10);
  smartRover.autoRun();
  smartRover.clearWaypoints();
}
