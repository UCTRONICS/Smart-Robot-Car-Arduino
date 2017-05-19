// ArduCAM Smart_Robot_Car demo (C)2017 Lee
// This demo support bluetooth control、IR control and default is  bluetooth control mode
//This demo support track line mode.
// This demo also support smart mode .
//If you want to the car work on smart mode, you should enable it "#define smart"  ,
//but if you enable it, Your IR control will be disable.
// It needs to be used in combination with bluetooth software.
// Before running this demo, you should enable the IR_RECV_TIMER1 
// in the ../../libraries../Arducam_Smart_Robot_Car/IRLibTimer.h!!


#define smart  

#if !(defined (smart))
  #include <IRLib.h>
  #include <IRLibTimer.h>
  #if !(defined IR_RECV_TIMER1)
    #error Please enable the IR_RECV_TIMER1 ans disable the IR_RECV_TIMER2 in the ../../libraries../Arducam_Smart_Robot_Car/IRLibTimer.h
  #endif
#endif
#include <AFMotor.h> 
#if defined (smart)
#include <Servo.h>  
#endif  
#include <NewPing.h>

#define leftSensor  A0
#define middleSensor  A1
#define rightSensor 13

#define TRIG_PIN A2 
#define ECHO_PIN A3
#define MAX_DISTANCE_POSSIBLE 1000 
#define MAX_SPEED 150 
#define TURN_MAX_SPEED 150
#define TrackspeedSet 150
#define MOTORS_CALIBRATION_OFFSET 0
#define COLL_DIST 30 
#define TURN_DIST COLL_DIST+10 

#if defined(smart)
  Servo neckControllerServoMotor; 
  NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE_POSSIBLE);
#else
  int RECV_PIN = 2;
  IRrecvLoop My_Receiver(RECV_PIN); 
  IRdecode My_Decoder;
#endif

AF_DCMotor leftMotor1(2, MOTOR12_64KHZ); 
AF_DCMotor rightMotor1(1, MOTOR12_64KHZ); 
AF_DCMotor leftMotor2(3, MOTOR34_64KHZ); 
AF_DCMotor rightMotor2(4, MOTOR34_64KHZ);

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
bool IR_Enable = false;
uint8_t mode;
void setup() {
uint8_t temp;
  Serial.begin(9600);
moveStop();
#if defined(smart) 
 neckControllerServoMotor.attach(10);  
neckControllerServoMotor.write(90); 
checkPath(); 
motorSet = "FORWARD"; 
neckControllerServoMotor.write(90);
#else
  My_Receiver.enableIRIn(); // Start the receiver
#endif
}
void loop() {
// put your main code here, to run repeatedly:
uint8_t temp = 0xff;
if(IR_Enable){
#if !(defined (smart))
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
     moveStop();
   }else if (My_Decoder.value == 0xFF42BD){  
     My_Decoder.value = 0;
    IR_Enable = false;
    smartEnable = false;
    #if defined (smart)
    neckControllerServoMotor.write(90);
    #endif
    Serial.println(F(" This is bluetooth control mode, Please use your bluetooth controller"));
    moveStop();
   }
  }
 #endif
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
   #if defined (smart)
     neckControllerServoMotor.write(90);
   #endif
   moveStop();
    break;
  case 0x06:
 #if (defined (smart))
   smartEnable = true;
   Serial.println(F(" This is smart mode and you don't do anything"));
   moveStop();
   delay(1000);
   checkForward();  
   checkPath(); 
   moveForward();
  #else
 Serial.println(F(" This mode can't support obscurity mode, please enable the smart"));
  #endif
  break; 
  case 0x07:
   IR_Enable = true;
   smartEnable = false;
   Serial.println(F(" This is IR control mode, Please use your IR controller"));
   moveStop();
  break;
 case 0x20:
  moveStop();
  smartEnable = false;
  mode = 2;
  pinMode (leftSensor,INPUT);
  pinMode (middleSensor,INPUT);
  pinMode (rightSensor,INPUT); 
  Serial.println(F("  This is Line_tracking mode, Please put your car to the black line. "));
  break;
 default:
  break;
  }
}
}
if (mode == 2){
  mode = 0;
  int num1 = 0,num2 = 0,num3 = 0;
while(1){ 

 if (Serial.available())
  {
  temp = Serial.read();
  if(temp = 0x21){
    moveStop();break;
  }
  }
  num1=digitalRead(leftSensor);  
  num2=digitalRead(middleSensor); 
  num3=digitalRead(rightSensor);  
 if((num2==0)&&(num1==0)&&(num3==0)){
  moveStop();continue;
  }else if( (num1==0)&&num3==1){  //go to right
   TrackturnLeft(); 
   while(1){ 
   num2=digitalRead(middleSensor);  
   if(num2){
   TrackturnLeft(); continue;
    } else
    break;
  } 
  }else if((num3==0)&&(num1==1)){  // go to left
    TrackturnRight();
   while(1){
   num2=digitalRead(middleSensor);  
    if(num2){
   TrackturnRight();continue;
    } else
    break;
    }
  }else
    {
    TrackmoveForward();
    }
    
  }
}
  if(smartEnable){
   checkForward();  
   checkPath();
  }
}

int readPing() {
#if defined (smart)
  delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS/US_ROUNDTRIP_CM;
  return cm;
#endif
}
void checkPath() {
  int curLeft = 0;
  int curFront = 0;
  int curRight = 0;
  int curDist = 0;
  uint16_t temp = 0;
  #if defined (smart)
  neckControllerServoMotor.write(150);
  delay(90); 
  for(pos = 160; pos >= 30; pos-=20)
  {
  neckControllerServoMotor.write(pos);
  delay(30);
 checkForward(); 
  temp = readPing();
  if(temp < 3){
    curDist = 50;
  } 
  else
  curDist= temp;  
  if (curDist < TURN_DIST) {
  moveStop(); changePath();
  }
  if (curDist > curDist) {maxAngle = pos;}
  if (pos > 90 && curDist > curLeft) { curLeft = curDist;}
  if (pos == 90 && curDist > curFront) {curFront = curDist;}
  if (pos < 90 && curDist > curRight) {curRight = curDist;}
  }
  maxLeft = curLeft;
  maxRight = curRight;
  maxFront = curFront;
 #endif
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
   leftMotor1.run(FORWARD); 
   rightMotor1.run(FORWARD); 
   leftMotor2.run(FORWARD); 
  rightMotor2.run(FORWARD);
 }
 }

void checkBackward() { 
  if (motorSet=="BACKWARD") 
  {
   leftMotor1.run(BACKWARD); 
   rightMotor1.run(BACKWARD); 
   leftMotor2.run(BACKWARD); 
   rightMotor2.run(BACKWARD);
  } 
}

void moveStop() {
leftMotor1.run(RELEASE); rightMotor1.run(RELEASE);
leftMotor2.run(RELEASE); rightMotor2.run(RELEASE);
}

void moveForward() {
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);
  rightMotor1.run(FORWARD);
   leftMotor2.run(FORWARD);
  rightMotor2.run(FORWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2)
  {
  leftMotor1.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
  rightMotor1.setSpeed(speedSet);
  leftMotor2.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
  rightMotor2.setSpeed(speedSet);
  delay(5);
  }
}

void moveBackward() {
  motorSet = "BACKWARD";
  leftMotor1.run(BACKWARD);
  rightMotor1.run(BACKWARD);
  leftMotor2.run(BACKWARD);
  rightMotor2.run(BACKWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2)
  {
  leftMotor1.setSpeed(speedSet);
  rightMotor1.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
  leftMotor2.setSpeed(speedSet);
  rightMotor2.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
  delay(5);
  }
}  

void turnRight() {
 motorSet = "RIGHT";
 leftMotor1.run(FORWARD);
  rightMotor1.run(BACKWARD);
  leftMotor2.run(FORWARD);
  rightMotor2.run(BACKWARD);
  if(smartEnable){
    delay(500);
    motorSet = "FORWARD";
   leftMotor1.run(FORWARD);
   rightMotor1.run(FORWARD); 
   leftMotor2.run(FORWARD);
   rightMotor2.run(FORWARD); 
  }else{
  for (speedSet = 0; speedSet < TURN_MAX_SPEED; speedSet +=2)
  {
  leftMotor1.setSpeed(speedSet);
  rightMotor1.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
  leftMotor2.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
  rightMotor2.setSpeed(speedSet); 
  delay(5);
  }
  }
}  

void TrackmoveForward() {
   motorSet = "FORWARD";
   leftMotor1.run(FORWARD);
   rightMotor1.run(FORWARD);
   leftMotor2.run(FORWARD);
  rightMotor2.run(FORWARD);
  leftMotor1.setSpeed(TrackspeedSet+MOTORS_CALIBRATION_OFFSET);
  rightMotor1.setSpeed(TrackspeedSet);
  leftMotor2.setSpeed(TrackspeedSet+MOTORS_CALIBRATION_OFFSET);
  rightMotor2.setSpeed(TrackspeedSet);
}

void TrackturnRight() {
  motorSet = "RIGHT";
 leftMotor1.run(FORWARD);
  rightMotor1.run(BACKWARD);
  leftMotor2.run(FORWARD);
  rightMotor2.run(BACKWARD);
   leftMotor1.setSpeed(TrackspeedSet);
  rightMotor1.setSpeed(TrackspeedSet+MOTORS_CALIBRATION_OFFSET);
  leftMotor2.setSpeed(TrackspeedSet);
  rightMotor2.setSpeed(TrackspeedSet+MOTORS_CALIBRATION_OFFSET);
}  

void TrackturnLeft() {
  motorSet = "LEFT";
  leftMotor1.run(BACKWARD);
  rightMotor1.run(FORWARD);
  leftMotor2.run(BACKWARD);
  rightMotor2.run(FORWARD);
  leftMotor1.setSpeed(TrackspeedSet);
  rightMotor1.setSpeed(TrackspeedSet+MOTORS_CALIBRATION_OFFSET);
  leftMotor2.setSpeed(TrackspeedSet);
  rightMotor2.setSpeed(TrackspeedSet+MOTORS_CALIBRATION_OFFSET);
}  
 void turnLeft(){
  motorSet = "LEFT";
  leftMotor1.run(BACKWARD);
  rightMotor1.run(FORWARD);
  leftMotor2.run(BACKWARD);
  rightMotor2.run(FORWARD);
  if(smartEnable){
    delay(500);
    motorSet = "FORWARD";
   leftMotor1.run(FORWARD);
   rightMotor1.run(FORWARD); 
   leftMotor2.run(FORWARD);
   rightMotor2.run(FORWARD); 
  } 
  else{
    for (speedSet = 0; speedSet < TURN_MAX_SPEED; speedSet +=2)
    {
    leftMotor1.setSpeed(speedSet);
    rightMotor1.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    leftMotor2.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    rightMotor2.setSpeed(speedSet);
    delay(5);
    }
  }
  }
void lookRight() {
  rightMotor1.run(BACKWARD); delay(400); leftMotor1.run(FORWARD);
  rightMotor2.run(BACKWARD); delay(400); leftMotor2.run(FORWARD);
  }
void lookLeft() {
 leftMotor1.run(BACKWARD); delay(400);rightMotor1.run(FORWARD);
 leftMotor2.run(BACKWARD); delay(400);rightMotor2.run(FORWARD);
  }
