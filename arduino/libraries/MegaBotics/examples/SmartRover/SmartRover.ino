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

#define TURN_THROTTLE 10
#define STRAIGHT_THROTTLE 30
#define TO_PIN_THROTTLE 30

void setup() {
        serial.begin(115200);
        smartRover.setup();
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
    case SpdtSwitch::UP:
      figure8();
      break;
    case SpdtSwitch::DOWN:
      goStraight();
      break;
    case SpdtSwitch::MID:
      break;
    }
  }
}

void loop() {
}

void figure8() {
  smartRover.addWaypoint(3, 0, TURN_THROTTLE);
  smartRover.addWaypoint(21 ,-120, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(6, 0, TURN_THROTTLE);
  smartRover.addWaypoint(21, 120, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(6, 0, TURN_THROTTLE);
  smartRover.autoRun();
  smartRover.clearWaypoints();
}

void goStraight() {
  smartRover.addWaypoint(40, 0, TO_PIN_THROTTLE);
  smartRover.autoRun();
  smartRover.clearWaypoints();
}
