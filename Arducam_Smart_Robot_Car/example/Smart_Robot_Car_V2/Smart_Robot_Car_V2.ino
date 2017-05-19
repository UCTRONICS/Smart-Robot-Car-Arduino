// ArduCAM Smart_Robot_Car demo (C)2017 Lee
// This demo support bluetooth control、IR control and default is  bluetooth control mode
// It needs to be used in combination with bluetooth software.
// Before running this demo, you should enable the IR_RECV_TIMER2 
// in the ../../libraries/ArducamIR/IRLibTimer.h !!

#include <AFMotor.h> 
#include <Servo.h>    
#include <NewPing.h>
#include <IRLib.h>
#include <IRLibTimer.h>
#define TRIG_PIN A2 
#define ECHO_PIN A3
#define MAX_DISTANCE_POSSIBLE 1000 
#define MAX_SPEED 150 
#define TURN_MAX_SPEED 150
#define MOTORS_CALIBRATION_OFFSET 0
#define COLL_DIST 30 
#define TURN_DIST COLL_DIST+10 

int RECV_PIN = 2;

#if !(defined IR_RECV_TIMER2)
  #error Please enable the IR_RECV_TIMER2 ans disable the IR_RECV_TIMER1 in the ../../libraries/ArducamIR/IRLibTimer.h
#endif

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
bool smartEnable=false;
bool checkPathEenable= false;
bool IR_Enable = false;
IRrecvLoop My_Receiver(RECV_PIN); 
IRdecode My_Decoder;

void setup() {
// put your setup code here, to run once:
uint8_t temp;
 Serial.begin(9600);
neckControllerServoMotor.attach(10);  
neckControllerServoMotor.write(90); 
delay(1000);
checkPath(); 
motorSet = "FORWARD"; 
neckControllerServoMotor.write(90);
moveStop();
My_Receiver.enableIRIn(); // Start the receiver
}
void loop() {
// put your main code here, to run repeatedly:
uint8_t temp = 0xff, temp_last = 0;
bool is_header = false;
if(IR_Enable){
if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
   My_Decoder.DumpResults();
    My_Receiver.resume(); 
    delay(500);
  if (My_Decoder.value == 0xFF629D)  //up
  { 
   My_Decoder.value = 0;
   moveForward();
 }else if(My_Decoder.value == 0xFFA857){  //down
   Serial.println(F(" Move backward"));
  My_Decoder.value = 0;
   moveBackward();
  }else if (My_Decoder.value == 0xFF22DD){ // left
   Serial.println(F(" Turn left"));
   My_Decoder.value = 0;
    turnLeft();  
    }else if (My_Decoder.value == 0xFFC23D){  //right
    My_Decoder.value = 0;
    turnRight();
    }else if (My_Decoder.value == 0xFF02FD){  
     Serial.println(F(" Stop"));
     My_Decoder.value = 0;
    smartEnable = false;
     neckControllerServoMotor.write(90);
     moveStop();
   }else if (My_Decoder.value == 0xFF42BD){  //change to bluetooth mode
            IR_Enable = false;
            smartEnable = false;
            neckControllerServoMotor.write(90);
            Serial.println(F(" This is bluetooth control mode, Please use your bluetooth controller"));
            moveStop();
         }  
  }
}else{
if (Serial.available())
{
  temp = Serial.read();
  switch (temp)
  {
   case 0x01:
    Serial.println(F( "Move forward"));
    smartEnable = false;
    moveForward();
    break;
 case 0x02:
   Serial.println(F(" Move backward"));
   smartEnable = false;
   moveBackward();
    break;  
  case 0x03:
    Serial.println(F(" Turn left"));
    smartEnable = false;
    turnLeft();
    break;
  case 0x04:
    Serial.println(F(" Turn right"));
    smartEnable = false;
    turnRight();
    break;  
  case 0x05:
   Serial.println(F(" Stop"));
   smartEnable = false;
     neckControllerServoMotor.write(90);
   moveStop();
    break;
  case 0x06:
       smartEnable = true;
       Serial.println(F("this is smart mode and you don't do anything"));
       moveStop();
       delay(1000);
       moveForward();
      break;
  case 0x07:
       IR_Enable = true;
       smartEnable = false;
       Serial.println(F("this is IR control mode, Please use your IR controller"));
       moveStop();
      break;
    default:
    break;
  }
}
}
  if(smartEnable){
     checkForward();  
     checkPath();
    }
}


int readPing() {
  delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS/US_ROUNDTRIP_CM;
  return cm;
}
void checkPath() {
  int curLeft = 0;
  int curFront = 0;
  int curRight = 0;
  int curDist = 0;
  uint16_t temp = 0;
  neckControllerServoMotor.write(150);
  delay(90); 
  for(pos = 160; pos >= 30; pos-=20)
  {
    neckControllerServoMotor.write(pos);
    delay(30);
   checkForward(); 
    temp = readPing();
    if(temp < 3){
      moveStop();continue;
      } 
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
  delay(1000);
  moveStop();
  setCourse();
}
void changePath() {
  if (pos < 90) {lookLeft();} 
  if (pos > 90) {lookRight();}
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
    leftMotor.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    rightMotor.setSpeed(speedSet);
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
  if (smartEnable){
    delay(400);
    motorSet = "FORWARD";
   leftMotor.run(FORWARD);
   rightMotor.run(FORWARD); 
  } 
  else{
  for (speedSet = 0; speedSet < TURN_MAX_SPEED; speedSet +=2)
  {
  leftMotor.setSpeed(speedSet);
  rightMotor.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
  delay(5);
  }
  }
}  
void turnLeft() {
 motorSet = "LEFT";
   leftMotor.run(BACKWARD);
   rightMotor.run(FORWARD);
   if(smartEnable){
    delay(400);
    motorSet = "FORWARD";
   leftMotor.run(FORWARD);
   rightMotor.run(FORWARD);   
   }
 else{
    for (speedSet = 0; speedSet < TURN_MAX_SPEED; speedSet +=2)
    {
    leftMotor.setSpeed(speedSet);
    rightMotor.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    delay(5);
    }
 }
}  

void lookRight() {rightMotor.run(BACKWARD); delay(400); rightMotor.run(FORWARD);}
void lookLeft() {leftMotor.run(BACKWARD); delay(400); leftMotor.run(FORWARD);}

