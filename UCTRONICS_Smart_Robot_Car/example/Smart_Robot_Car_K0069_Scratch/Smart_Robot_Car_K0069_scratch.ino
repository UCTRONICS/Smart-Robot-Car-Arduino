// UCTRONICS Smart Robot Car demo (C)2018 uctronics
// Web: http://www.uctronics.com
// This demo support Graphical programming
// Step1: Upload the Smart_Robot_Car_K0069_Scratch firmware
// Step2: Set bool isSmartMode  = false;
// Step3: Download the mblock software from http://www.mblock.cc/software/mblock/mblock3/
// Step4: Install The UCTRONICS_Smart_Robot_Car extension
// Step5: Enjoy your Graphical programming.
#include <UCMotor.h>
#include <Servo.h>
typedef struct MeModule
{
  int device;
  int port;
  int slot;
  int pin;
  int index;
  float values[3];
} MeModule;
union {
  byte byteVal[4];
  float floatVal;
  long longVal;
} val;

union {
  byte byteVal[8];
  double doubleVal;
} valDouble;

union {
  byte byteVal[2];
  short shortVal;
} valShort;


String mVersion = "0d.01.105";
boolean isAvailable = false;
boolean isBluetooth = false;

int len = 52;
char buffer[52];
char bufferBt[52];
byte index = 0;
byte dataLen;
boolean isStart = true;
char serialRead;
// define the device ID
#define ROBOTCAR 54
#define ULTRASONIC_SENSOR 55
#define SERVO 56

#define GET 1
#define RUN 2
#define RESET 4
#define START 5
unsigned char prevc = 0;
double lastTime = 0.0;
double currentTime = 0.0;
uint8_t keyPressed = 0;
uint8_t command_index = 0;

bool isSmartMode  = true;
unsigned int S;
unsigned int Sleft;
unsigned int Sright;

#define TURN_DIST 40

#define TRIG_PIN A2
#define ECHO_PIN A3
#define SERVO_PIN 10

UC_DCMotor leftMotor1(3, MOTOR34_64KHZ);
UC_DCMotor rightMotor1(4, MOTOR34_64KHZ);
Servo neckControllerServoMotor;
void setup() {
  pinMode(ECHO_PIN, INPUT); //Set the connection pin output mode Echo pin
  pinMode(TRIG_PIN, OUTPUT);//Set the connection pin output mode trog pin
  neckControllerServoMotor.attach(SERVO_PIN);
  neckControllerServoMotor.write(90);
  delay(100);
  Serial.begin(115200);
  Serial.print("Version: ");
  Serial.println(mVersion);
}
void loop() {
  currentTime = millis() / 1000.0 - lastTime;
  readSerial();
  if (isAvailable) {
    unsigned char c = serialRead & 0xff;
    if (c == 0x55 && isStart == false) {
      if (prevc == 0xff) {
        index = 1;
        isStart = true;
      }
    } else {
      prevc = c;
      if (isStart) {
        if (index == 2) {
          dataLen = c;
        } else if (index > 2) {
          dataLen--;
        }
        writeBuffer(index, c);
      }
    }
    index++;
    if (index > 51) {
      index = 0;
      isStart = false;
    }
    if (isStart && dataLen == 0 && index > 3) {
      isStart = false;
      parseData();
      index = 0;
    }else if (!isStart) {
      if (serialRead >= 1 && serialRead <= 5) { //0x01->forward  0x02->backward  0x03->left  0x04-> right  0x05->stop
        leftMotor1.run(serialRead); rightMotor1.run(serialRead);
        leftMotor1.setSpeed(200);
        rightMotor1.setSpeed(200);
      }
      if (serialRead == 0x06) { //automatic obstacle avoidance
        neckControllerServoMotor.attach(SERVO_PIN);
        neckControllerServoMotor.write(90);
        isSmartMode = true;
      }
    }
  }
  if (isSmartMode) {
    neckControllerServoMotor.write(90);
    S = readPing();
    if (S <= TURN_DIST ) {
      turn();
    } else if (S > TURN_DIST) {
      leftMotor1.run(1); rightMotor1.run(1);//1-> forward
      leftMotor1.setSpeed(200); rightMotor1.setSpeed(200);
    }
  }

}
unsigned char readBuffer(int index) {
  return isBluetooth ? bufferBt[index] : buffer[index];
}
void writeBuffer(int index, unsigned char c) {
  if (isBluetooth) {
    bufferBt[index] = c;
  } else {
    buffer[index] = c;
  }
}
void writeHead() {
  writeSerial(0xff);
  writeSerial(0x55);
}
void writeEnd() {
  Serial.println();
#if defined(__AVR_ATmega32U4__)
  Serial1.println();
#endif
}
void writeSerial(unsigned char c) {
  Serial.write(c);
#if defined(__AVR_ATmega32U4__)
  Serial1.write(c);
#endif
}
void readSerial() {
  isAvailable = false;
  if (Serial.available() > 0) {
    isAvailable = true;
    isBluetooth = false;
    serialRead = Serial.read();
  }
}
/*
  ff 55 len idx action device port  slot  data a
  0  1  2   3   4      5      6     7     8
*/
void parseData() {
  isStart = false;
  int idx = readBuffer(3);
  command_index = (uint8_t)idx;
  int action = readBuffer(4);
  int device = readBuffer(5);
  switch (action) {
    case GET: {
        writeHead();
        writeSerial(idx);
        readSensor(device);
        writeEnd();
      }
      break;
    case RUN: {
        runModule(device);
        callOK();
      }
      break;
    case RESET: {
        //reset
        leftMotor1.run(5);leftMotor1.setSpeed(0);
        rightMotor1.run(5);rightMotor1.setSpeed(0);     
        neckControllerServoMotor.write(90);
        delay(100);
      }
      break;
    case START: {
        //start
        callOK();
      }
      break;
  }
}
void callOK() {
  writeSerial(0xff);
  writeSerial(0x55);
  writeEnd();
}
void sendByte(char c) {
  writeSerial(1);
  writeSerial(c);
}
void sendString(String s) {
  int l = s.length();
  writeSerial(4);
  writeSerial(l);
  for (int i = 0; i < l; i++) {
    writeSerial(s.charAt(i));
  }
}
void sendFloat(float value) {
  writeSerial(0x2);
  val.floatVal = value;
  writeSerial(val.byteVal[0]);
  writeSerial(val.byteVal[1]);
  writeSerial(val.byteVal[2]);
  writeSerial(val.byteVal[3]);
}
void sendShort(double value) {
  writeSerial(3);
  valShort.shortVal = value;
  writeSerial(valShort.byteVal[0]);
  writeSerial(valShort.byteVal[1]);
}
void sendDouble(double value) {
  writeSerial(2);
  valDouble.doubleVal = value;
  writeSerial(valDouble.byteVal[0]);
  writeSerial(valDouble.byteVal[1]);
  writeSerial(valDouble.byteVal[2]);
  writeSerial(valDouble.byteVal[3]);
}
short readShort(int idx) {
  valShort.byteVal[0] = readBuffer(idx);
  valShort.byteVal[1] = readBuffer(idx + 1);
  return valShort.shortVal;
}
float readFloat(int idx) {
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx + 1);
  val.byteVal[2] = readBuffer(idx + 2);
  val.byteVal[3] = readBuffer(idx + 3);
  return val.floatVal;
}
long readLong(int idx) {
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx + 1);
  val.byteVal[2] = readBuffer(idx + 2);
  val.byteVal[3] = readBuffer(idx + 3);
  return val.longVal;
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
  cm = duration / 29 / 2;;
  return cm ;
}

void turn() {
  leftMotor1.run(5); rightMotor1.run(5);//5-> stop
  leftMotor1.setSpeed(0); rightMotor1.setSpeed(0);
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
    leftMotor1.run(2); rightMotor1.run(2);//2-> backward
    leftMotor1.setSpeed(200); rightMotor1.setSpeed(200);
    delay(500);
    int x = random(1);
    if (x = 0) {
      leftMotor1.run(4); rightMotor1.run(4);//4-> right
      leftMotor1.setSpeed(200); rightMotor1.setSpeed(0);
    }
    else {
      leftMotor1.run(4); rightMotor1.run(4);//3-> left
      leftMotor1.setSpeed(0); rightMotor1.setSpeed(0);
    }
  } else {
    if (Sleft >= Sright) {
      leftMotor1.run(4); rightMotor1.run(4);//3-> left
      leftMotor1.setSpeed(0); rightMotor1.setSpeed(0);
    } else {
      leftMotor1.run(4); rightMotor1.run(4);//4-> right
      leftMotor1.setSpeed(200); rightMotor1.setSpeed(0);
    }
  }
}
void runModule(int device) {
  //0xff 0x55 0x6 0x0 0x1 0xa 0x9 0x0 0x0 0xa
  int port = readBuffer(6);
  int pin = port;
  switch (device) {
    case SERVO: {
        int angle = readBuffer(6);
        if (angle >= 0 && angle <= 180)
        {
          neckControllerServoMotor.attach(SERVO_PIN);
          neckControllerServoMotor.write(angle);
        }
      }
      break;
    case ROBOTCAR: {
        int directionMode = readBuffer(6);
        int motorSpeed = readBuffer(8);
        leftMotor1.run(directionMode);
        rightMotor1.run(directionMode);
        leftMotor1.setSpeed(motorSpeed);
        rightMotor1.setSpeed(motorSpeed);
      }
      break;
  }
}

void readSensor(int device) {
  /**************************************************
      ff 55 len idx action device port slot data a
       0  1  2   3   4      5      6    7    8
  ***************************************************/
  float value = 0.0;
  int port, slot, pin;
  port = readBuffer(6);
  pin = port;
  switch (device) {
    case ULTRASONIC_SENSOR: {
        value = readPing();
        sendFloat(value);
      }
      break;
  }
}
