//This demo is used for testing the IR module.
//When infrared signal is received, , 
//the infrared value is printed to the serial port
/*******hardware connection********** 

 * IR       UNO
 * VCC      VCC
 * Signal    2
 * GND      GND
 *****************************************/
#include "UCNEC.h"
int32_t temp = 0;
UCNEC myIR(2);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myIR.begin();
}
void loop() {
  // put your main code here, to run repeatedly:
   while (myIR.available())
    {
      temp =  myIR.read();
      Serial.println(temp, HEX);
    }
 }
