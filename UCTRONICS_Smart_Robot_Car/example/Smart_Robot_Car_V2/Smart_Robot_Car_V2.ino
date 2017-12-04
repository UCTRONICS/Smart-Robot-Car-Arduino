
// ArduCAM Smart_Robot_Car demo (C)2017 Lee
// This demo support smart mode .
// This demo support bluetooth control and IR remote control.
// The default mode is bluetooth control you can change it to
// the IR control mode by touching the IRENABLE key.
// We optimize the IR control code for more sensitive control.
// When there are some obstacles ahead, the car will stop.
//video link: https://youtu.be/0FB7J-Qzcag
//Before uploading this demo,you should remove your HC-05 module.
//If not you can not uploading to the UNO board.
/***********************[NOTICE]*********************************
  We can't guarantee that the motor load
  is exactly the same, so it increases the compensation
  factor. You should adjust them to suit for your motor
****************************************************************/
#define leftFactor 10
#define rightFactor 5
#define speedSet  150

#define TURN_DIST 40
#include <UCMotor.h>
#include <Servo.h>
#include "UCNEC.h"

#define TRIG_PIN A2
#define ECHO_PIN A3

UCNEC myIR(2);
UC_DCMotor leftMotor(3, MOTOR34_64KHZ);
UC_DCMotor rightMotor(4, MOTOR34_64KHZ);
Servo neckControllerServoMotor;

unsigned int S;
unsigned int Sleft;
unsigned int Sright;
int RECV_PIN = 2;
bool smartEnable = false;  //This flag is used for enable smart mode
bool IR_Enable = false;    //This flag is used for enable ir remote control mode
bool detected_flag = false; //This flag is used for enable detected obstacles ahead
void setup() {
  // put your setup code here, to run once:
  uint8_t temp;
  Serial.begin(9600);
  pinMode(ECHO_PIN, INPUT); //Set the connection pin output mode Echo pin
  pinMode(TRIG_PIN, OUTPUT);//Set the connection pin output mode trog pin
  neckControllerServoMotor.attach(10);
  neckControllerServoMotor.write(90);
  delay(2000);
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
          S = readPing();
          if (S <= 30) {
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
      S = readPing();
      if (S <= 30) {
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
    S = readPing();
    if (S <= 30) {
      moveStop();
    }
  }
  if (smartEnable) {
    neckControllerServoMotor.write(90);
    S = readPing();
    if (S <= TURN_DIST ) {
      turn();
    } else if (S > TURN_DIST) {
      moveForward();
    }
  }
}

void turn() {
  moveStop();
  neckControllerServoMotor.write(150);
  delay(500);
  S = readPing();
  Sleft = S;
  neckControllerServoMotor.write(90);
  delay(500);
  neckControllerServoMotor.write(30);
  delay(500);
  S = readPing();
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
int readPing()
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, cm;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, LOW);

  pinMode(ECHO_PIN, INPUT);
  duration = pulseIn(ECHO_PIN, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  return cm ;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
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

