
// ArduCAM Smart_Robot_Car demo (C)2017 Lee
// This demo support smart mode .
// This demo support bluetooth control and IR remote control.
// The default mode is bluetooth control you can change it to
// the IR control mode by touching the IRENABLE key.
// We optimize the IR control code for more sensitive control.
// When there are some obstacles ahead, the car will stop.
//video link: https://youtu.be/0FB7J-Qzcag


#include <AFMotor.h>
#include <Servo.h>
#include <NewPing.h>
#include "ArducamNEC.h"
#define TRIG_PIN A2
#define ECHO_PIN A3
#define MAX_DISTANCE_POSSIBLE 1000
#define MAX_SPEED 150
#define TURN_MAX_SPEED 150
#define MOTORS_CALIBRATION_OFFSET 0
#define COLL_DIST 30
#define TURN_DIST COLL_DIST+10

int RECV_PIN = 2;
ArducamNEC myIR(2);
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE_POSSIBLE);

AF_DCMotor leftMotor(3, MOTOR34_64KHZ);
AF_DCMotor rightMotor(4, MOTOR34_64KHZ);
Servo neckControllerServoMotor;
int pos = 0;
int maxDist = 0;
int maxAngle = 0;
int maxRight = 0;
int maxLeft = 0;
int maxFront = 0;
int course = 0;
int curDist = 0;
String motorSet = "";
int speedSet = 0;
bool smartEnable = false;  //This flag is used for enable smart mode
bool IR_Enable = false;    //This flag is used for enable ir remote control mode
bool detected_flag = false; //This flag is used for enable detected obstacles ahead
void setup() {
  // put your setup code here, to run once:
  uint8_t temp;
  Serial.begin(9600);
  neckControllerServoMotor.attach(10);
  neckControllerServoMotor.write(90);
  moveStop();
}
void loop() {
  // put your main code here, to run repeatedly:
  uint32_t temp = 0xff;
  int detecteVal = 0;
  if (!IR_Enable)
  {
    if (Serial.available())
    {
      temp = Serial.read();
      switch (temp)
      {
        case 0x01:
          Serial.println(F( "Move forward"));
          temp = 0;
          smartEnable = false;
          detected_flag = true;
          for (uint8_t i = 0; i < 3; i++)
            detecteVal = detecteVal + readPing();
          detecteVal = detecteVal / 3;
          if ((detecteVal > 0) && (detecteVal < 30) ) {
            moveStop();
          }
          else
            moveForward();
          break;
        case 0x02:
          temp = 0;
          Serial.println(F(" Move backward"));
          smartEnable = false;
          detected_flag = false;
          moveBackward();
          break;
        case 0x03:
          temp = 0;
          Serial.println(F(" Turn left"));
          smartEnable = false;
          detected_flag = false;
          turnLeft();
          break;
        case 0x04:
          temp = 0;
          Serial.println(F(" Turn right"));
          smartEnable = false;
          detected_flag = false;
          turnRight();
          break;
        case 0x05:
          temp = 0;
          Serial.println(F(" Stop"));
          smartEnable = false;
          detected_flag = false;
          neckControllerServoMotor.write(90);
          delay(100);
          neckControllerServoMotor.detach();
          moveStop();
          break;
        case 0x06:
          temp = 0;
          neckControllerServoMotor.attach(10);
          neckControllerServoMotor.write(90);
          smartEnable = true;
          IR_Enable = false;
          detected_flag = false;
          Serial.println(F("this is smart mode and you don't do anything"));
          moveStop();
          delay(1000);
          detected_flag = false;
          moveForward();
          break;
        case 0x07:
          temp = 0;
          delay(100);
          neckControllerServoMotor.detach();
          delay(100);
          myIR.begin();
          IR_Enable = true;
          detected_flag = false;
          smartEnable = false;
          Serial.println(F("this is IR control mode, Please use your IR controller"));
          moveStop();
          break;
        default:
          break;
      }
    }
  }
  if (IR_Enable) {
    while (myIR.available())
    {
      temp =  myIR.read();
    }
    if (temp == 0xFF46B9)  //up
    {
      temp = 0;
      Serial.println(F(" Move forward"));
      detected_flag = true;
      for (uint8_t i = 0; i < 3; i++)
        detecteVal = detecteVal + readPing();
      detecteVal = detecteVal / 3;
      if ((detecteVal > 0) && (detecteVal < 30) ) {
        moveStop();
      }
      else
        moveForward();
      moveForward();
    } else if (temp == 0xFF15EA) { //down
      detected_flag = false;
      Serial.println(F(" Move backward"));
      temp = 0;
      moveBackward();
    } else if (temp == 0xFF44BB) { // left
      Serial.println(F(" Turn left"));
      temp = 0;
      detected_flag = true;
      turnLeft();
    } else if (temp == 0xFF43BC) { //right
      temp = 0;
      detected_flag = false;
      Serial.println(F(" Turn right"));
      turnRight();
    } else if (temp == 0xFF40BF) {
      Serial.println(F(" Stop"));
      temp = 0;
      smartEnable = false;
      detected_flag = false;
      neckControllerServoMotor.write(90);
      moveStop();
    } else if (temp == 0xFF42BD) { //change to bluetooth mode
      IR_Enable = false;
      smartEnable = false;
      detected_flag = false;
      neckControllerServoMotor.write(90);
      Serial.println(F(" This is bluetooth control mode, Please use your bluetooth controller"));
      moveStop();
    }
  }
  if (detected_flag) {
    for (uint8_t i = 0; i < 3; i++)
     detecteVal = detecteVal + readPing();
     detecteVal = detecteVal / 3;
    if ((detecteVal > 0) && (detecteVal < 30) )
      moveStop();
  }
  if (smartEnable) {
    checkForward();
    checkPath();
  }


}


int readPing() {
  delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS / US_ROUNDTRIP_CM;
  return cm;
}
void checkPath() {
  int i = 0;
  int curLeft = 0;
  int curFront = 0;
  int curRight = 0;
  int curDist = 0;
  uint16_t temp = 0;
  if (Serial.available())
    i = Serial.read();
  if (i == 0x05) {
    neckControllerServoMotor.write(90);
    moveStop();
    delay(100);
    neckControllerServoMotor.detach();
  } else
  { neckControllerServoMotor.write(150);
    delay(90);
    for (pos = 160; pos >= 30; pos -= 20)
    {
      neckControllerServoMotor.write(pos);
      delay(30);
      checkForward();
      temp = readPing();
      if (temp < 3) {
        moveStop(); continue;
      }
      else
        curDist = temp;
      if (curDist < TURN_DIST) {
        maxAngle = pos;
        changePath();
      }
      if (curDist > curDist) {
        maxAngle = pos;
      }
      if (pos > 90 && curDist > curLeft) {
        curLeft = curDist;
      }
      if (pos == 90 && curDist > curFront) {
        curFront = curDist;
      }
      if (pos < 90 && curDist > curRight) {
        curRight = curDist;
      }
    }
    maxLeft = curLeft;
    maxRight = curRight;
    maxFront = curFront;
  }
}
void setCourse() {
  if (maxAngle < 90) {
    turnRight();
  }
  if (maxAngle > 90) {
    turnLeft();
  }
  maxLeft = 0;
  maxRight = 0;
  maxFront = 0;
}
void checkCourse() {
  moveBackward();
  delay(200);
  moveStop();
  setCourse();
}
void changePath() {
  checkCourse();
  if (pos < 90) {
    lookLeft();
  }
  if (pos > 90) {
    lookRight();
  }
}
void checkForward()
{
  if (motorSet == "FORWARD")
  {
    leftMotor.run(FORWARD);
    rightMotor.run(FORWARD);
  }
}
void checkBackward() {
  if (motorSet == "BACKWARD") {
    leftMotor.run(BACKWARD);
    rightMotor.run(BACKWARD);
  }
}

void moveStop() {
  leftMotor.run(RELEASE);
  rightMotor.run(RELEASE);
}

void moveForward() {
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2)
  {
    leftMotor.setSpeed(speedSet + MOTORS_CALIBRATION_OFFSET);
    rightMotor.setSpeed(speedSet);
    delay(5);
  }
}

void moveBackward() {
  motorSet = "BACKWARD";
  leftMotor.run(BACKWARD);
  rightMotor.run(BACKWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2)
  {
    leftMotor.setSpeed(speedSet);
    rightMotor.setSpeed(speedSet + MOTORS_CALIBRATION_OFFSET);
    delay(5);
  }
}
void turnRight() {
  motorSet = "RIGHT";
  leftMotor.run(FORWARD);
  rightMotor.run(BACKWARD);
  if (smartEnable) {
    delay(80);
    motorSet = "FORWARD";
    leftMotor.run(FORWARD);
    rightMotor.run(FORWARD);
  }
  else {
    for (speedSet = 0; speedSet < TURN_MAX_SPEED; speedSet += 2)
    {
      leftMotor.setSpeed(speedSet);
      rightMotor.setSpeed(speedSet + MOTORS_CALIBRATION_OFFSET);
      delay(5);
    }
  }
}
void turnLeft() {
  motorSet = "LEFT";
  leftMotor.run(BACKWARD);
  rightMotor.run(FORWARD);
  if (smartEnable) {
    delay(80);
    motorSet = "FORWARD";
    leftMotor.run(FORWARD);
    rightMotor.run(FORWARD);
  }
  else {
    for (speedSet = 0; speedSet < TURN_MAX_SPEED; speedSet += 2)
    {
      leftMotor.setSpeed(speedSet);
      rightMotor.setSpeed(speedSet + MOTORS_CALIBRATION_OFFSET);
      delay(5);
    }
  }
}

void lookRight() {
  rightMotor.run(BACKWARD);
  delay(400);
  rightMotor.run(FORWARD);
}
void lookLeft() {
  leftMotor.run(BACKWARD);
  delay(400);
  leftMotor.run(FORWARD);
}
