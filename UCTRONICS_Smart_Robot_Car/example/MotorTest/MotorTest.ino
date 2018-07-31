//This demo is used for testing the Motor module.
#include <UCMotor.h>

UC_DCMotor leftMotor1(3, MOTOR34_64KHZ);
UC_DCMotor rightMotor1(4, MOTOR34_64KHZ);
UC_DCMotor leftMotor2(1, MOTOR34_64KHZ);
UC_DCMotor rightMotor2(2, MOTOR34_64KHZ);
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  //Forward
        leftMotor1.run(0x01); rightMotor1.run(0x01);
        leftMotor2.run(0x01); rightMotor2.run(0x01);
        leftMotor1.setSpeed(200); rightMotor1.setSpeed(200);
        leftMotor2.setSpeed(200); rightMotor2.setSpeed(200);
        delay(2000);
   //Backward
        leftMotor1.run(0x02); rightMotor1.run(0x02);
        leftMotor2.run(0x02); rightMotor2.run(0x02);
        leftMotor1.setSpeed(200); rightMotor1.setSpeed(200);
        leftMotor2.setSpeed(200); rightMotor2.setSpeed(200);
        delay(2000);
    //left
        leftMotor1.run(0x03); rightMotor1.run(0x03);
        leftMotor2.run(0x03); rightMotor2.run(0x03);
        leftMotor1.setSpeed(200); rightMotor1.setSpeed(200);
        leftMotor2.setSpeed(200); rightMotor2.setSpeed(200);
        delay(2000); 
    //Right
        leftMotor1.run(0x04); rightMotor1.run(0x04);
        leftMotor2.run(0x04); rightMotor2.run(0x04);
        leftMotor1.setSpeed(200); rightMotor1.setSpeed(200);
        leftMotor2.setSpeed(200); rightMotor2.setSpeed(200);
        delay(2000);      

}
