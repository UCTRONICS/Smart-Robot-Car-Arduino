// ArduCAM Mini demo (C)2017 Lee
// Web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with ArduCAM Mini camera, and can run on any Arduino platform.
// This demo was made for ArduCAM_Mini_5MP_Plus.
// It needs to be used in combination with PC software.
// It can take photo continuously as video streaming.
//
// The demo sketch will do the following tasks:
// 1. Set the camera to JPEG output mode.
// 2. Read data from Serial port and deal with it
// 3. If receive 0x00-0x08,the resolution will be changed.
// 4. If receive 0x10,camera will capture a JPEG photo and buffer the image to FIFO.Then write datas to Serial port.
// 5. If receive 0x20,camera will capture JPEG photo and write datas continuously.Stop when receive 0x21.
// 6. If receive 0x30,camera will capture a BMP  photo and buffer the image to FIFO.Then write datas to Serial port.
// 7. If receive 0x11 ,set camera to JPEG output mode.
// 8. If receive 0x31 ,set camera to BMP  output mode.
// This program requires the ArduCAM V4.0.0 (or later) library and ArduCAM_Mini_5MP_Plus
// and use Arduino IDE 1.6.8 compiler or above
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
//This demo can only work on OV2640_MINI_2MP or OV5642_MINI_5MP or OV5642_MINI_5MP_BIT_ROTATION_FIXED platform.
#if !(defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined OV2640_MINI_2MP || defined OV3640_MINI_3MP)
  #error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

#include <IRLib.h>
#include <AFMotor.h> 
//#include <Servo.h>    
#include <NewPing.h>

#define MISO  9
#define MOSI  8
#define SCK   12

#define TRIG_PIN A2 
#define ECHO_PIN A3
#define MAX_DISTANCE_POSSIBLE 1000 
#define MAX_SPEED 150 
#define MOTORS_CALIBRATION_OFFSET 0
#define COLL_DIST 20 
#define TURN_DIST COLL_DIST+10 

int RECV_PIN = 2;
IRrecvLoop My_Receiver(RECV_PIN); //comment out previous line and un-comment this for loop version
IRdecode My_Decoder;

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE_POSSIBLE);
AF_DCMotor leftMotor(3, MOTOR34_64KHZ); 
AF_DCMotor rightMotor(4, MOTOR34_64KHZ); 

AF_DCMotor leftMotor1(1, MOTOR12_64KHZ); 
AF_DCMotor rightMotor1(2, MOTOR12_64KHZ); 

//Servo //neckControllerServoMotor;

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

// set pin 7 as the slave select for the digital pot:
const int CS = 4;
bool is_header = false;
int mode = 0;
uint8_t start_capture = 0;
#if defined (OV2640_MINI_2MP)
  ArduCAM myCAM( OV2640,MISO, MOSI, SCK,  CS );
 // ArduCAM myCAM( OV2640,  CS );
#elif defined (OV3640_MINI_3MP)
ArduCAM myCAM( OV3640, CS );
#else
  ArduCAM myCAM( OV5642, CS );
#endif
uint8_t read_fifo_burst(ArduCAM myCAM);
void setup() {
// put your setup code here, to run once:
uint8_t vid, pid;
uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
  Serial.begin(115200);
#else
  Wire.begin();
  Serial.begin(9600);
#endif
Serial.println(F("ACK CMD ArduCAM Start!"));
// set the CS as an output:
pinMode(CS, OUTPUT);
//while(1){
//  //Check if the ArduCAM SPI bus is OK
//  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
//  temp = myCAM.read_reg(ARDUCHIP_TEST1);
//  if (temp != 0x55){
//    Serial.println(F("ACK CMD SPI interface Error!"));
//    delay(1000);continue;
//  }else{
//    Serial.println(F("ACK CMD SPI interface OK."));break;
//  }
//}

//#if defined (OV2640_MINI_2MP)
//  while(1){
//    //Check if the camera module type is OV2640
//    myCAM.wrSensorReg8_8(0xff, 0x01);
//    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
//    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
//    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
//      Serial.println(F("ACK CMD Can't find OV2640 module!"));
//      delay(1000);continue;
//    }
//    else{
//      Serial.println(F("ACK CMD OV2640 detected."));break;
//    } 
//  }
//#elif defined (OV3640_MINI_3MP)
//  while(1){
//    //Check if the camera module type is OV3640
//    myCAM.rdSensorReg16_8(OV3640_CHIPID_HIGH, &vid);
//    myCAM.rdSensorReg16_8(OV3640_CHIPID_LOW, &pid);
//    if ((vid != 0x36) || (pid != 0x4C)){
//      Serial.println(F("Can't find OV3640 module!"));
//      delay(1000);continue; 
//    }else{
//      Serial.println(F("OV3640 detected."));break;    
//    }
// } 
//  
//#else
//  while(1){
//    //Check if the camera module type is OV5642
//    myCAM.wrSensorReg16_8(0xff, 0x01);
//    myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
//    myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
//    if((vid != 0x56) || (pid != 0x42)){
//      Serial.println(F("ACK CMD Can't find OV5642 module!"));
//      delay(1000);continue;
//    }
//    else{
//      Serial.println(F("ACK CMD OV5642 detected."));break;
//    } 
//  }
//#endif
////Change to JPEG capture mode and initialize the OV5642 module
//myCAM.set_format(JPEG);
//myCAM.InitCAM();
//#if defined (OV2640_MINI_2MP)
//  myCAM.OV2640_set_JPEG_size(OV2640_320x240);
//#elif defined (OV3640_MINI_3MP)
//  myCAM.OV3640_set_JPEG_size(OV3640_320x240);
//#else
//  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
//  myCAM.OV5642_set_JPEG_size(OV5642_320x240);
//#endif
//delay(1000);
//myCAM.clear_fifo_flag();
//#if !(defined (OV2640_MINI_2MP))
//myCAM.write_reg(ARDUCHIP_FRAMES,0x00);
//#endif
////neckControllerServoMotor.attach(10);  
////neckControllerServoMotor.write(90); 
delay(2000);
checkPath(); 
motorSet = "FORWARD"; 
////neckControllerServoMotor.write(90);
moveStop();
}
void loop() {
// put your main code here, to run repeatedly:
uint8_t temp = 0xff, temp_last = 0;
bool is_header = false;
moveForward();
My_Receiver.enableIRIn(); // Start the receiver

if (Serial.available())
{
  temp = Serial.read();
  switch (temp)
  {
   case 0x07:
          Serial.println(F("Move forward"));
          smartEnable = false; checkPathEenable = true;
           checkPathIR(); 
           moveForward();
          break;
      case 0x03:
          Serial.println(F("Turn left"));
            turnLeft(); checkPathIR();
          break;
       case 0x05:
          Serial.println(F("Turn right"));
            smartEnable = false; checkPathEenable = true;
            turnRight();checkPathIR();
          break;
          case 0x01:
          Serial.println(F("Move backward"));checkPathEenable = false;
           moveBackward();
          break;
          case 0x04:
           smartEnable = false;checkPathEenable = false; 
            Serial.println(F("Stop"));
           moveStop();
          break;
          case 0x02:
           smartEnable = true;checkPathEenable = false; 
            Serial.println(F("this is smart mode and you don't do anything"));
           moveStop();
          break;
    case 0x00:
    mode = 1;
    temp = 0xff;
    start_capture = 1;
    Serial.println(F("ACK CMD CAM start single shoot."));
    break;
    case 0x11: 
    temp = 0xff;
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    #if !(defined (OV2640_MINI_2MP))
    myCAM.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
    #endif
    break;
    case 0x20:
    mode = 2;
    temp = 0xff;
    start_capture = 2;
    Serial.println(F("ACK CMD CAM start video streaming."));
    break;
    default:
    break;
  }
}
if (mode == 1)
{
  if (start_capture == 1)
  {
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    //Start capture
    myCAM.start_capture();
    start_capture = 0;
  }
  if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
  {
    Serial.println(F("ACK CMD CAM Capture Done."));
    read_fifo_burst(myCAM);
    //Clear the capture done flag
    myCAM.clear_fifo_flag();
  }
}
else if (mode == 2)
{
  while (1)
  {
    temp = Serial.read();
    if (temp == 0x21)
    {
      start_capture = 0;
      mode = 0;
      Serial.println(F("ACK CMD CAM stop video streaming."));
      break;
    }
    if (start_capture == 2)
    {
      myCAM.flush_fifo();
      myCAM.clear_fifo_flag();
      //Start capture
      myCAM.start_capture();
      start_capture = 0;
    }
    if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    {
      uint32_t length = 0;
      length = myCAM.read_fifo_length();
      if ((length >= MAX_FIFO_SIZE) | (length == 0))
      {
        myCAM.clear_fifo_flag();
        start_capture = 2;
        continue;
      }
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();//Set fifo burst mode
     // temp =  myCAM.SoftSPItransfer(0x00);
      length --;
      while ( length-- )
      {
        temp_last = temp;
        //temp =  myCAM.SoftSPItransfer(0x00);
        temp =  SPI.transfer(0x00);
        if (is_header == true)
        {
          Serial.write(temp);
        }
        else if ((temp == 0xD8) & (temp_last == 0xFF))
        {
          is_header = true;
          Serial.println(F("ACK IMG"));
          Serial.write(temp_last);
          Serial.write(temp);
        }
        if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
        break;
        delayMicroseconds(15);
      }
      myCAM.CS_HIGH();
      myCAM.clear_fifo_flag();
      start_capture = 2;
      is_header = false;
    }
  }
}

if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
   My_Decoder.DumpResults();
    My_Receiver.resume(); 
  }

//if (irrecv.decode(&results)) {
//   // Serial.println(results.value, HEX);
//    irrecv.resume(); // Receive the next value
//  }
//  if (results.value == 0xFF629D)  //up
//  {    
//       smartEnable = false;checkPathEenable = true;
//       checkPathIR(); 
//       moveForward();
//    }else if(results.value == 0xFFA857){  //dowm
//      smartEnable = false;checkPathEenable = true;
//       moveBackward();
//      checkPathIR();
//       }else if (results.value == 0xFF22DD){ // left
//        smartEnable = false;
//        turnLeft(); checkPathIR();
//        
//        }else if (results.value == 0xFFC23D){  //right
//          smartEnable = false;checkPathEenable = true;
//          turnRight();
//           checkPathIR();
//          }else if (results.value == 0xFF02FD){  //right
//          smartEnable = false;
//           moveStop();
//           checkPathEenable=false;
//          }

  if(smartEnable){
     checkForward();  
     checkPath();
    }
    if(checkPathEenable)
    checkPathIR();

}
uint8_t read_fifo_burst(ArduCAM myCAM)
{
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  length = myCAM.read_fifo_length();
  Serial.println(length, DEC);
  if (length >= MAX_FIFO_SIZE) //512 kb
  {
    Serial.println(F("Over size."));
    return 0;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
    return 0;
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();//Set fifo burst mode
   //temp =  myCAM.SoftSPItransfer(0x00);
   temp =  myCAM.transfer(0x00);
  length --;
  while ( length-- )
  {
    temp_last = temp;
   // temp =  myCAM.SoftSPItransfer(0x00);
    temp =  myCAM.transfer(0x00);
    if (is_header == true)
    {
      Serial.write(temp);
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      Serial.println(F("ACK IMG"));
      Serial.write(temp_last);
      Serial.write(temp);
    }
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    break;
    delayMicroseconds(15);
  }
  myCAM.CS_HIGH();
  is_header = false;
  return 1;
}

int readPing() {
  delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS/US_ROUNDTRIP_CM;
  return cm;
}


void checkPathIR() {
  int curLeft = 0;
  int curFront = 0;
  int curRight = 0;
  int curDist = 0;
  uint16_t temp = 0;
  //neckControllerServoMotor.write(150);
  delay(120); 
  for(pos = 160; pos >= 30; pos-=20)
  {
    //neckControllerServoMotor.write(pos);
    delay(90);
   //checkForward(); 
    temp = readPing();
    if(temp < 3)
      curDist = 50;
      else
      curDist= temp;  
    if (curDist < TURN_DIST) {
      maxAngle = pos;
      changePath();
    }else{
        //neckControllerServoMotor.write(90);
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


void checkPath() {
  int curLeft = 0;
  int curFront = 0;
  int curRight = 0;
  int curDist = 0;
  uint16_t temp = 0;
  //neckControllerServoMotor.write(150);
  delay(120); 
  for(pos = 160; pos >= 30; pos-=20)
  {
    //neckControllerServoMotor.write(pos);
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
     leftMotor1.run(FORWARD);
    rightMotor1.run(FORWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2)
  {
    leftMotor.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    rightMotor.setSpeed(speedSet);
    leftMotor1.setSpeed(speedSet+MOTORS_CALIBRATION_OFFSET);
    rightMotor1.setSpeed(speedSet);
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
  delay(600);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);      
}  
void turnLeft() {
  motorSet = "LEFT";
  leftMotor.run(BACKWARD);
  rightMotor.run(FORWARD);
  delay(600);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
}  

void lookRight() {rightMotor.run(BACKWARD); delay(400); rightMotor.run(FORWARD);}
void lookLeft() {leftMotor.run(BACKWARD); delay(400); leftMotor.run(FORWARD);}

