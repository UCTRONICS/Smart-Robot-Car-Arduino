//This demo is used for testing the Motor module.
#define leftFactor 10
#define rightFactor 5
#define speedSet  150

#include <UCMotor.h>
UC_DCMotor leftMotor(3, MOTOR34_64KHZ);
UC_DCMotor rightMotor(4, MOTOR34_64KHZ);

void setup() {
  Serial.begin(9600);
}
 void loop() {
 moveForward();
 delay(2000);
 moveBackward();
 delay(2000);
 turnLeft();
 delay(2000);
 turnRight();
 delay(2000);
 moveStop();
 delay(2000);
}
void moveForward() {
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
  leftMotor.setSpeed(speedSet + leftFactor);
  rightMotor.setSpeed(speedSet + rightFactor);
}
void turnLeft() {
  leftMotor.run(BACKWARD);
  rightMotor.run(FORWARD);
  leftMotor.setSpeed(speedSet + leftFactor);
  rightMotor.setSpeed(speedSet + rightFactor);
  delay(400);
  moveStop();
}
void turnRight() {
  leftMotor.run(FORWARD);
  rightMotor.run(BACKWARD);
  leftMotor.setSpeed(speedSet + leftFactor);
  rightMotor.setSpeed(speedSet + rightFactor);
  delay(400);
  moveStop();
}
void moveBackward() {
  leftMotor.run(BACKWARD);
  rightMotor.run(BACKWARD);
  leftMotor.setSpeed(speedSet + leftFactor);
  rightMotor.setSpeed(speedSet + rightFactor);
}
void moveStop() {
  leftMotor.run(RELEASE); rightMotor.run(RELEASE);
}

