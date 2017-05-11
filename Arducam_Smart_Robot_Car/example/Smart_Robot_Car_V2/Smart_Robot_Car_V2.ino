#include <AFMotor.h> 
#include <Servo.h>    
#include <NewPing.h>

#define TRIG_PIN A2 
#define ECHO_PIN A3
#define MAX_DISTANCE_POSSIBLE 1000 
#define MAX_SPEED 150 
#define MOTORS_CALIBRATION_OFFSET 0
#define COLL_DIST 20 
#define TURN_DIST COLL_DIST+10 
#include <IRLib.h>
int RECV_PIN = 2;
////irrecv //irrecv(RECV_PIN);
//decode_//results //results;

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

IRrecvLoop My_Receiver(RECV_PIN); //comment out previous line and un-comment this for loop version
IRdecode My_Decoder;

void setup() {
// put your setup code here, to run once:
uint8_t vid, pid;
uint8_t temp;
 Serial.begin(9600);

//irrecv.enableIRIn(); // Start the receiver 
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

if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
   My_Decoder.DumpResults();
    My_Receiver.resume(); 
    delay(500);
   if (My_Decoder.value == 0xFF629D)  //up
  {   
    Serial.println(F("Move forward"));
       My_Decoder.value = 0;
      smartEnable = true;
      moveForward();
   }else if(My_Decoder.value == 0xFFA857){  //down
     Serial.println(F("Move backward"));
      My_Decoder.value = 0;
       smartEnable = false;
       moveBackward();
      }else if (My_Decoder.value == 0xFF22DD){ // left
         Serial.println(F("Turn left"));
         My_Decoder.value = 0;
        moveForward();
        turnLeft();
        smartEnable = true;   
        }else if (My_Decoder.value == 0xFFC23D){  //right
          My_Decoder.value = 0;
          moveForward();
          turnRight();
          smartEnable = true;
          }else if (My_Decoder.value == 0xFF02FD){  
           Serial.println(F("Turn right"));
           My_Decoder.value = 0;
          smartEnable = false;
           moveStop();
         }  
  }


if (Serial.available())
{
  temp = Serial.read();
  switch (temp)
  {
   case 0x01:
          Serial.println(F("Move forward"));
          smartEnable = true;
          moveForward();
          break;
   case 0x02:
       Serial.println(F("Move backward"));
       smartEnable = false;
       moveBackward();
        break;  
  case 0x03:
        Serial.println(F("Turn left"));
        moveForward();
        turnLeft();
        smartEnable = true; 
        break;
  case 0x04:
          Serial.println(F("Turn right"));
          moveForward();
          turnRight();
          smartEnable = true;
        break;  
  case 0x05:
         smartEnable = false;
         Serial.println(F("Stop"));
         moveStop();
        break;
  case 0x06:
       smartEnable = true;
       Serial.println(F("this is smart mode and you don't do anything"));
       moveStop();
       delay(1000);
       moveForward();
      break;
    default:
    break;
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
  delay(500);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);      
}  
void turnLeft() {
  motorSet = "LEFT";
  leftMotor.run(BACKWARD);
  rightMotor.run(FORWARD);
  delay(500);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
}  

void lookRight() {rightMotor.run(BACKWARD); delay(400); rightMotor.run(FORWARD);}
void lookLeft() {leftMotor.run(BACKWARD); delay(400); leftMotor.run(FORWARD);}

