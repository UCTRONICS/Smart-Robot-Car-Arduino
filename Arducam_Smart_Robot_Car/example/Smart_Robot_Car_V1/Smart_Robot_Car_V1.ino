// ArduCAM Smart_Robot_Car demo (C)2017 Lee
// This demo support smart mode .


#include <AFMotor.h> 
#include <Servo.h>    
#include <NewPing.h>
#define TRIG_PIN A2 
#define ECHO_PIN A3
#define MAX_DISTANCE_POSSIBLE 1000 
#define MAX_SPEED 150 // 
#define MOTORS_CALIBRATION_OFFSET 5
#define COLL_DIST 20 
#define TURN_DIST COLL_DIST+10 
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


void setup() {
  Serial.begin(115200);
  neckControllerServoMotor.attach(10);  
  neckControllerServoMotor.write(90); 
  delay(2000);
  checkPath(); 
  motorSet = "FORWARD"; 
  neckControllerServoMotor.write(90);
  moveForward();
}



void loop() {
  checkForward();  
  checkPath();
}

void checkPath() {
  int curLeft = 0;
  int curFront = 0;
  int curRight = 0;
  int curDist = 0;
  uint16_t temp = 0;
  neckControllerServoMotor.write(150);
  delay(120); 
  for(pos = 160; pos >= 30; pos-=20)
  {
    neckControllerServoMotor.write(pos);
    delay(90);
   checkForward(); 
    temp = readPing();
    if(temp < 3)
      curDist = 50;
      else
      curDist= temp;  
    if (curDist < TURN_DIST) {
      maxAngle = pos;
      changePath();
    }
    if (curDist > curDist) {maxAngle = pos;}
    if (pos > 90 && curDist > curLeft) { curLeft = curDist;}
    if (pos == 90 && curDist > curFront) {curFront = curDist;}
    if (pos < 90 && curDist > curRight) {curRight = curDist;}
  }
  maxLeft = curLeft;
  maxRight = curRight;
  maxFront = curFront;
}

void setCourse() {
    if (maxAngle < 90) {turnRight();}
    if (maxAngle > 90) {turnLeft();}
    maxLeft = 0;
    maxRight = 0;
    maxFront = 0;
}

void checkCourse() {
  moveBackward();
  delay(300);
  moveStop();
  setCourse();
}

void changePath() {
  if (pos < 90) {lookLeft();} 
  if (pos > 90) {lookRight();}
}


int readPing() {
  delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS/US_ROUNDTRIP_CM;
  return cm;
}

void checkForward() 
{ 
  if (motorSet=="FORWARD") 
  {
    leftMotor.run(FORWARD); 
    rightMotor.run(FORWARD); 
   }
 }

void checkBackward() { if (motorSet=="BACKWARD") {leftMotor.run(BACKWARD); rightMotor.run(BACKWARD); } }

void moveStop() {leftMotor.run(RELEASE); rightMotor.run(RELEASE);}

void moveForward() {
    motorSet = "FORWARD";
    leftMotor.run(FORWARD);
    rightMotor.run(FORWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2)
  {
    leftMotor.setSpeed(speedSet);
    rightMotor.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    delay(5);
  }
}

void moveBackward() {
    motorSet = "BACKWARD";
    leftMotor.run(BACKWARD);
    rightMotor.run(BACKWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2)
  {
    leftMotor.setSpeed(speedSet);
    rightMotor.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    delay(5);
  }
}  

void turnRight() {
  motorSet = "RIGHT";
  leftMotor.run(FORWARD);
  rightMotor.run(BACKWARD);
  delay(400);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);      
}  

void turnLeft() {
  motorSet = "LEFT";
  leftMotor.run(BACKWARD);
  rightMotor.run(FORWARD);
  delay(400);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
}  

void lookRight() {rightMotor.run(BACKWARD); delay(400); rightMotor.run(FORWARD);}
void lookLeft() {leftMotor.run(BACKWARD); delay(400); leftMotor.run(FORWARD);}
