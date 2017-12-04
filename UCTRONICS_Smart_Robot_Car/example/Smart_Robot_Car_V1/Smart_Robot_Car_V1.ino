// UCTRONICS Smart_Robot_Car demo (C)2017
//Before using this demo, you shold install UCMotor library
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
#include <UCMotor.h>

#define TRIG_PIN A2
#define ECHO_PIN A3

Servo neckControllerServoMotor;
UC_DCMotor leftMotor(3, MOTOR34_64KHZ);
UC_DCMotor rightMotor(4, MOTOR34_64KHZ);


unsigned int S;
unsigned int Sleft;
unsigned int Sright;

void setup() {
  Serial.begin(9600);
  pinMode(ECHO_PIN, INPUT); //Set the connection pin output mode Echo pin
  pinMode(TRIG_PIN, OUTPUT);//Set the connection pin output mode trog pin
  neckControllerServoMotor.attach(10);
  neckControllerServoMotor.write(90);
  delay(2000);
}
void loop() {
  neckControllerServoMotor.write(90);
  S = readPing();
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

