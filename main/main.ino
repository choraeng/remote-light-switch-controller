#include <Servo.h>
// https://github.com/RoboticsBrno/ServoESP32

Servo servo_top, servo_bot;

static const int servopin_top = 13;
static const int servopin_bot = 13;

static const int rotate_idle = 00;
static const int rotate_push = 00;
static const int rotate_pop  = 00;

void setup() {
  // put your setup code here, to run once:
  servo_top.attach(servopin_top);
  servo_bot.attach(servopin_bot);
}

void loop() {
  // put your main code here, to run repeatedly:

  servo_top.write(rotate_idle)
}
