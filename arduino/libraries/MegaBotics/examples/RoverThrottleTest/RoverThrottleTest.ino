// Do not remove the include below
#define ROVER_THROTTLE_TEST 1

#if ROVER_THROTTLE_TEST

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <MegaBotics.h>

Rover& rover = Rover::getReference();
EepromStore& estore = EepromStore::getReference();
InputPanel panel;

//The setup function is called once at startup of the sketch
void setup() {
	Serial.begin(115200);

	// estore.setup() should be called before rover.setup()
	estore.setup();
	panel.setup();

	rover.setup();
	rover.setControlMode(Rover::MANUAL);

	Serial.println("waiting for button press");
	panel.waitForClick();
	rover.setControlMode(Rover::AUTO);

	Serial.println("=============== forward and stop =============== ");
	Serial.println("go forward at 10");
	rover.forward(10);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== reverse and stop =============== ");
	Serial.println("go reverse at 10");
	rover.reverse(10);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== forward, forward and stop =============== ");
	Serial.println("go forward at 50");
	rover.forward(50);
	delay(2000);

	Serial.println("go forward at 20");
	rover.forward(20);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== reverse, reverse and stop =============== ");
	Serial.println("go reverse at 50");
	rover.reverse(50);
	delay(2000);

	Serial.println("go reverse at 20");
	rover.reverse(20);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== forward, idle and stop =============== ");
	Serial.println("go forward at 10");
	rover.forward(10);
	delay(2000);

	Serial.println("idle");
	rover.idle();
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== forward, stop and reverse, stop =============== ");
	Serial.println("go forward at 10");
	rover.forward(10);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("go reverse at 10");
	rover.reverse(10);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== forward, idle and idle =============== ");
	Serial.println("go forward at 10");
	rover.forward(10);
	delay(2000);

	Serial.println("idle");
	rover.idle();
	delay(2000);

	Serial.println("idle");
	rover.stop();
	delay(2000);

	Serial.println("=============== reverse, idle and idle =============== ");
	Serial.println("go reverse at 10");
	rover.reverse(10);
	delay(2000);

	Serial.println("idle");
	rover.idle();
	delay(2000);

	Serial.println("idle");
	rover.idle();
	delay(2000);

	Serial.println("=============== forward, stop and stop =============== ");
	Serial.println("go forward at 10");
	rover.forward(10);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== reverse, stop and stop =============== ");
	Serial.println("go reverse at 10");
	rover.reverse(10);
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("stop");
	rover.stop();
	delay(2000);

	Serial.println("=============== testing done =============== ");
}

// The loop function is called in an endless loop
void loop() {
}

#endif
