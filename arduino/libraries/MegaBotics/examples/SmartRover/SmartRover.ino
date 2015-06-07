#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <MegaBotics.h>

#define SMART_ROVER 0

#if SMART_ROVER

UPort uPort(1);
HardwareSerial & serial = uPort.serial();
Logger & logger = Logger::getReference();
InputPanel panel;
SmartRover smartRover = SmartRover::getReference();

#define STRAIGHT_LENGTH 16.0
#define TO_PIN_LENGTH 80.0
#define LITTLE_TURN_LENGTH 5.5
#define BIG_TURN_LENGTH 17.0


#define TO_PIN_THROTTLE 10
#define FAST_THROTTLE 20

void setup() {
        serial.begin(115200);
        SmartRover::Config config;
        config.wpProximRadius = 3.0;
        smartRover.setup(config);
        logger.setup();
        logger.autoFlush(true);
        //logger.enable(Logger::LOG_SERIAL);
        //logger.setLevel(Logger::LOG_SERIAL, Logger::LEVEL_DEBUG);

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
    	figure8();
      break;
    case SpdtSwitch::UP:
    	figure8();
      break;
    }
  }
}

void loop() {
}

#define FUI 1

#define MID_THROTTLE 		5
#define JUMP_THROTTLE 		10
#define STRAIGHT_THROTTLE	10

void figure8() {
#if FUI
  smartRover.addWaypoint(63, 0, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(130, 90, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(50, 100, MID_THROTTLE);
  smartRover.addWaypoint(35, 75, MID_THROTTLE);
  smartRover.addWaypoint(34, 100, MID_THROTTLE);
  smartRover.addWaypoint(35, 75, MID_THROTTLE);
  smartRover.addWaypoint(17, 90, MID_THROTTLE);
  smartRover.addWaypoint(75, 180, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(45, 180, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(75, -90, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(80, -90, JUMP_THROTTLE);
  smartRover.addWaypoint(130, -90, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(85,0, STRAIGHT_THROTTLE);

#else
  smartRover.addWaypoint(63, 0, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(130, 90, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(154, 90, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(75, 180, MID_THROTTLE);
  smartRover.addWaypoint(45, 180, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(74, -90, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(80, -90, JUMP_THROTTLE);
  smartRover.addWaypoint(130, -90, STRAIGHT_THROTTLE);
  smartRover.addWaypoint(85,0, STRAIGHT_THROTTLE);
#endif
  smartRover.autoRun();
  smartRover.clearWaypoints();
}

void goStraight() {
  smartRover.addWaypoint(71, 0, TO_PIN_THROTTLE);
  smartRover.addWaypoint(4, 0, -10);
  smartRover.autoRun();
  smartRover.clearWaypoints();
}

#endif
