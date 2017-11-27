//Smart_Robot_Car_V3 demo (C)2017 Lee
// This version support wifi camera
//This demo need top be used in combination with Android software
//With this demo, you can make your Robot car take picture and record video.
//NOTICE: Before uploadig this demo, you should remove the WIFI board from UNO.

#include <NewPing.h>
#include <AFMotor.h>
#include <Servo.h>
#include <Wire.h>

#define TRIG_PIN A2
#define ECHO_PIN A3
#define MAX_DISTANCE_POSSIBLE 1000
#define DEFLE_SPEED 50

//Define the tracking pin
#define leftSensor    A0
#define middleSensor  A1
#define rightSensor   13
int buzzerPin = 2; //Beep pin
int MAX_SPEED_LEFT ;
int MAX_SPEED_RIGHT;
int TURN_MAX_SPEED_LEFT = 250;
int TURN_MAX_SPEED_RIGHT = 250;
int SZ_SPEEDPRO = 0;
int SZ_SPEEDTHR = 150;

byte serialIn = 0;
byte commandAvailable = false;
String strReceived = "";

//The center Angle of two steering engines.
byte servoXCenterPoint = 88;
byte servoYCenterPoint = 70;

//The maximum Angle of two steering gear
byte servoXmax = 170;
byte servoYmax = 130;

//The minimum Angle of two steering gear
byte servoXmini = 10;
byte servoYmini = 10;

//The current Angle of the two steering engines is used for retransmission
byte servoXPoint = 0;
byte servoYPoint = 0;

//The speed of the motor
byte leftspeed = 0;
byte rightspeed = 0;

//Step precision
byte servoStep = 4;

//status flag
String motorSet = "";
bool detected_flag = false;
bool stopFlag = false;
bool timeFlag = true;
long currentTime = 0;



AF_DCMotor leftMotor1(3, MOTOR34_64KHZ);
AF_DCMotor rightMotor1(4, MOTOR34_64KHZ);
AF_DCMotor leftMotor2(1, MOTOR34_64KHZ);
AF_DCMotor rightMotor2(2, MOTOR34_64KHZ);
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE_POSSIBLE);

Servo servoX;
Servo servoY;

void setup()
{
  pinMode(leftSensor, INPUT_PULLUP);
  pinMode(middleSensor, INPUT_PULLUP);
  pinMode(rightSensor, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  servoX.attach(9);
  servoY.attach(10);
  Serial.begin(115200);
  delay(5000);
  servo_center();
  MAX_SPEED_LEFT = SZ_SPEEDTHR;
  MAX_SPEED_RIGHT = SZ_SPEEDTHR;
}

void loop()
{
  getSerialLine();
  if (commandAvailable) {
    processCommand(strReceived);
    strReceived = "";
    commandAvailable = false;
  }
}

void getSerialLine()
{
  static int temp = 0;
  while (serialIn != '\r')
  {
    if (!(Serial.available() > 0))
    {
      if (timeFlag) {
        currentTime = micros();
        timeFlag = false;
      }
      if (  micros() - currentTime >= 300000) {
        timeFlag = true;
        currentTime = micros();
        if (detected_flag) {
          temp = readPing();
          if ( temp <= 40 && temp > 0 ) {
            moveStop();
          }
        }
      }
      return;
    }
    serialIn = Serial.read();
    if (serialIn != '\r') {
      if (serialIn != '\n') {
        char a = char(serialIn);
        strReceived += a;
      }
    }
  }
  if (serialIn == '\r') {
    commandAvailable = true;
    serialIn = 0;
  }
}

void processCommand(String input)
{
  String command = getValue(input, ' ', 0);
  static String SZ_speed, SZ_T, SZ_K;
  byte iscommand = true;
  int val;
  if (command == "MD_up")
  {
    detected_flag = true;
    moveForward();
  } else if (command == "MD_up_left" || command == "MD_up_right" || command == "MD_down_left" || command == "MD_down_right")
  {
    movePianZhuan (command.substring(command.indexOf("_") + 1));
    detected_flag = false;
  } else if (command == "MD_down")
  {
    moveBackward(); detected_flag = false;
  } else if (command == "MD_left")
  {
    turnLeft(); detected_flag = false;
  } else if (command == "MD_right")
  {
    turnRight(); detected_flag = false;
  } else if (command == "MD_stop")
  {
    moveStop();
  } else if (command == "track")
  {
    moveTrack();
  } else if (command == "stop")
  {
    moveStop();
    stopFlag = true;
  } else if (command == "MD_SD")
  {
    val = getValue(input, ' ', 1).toInt();
    leftspeed = val;
    val = getValue(input, ' ', 2).toInt();
    rightspeed = val;
  } else if (command == "DJ_up")
  {
    servo_up();
  } else if (command == "DJ_down")
  {
    servo_down();
  } else if (command == "DJ_left")
  {
    servo_left();
  } else if (command == "DJ_right")
  {
    servo_right();
  } else if (command == "DJ_middle")
  {
    servo_center();
  }
  else if (command == "beep")
  {
    digitalWrite(buzzerPin, HIGH);
  } else if (command == "beep_stop")
  {
    digitalWrite(buzzerPin, LOW);
  }
  else if (!(command.indexOf("SZ") < 0))
  {
    if (!(command.indexOf("_K") < 0))
    {
      SZ_K = command.substring(command.indexOf("K") + 1, command.indexOf("T"));
      SZ_SPEEDPRO = SZ_K.toInt ();
      SZ_T = command.substring(command.indexOf("T") + 1);
      SZ_SPEEDTHR = SZ_T.toInt ();
      if (SZ_SPEEDPRO < 0)
      {
        if (SZ_SPEEDTHR + abs (SZ_SPEEDPRO) > 255)
        {
          MAX_SPEED_RIGHT = 255;
        }
        else
        {
          MAX_SPEED_RIGHT = SZ_SPEEDTHR + abs (SZ_SPEEDPRO);
        }
        MAX_SPEED_LEFT = SZ_SPEEDTHR;

      } else if (SZ_SPEEDPRO > 0 || SZ_SPEEDPRO == 0)
      {
        if (SZ_SPEEDTHR + SZ_SPEEDPRO > 255)
        {
          MAX_SPEED_LEFT = 255;
        }
        else
        {
          MAX_SPEED_LEFT = SZ_SPEEDTHR + SZ_SPEEDPRO;
        }
        MAX_SPEED_RIGHT = SZ_SPEEDTHR;

      }
      leftMotor1.setSpeed(MAX_SPEED_LEFT);
      rightMotor1.setSpeed(MAX_SPEED_RIGHT);
      leftMotor2.setSpeed(MAX_SPEED_LEFT);
      rightMotor2.setSpeed(MAX_SPEED_RIGHT);
    }
  }
  else
  {
    iscommand = false;
  }
  if (iscommand) {
    SendMessage("cmd:" + input);
  }
}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void servo_right(void)
{
  int servotemp = servoX.read();
  servotemp -= servoStep;
  servo_Horizontal(servotemp);
}
void servo_left(void)
{
  int servotemp = servoX.read();
  servotemp += servoStep;
  servo_Horizontal(servotemp);
}
void servo_down(void)
{
  int servotemp = servoY.read();
  servotemp += servoStep;
  servo_Vertical(servotemp);
}
void servo_up(void)
{
  int servotemp = servoY.read();
  servotemp -= servoStep;
  servo_Vertical(servotemp);
}
void servo_center(void)
{
  servo_Vertical(servoYCenterPoint);
  servo_Horizontal(servoXCenterPoint);
}
void servo_Vertical(int corner)
{
  int cornerY = servoY.read();
  if (cornerY > corner) {
    for (int i = cornerY; i > corner; i = i - servoStep) {
      servoY.write(i);
      servoYPoint = i;
      delay(50);
    }
  }
  else {
    for (int i = cornerY; i < corner; i = i + servoStep) {
      servoY.write(i);
      servoYPoint = i;
      delay(50);
    }
  }
  servoY.write(corner);
  servoYPoint = corner;
}
void servo_Horizontal(int corner)
{
  int i = 0;
  byte cornerX = servoX.read();
  if (cornerX > corner) {
    for (i = cornerX; i > corner; i = i - servoStep) {
      servoX.write(i);
      servoXPoint = i;
      delay(50);
    }
  }
  else {
    for (i = cornerX; i < corner; i = i + servoStep) {
      servoX.write(i);
      servoXPoint = i;
      delay(50);
    }
  }
  servoX.write(corner);
  servoXPoint = corner;
}
void moveForward(void)
{
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);
  rightMotor1.run(FORWARD);
  leftMotor2.run(FORWARD);
  rightMotor2.run(FORWARD);
  leftMotor1.setSpeed(MAX_SPEED_LEFT);
  rightMotor1.setSpeed(MAX_SPEED_RIGHT);
  leftMotor2.setSpeed(MAX_SPEED_LEFT);
  rightMotor2.setSpeed(MAX_SPEED_RIGHT);
}
void movePianZhuan (String mode)
{
  static int MAX_SPEED_LEFT_A, MAX_SPEED_RIGHT_A, MODE;
  if (!(mode.indexOf("up") < 0))
  {
    if (mode.indexOf("left") > 0)
    {
      MAX_SPEED_RIGHT_A = MAX_SPEED_RIGHT > (255 - (int)(MAX_SPEED_RIGHT * 0.5)) ? 255 : MAX_SPEED_RIGHT + (int)(MAX_SPEED_RIGHT * 0.5);
      MAX_SPEED_LEFT_A  = (MAX_SPEED_LEFT - (int)(MAX_SPEED_LEFT * 0.5)) < 0 ? 0 : MAX_SPEED_LEFT - (int)(MAX_SPEED_LEFT * 0.5);
    }
    else if (mode.indexOf("right") > 0)
    {
      MAX_SPEED_LEFT_A = MAX_SPEED_LEFT > (255 - (int)(MAX_SPEED_LEFT * 0.5)) ? 255 : MAX_SPEED_LEFT + (int)(MAX_SPEED_LEFT * 0.5);
      MAX_SPEED_RIGHT_A  = (MAX_SPEED_RIGHT - (int)(MAX_SPEED_RIGHT * 0.5)) < 0 ? 0 : MAX_SPEED_RIGHT - (int)(MAX_SPEED_RIGHT * 0.5);
    }
    leftMotor1.run(FORWARD);
    rightMotor1.run(FORWARD);
    leftMotor2.run(FORWARD);
    rightMotor2.run(FORWARD);
  }
  if (!(mode.indexOf("down") < 0))
  {
    if (mode.indexOf("left") > 0)
    {
      MAX_SPEED_RIGHT_A = MAX_SPEED_RIGHT > (255 - (int)(MAX_SPEED_RIGHT * 0.5)) ? 255 : MAX_SPEED_RIGHT + (int)(MAX_SPEED_RIGHT * 0.5);
      MAX_SPEED_LEFT_A  = (MAX_SPEED_LEFT - (int)(MAX_SPEED_LEFT * 0.5)) < 0 ? 0 : MAX_SPEED_LEFT - (int)(MAX_SPEED_LEFT * 0.5);
    }
    else if (mode.indexOf("right") > 0)
    {
      MAX_SPEED_LEFT_A = MAX_SPEED_LEFT > (255 - (int)(MAX_SPEED_LEFT * 0.5)) ? 255 : MAX_SPEED_LEFT + (int)(MAX_SPEED_LEFT * 0.5);
      MAX_SPEED_RIGHT_A  = (MAX_SPEED_RIGHT - (int)(MAX_SPEED_RIGHT * 0.5)) < 0 ? 0 : MAX_SPEED_RIGHT - (int)(MAX_SPEED_RIGHT * 0.5);
    }
    leftMotor1.run(BACKWARD);
    rightMotor1.run(BACKWARD);
    leftMotor2.run(BACKWARD);
    rightMotor2.run(BACKWARD);
  }
  leftMotor1.setSpeed(MAX_SPEED_LEFT_A);
  rightMotor1.setSpeed(MAX_SPEED_RIGHT_A);
  leftMotor2.setSpeed(MAX_SPEED_LEFT_A);
  rightMotor2.setSpeed(MAX_SPEED_RIGHT_A);
}
void moveBackward(void)
{
  motorSet = "BACKWARD";
  leftMotor1.run(BACKWARD);
  rightMotor1.run(BACKWARD);
  leftMotor2.run(BACKWARD);
  rightMotor2.run(BACKWARD);
  leftMotor1.setSpeed(MAX_SPEED_LEFT);
  rightMotor1.setSpeed(MAX_SPEED_RIGHT);
  leftMotor2.setSpeed(MAX_SPEED_LEFT);
  rightMotor2.setSpeed(MAX_SPEED_RIGHT);
}
void turnRight(void)
{
  static int MAX_SPEED_LEFT_AR, MAX_SPEED_RIGHT_AR;
  motorSet = "RIGHT";
  leftMotor1.run(FORWARD);
  rightMotor1.run(BACKWARD);
  leftMotor2.run(FORWARD);
  rightMotor2.run(BACKWARD);
  MAX_SPEED_LEFT_AR = MAX_SPEED_LEFT > 200 ? 200 : MAX_SPEED_LEFT;
  MAX_SPEED_RIGHT_AR = MAX_SPEED_RIGHT > 200 ? 200 : MAX_SPEED_RIGHT;
  leftMotor1.setSpeed(MAX_SPEED_LEFT_AR);
  rightMotor1.setSpeed(MAX_SPEED_RIGHT_AR);
  leftMotor2.setSpeed(MAX_SPEED_LEFT_AR);
  rightMotor2.setSpeed(MAX_SPEED_RIGHT_AR);
}
void turnLeft(void)
{
  static int MAX_SPEED_LEFT_AL, MAX_SPEED_RIGHT_AL;
  motorSet = "LEFT";
  leftMotor1.run(BACKWARD);
  rightMotor1.run(FORWARD);
  leftMotor2.run(BACKWARD);
  rightMotor2.run(FORWARD);
  MAX_SPEED_LEFT_AL = MAX_SPEED_LEFT > 200 ? 200 : MAX_SPEED_LEFT;
  MAX_SPEED_RIGHT_AL = MAX_SPEED_RIGHT > 200 ? 200 : MAX_SPEED_RIGHT;
  leftMotor1.setSpeed(MAX_SPEED_LEFT_AL);
  rightMotor1.setSpeed(MAX_SPEED_RIGHT_AL);
  leftMotor2.setSpeed(MAX_SPEED_LEFT_AL);
  rightMotor2.setSpeed(MAX_SPEED_RIGHT_AL);
}
void moveStop(void)
{
  leftMotor1.run(RELEASE); rightMotor1.run(RELEASE);
  leftMotor2.run(RELEASE); rightMotor2.run(RELEASE);
}
void moveTrack(void)
{
  int temp = 0, num1 = 0, num2 = 0, num3 = 0;
  strReceived = "";
  commandAvailable = false;
  while (1) {
    if ((Serial.available() > 0))
    {
      serialIn = Serial.read();
      if (serialIn != '\r') {
        if (serialIn != '\n') {
          char a = char(serialIn);
          strReceived += a;
        }
      }
    }
    if (serialIn == '\r') {
      commandAvailable = true;
      serialIn = 0;
      processCommand(strReceived);
      strReceived = "";
      commandAvailable = false;
    }
    if (stopFlag) {
      stopFlag = false;
      moveStop();
      strReceived = "";
      commandAvailable = false;
      break;
    }
    num1 = digitalRead(leftSensor);
    num2 = digitalRead(middleSensor);
    num3 = digitalRead(rightSensor);
    if ((num2 == 0) && (num1 == 0) && (num3 == 0)) {
      moveStop(); continue;
    } else if ( (num1 == 0) && num3 == 1) { //go to right
      turnLeft();
      while (1) {
        num2 = digitalRead(middleSensor);
        if (num2) {
          turnLeft(); continue;
        } else
          break;
      }
    } else if ((num3 == 0) && (num1 == 1)) { // go to left
      turnRight();
      while (1) {
        num2 = digitalRead(middleSensor);
        if (num2) {
          turnRight(); continue;
        } else
          break;
      }
    } else
    {
      moveForward();
    }
  }
}
void SendMessage(String data) {
  Serial.println(data);
}
int readPing() {
  delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS / US_ROUNDTRIP_CM;
  return cm;
}

