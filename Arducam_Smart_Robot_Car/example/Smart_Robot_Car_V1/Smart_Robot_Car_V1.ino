// ArduCAM Smart_Robot_Car demo (C)2017
//Before using this demo, you shold install AFMotor library
//which are in the InstallLibrary folder.
// This demo support smart mode .
//video link: https://youtu.be/0FB7J-Qzcag

/***********************[NOTICE]*********************************
  We can't guarantee that the motor load
  is exactly the same, so it increases the compensation
  factor. You should adjust them to suit for your motor
****************************************************************/
#define leftFactor 10
#define rightFactor 5

#define speedSet  130
#define TURN_DIST 30

#include <Servo.h>
#include <AFMotor.h>

Servo neckControllerServoMotor;
AF_DCMotor leftMotor(3, MOTOR34_64KHZ);
AF_DCMotor rightMotor(4, MOTOR34_64KHZ);

int trig = A2;
int echo = A3;
unsigned int S;
unsigned int Sleft;
unsigned int Sright;

void setup() {
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  neckControllerServoMotor.attach(10);
  neckControllerServoMotor.write(90);
  delay(2000);
}
void loop() {
  neckControllerServoMotor.write(90);
  range();
  if (S <= TURN_DIST ) {
    turn();
  } else if (S > TURN_DIST) {
    moveForward();
  }
}
void turn() {
  moveStop();
  neckControllerServoMotor.write(150);
  delay(500);
  range();
  Sleft = S;
  neckControllerServoMotor.write(90);
  delay(500);
  neckControllerServoMotor.write(30);
  delay(500);
  range();
  Sright = S;
  neckControllerServoMotor.write(90);
  delay(500);
  if (Sleft <= TURN_DIST && Sright <= TURN_DIST) {
    moveBackward();
    delay(500);
    int x = random(1);
    if (x = 0) {
      turnRight();
    }
    else {
      turnLeft();
    }
  } else {
    if (Sleft >= Sright) {
      turnLeft();
    } else {
      turnRight();
    }
  }
}
void range() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(trig, LOW);
  int distance = pulseIn(echo, HIGH);
  distance = distance / 58;
  S = distance;
  if (S < TURN_DIST) {
    delay(50);
  }
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

