# Smart-Robot-Car-Arduino
## UCTRONICS has updated code library.
## For More powerful wifi car library,please refer to https://github.com/UCTRONICS/WIFI_Camera_Smart_Robot_Car

## we have set the baud rate to 115200, If you are using the old version , please change the baud rate to 9600  in the code .

## At present， UCTRONICS has released three versions Robot car. Both of them support graphical programming.Please refer to the below links to know detail information about each robot car.

### K0069： 
#### https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/document/K0069.md
### K0070： 
#### https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/document/K0070.md
### K0072： 
#### https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/document/K0072.md



# How to upload the robot car's firmware(Taking an example of K0070)

- Download and install the Arduino IDE from https://www.arduino.cc/download_handler.php?f=/arduino-1.8.5-windows.exe

![Alt text](https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/imge/1.jpeg)

- Download our UCTRONICS_Smart_Robot_Car library from https://github.com/UCTRONICS/Smart-Robot-Car-Arduino.git

 ![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/1_downloadLibrary.gif) 

- Unzip the Smart-Robot-Car-Arduino and copy the UCTRONICS_Smart_Robot_Car library to ..\Arduino\libraries path

![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/2_copyLibrary.gif) 

- Restart your Arduino IDE and connect your Robot Car. Choose Tools -> port -yout serial port number
- Choose the File -> Examples -> UCTRONICS_Smart_Robot_Car -> example ->[example you choeesed] 
- Then uploading the demo to your Robot Car.
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/3_downloadDemo.gif) 

# This firmware support Graphical programming(Taking an example of K0070)
- Download the mblock software.
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/4_DownloadMblock.gif) 
- Install the Mblock software
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/5_installMblock.gif) 
- Install the the UCTRONICS_Smart_RobotCar extension
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/6_installUCBlock.gif) 
- Demo operation
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/7_demoOperation.gif) 

# Module test code:
## For motor:
- https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/UCTRONICS_Smart_Robot_Car/example/MotorTest/MotorTest.ino
## For IR:
- https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/UCTRONICS_Smart_Robot_Car/example/NECTest/NECTest.ino
## For ultrasonic:
- https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/UCTRONICS_Smart_Robot_Car/example/ultrasonicTest/ultrasonicTest.ino

# How to use UCTRONICS_Car_Controller software.

- Step1: Download our app form https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/raw/master/BlueTooth%20Tool/RobotCar_Controller.apk

![Alt text](https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/imge/8.jpeg)

- Step2: Open the RobotCar_Controller.apk and choose Connected device

 ![Alt text](https://github.com/UCTRONICS/pic/blob/master/bt1.png)
 
- Step3: Choose CAR CONTROL MODE

![Alt text](https://github.com/UCTRONICS/pic/blob/master/bt2.png)

- Step4: Choose RESEARCH .If you are the first conect it skip this step

![Alt text](https://github.com/UCTRONICS/pic/blob/master/bt3.png)

- Step5: choose HC-05, if you are the first connect it, you should input the password"1234"/"0000"

![Alt text](https://github.com/UCTRONICS/pic/blob/master/bt4.png)

- Step6: Control your car using the keyboard.

![Alt text](https://github.com/UCTRONICS/pic/blob/master/bt5.png)
