# Smart-Robot-Car-Arduino
## UCTRONICS has updated code library.
## For More powerful wifi car library,please refer to https://github.com/UCTRONICS/WIFI_Camera_Smart_Robot_Car

## we have set the baud rate to 115200, If you are using the old version , please change the baud rate to 9600  in the code .

## At present， UCTRONICS has released three versions Robot car. Both of them support graphical programming.Please refer to the below links to know detail information about each robot car.

## K0069： 
### https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/document/K0069.md
## K0070： 
### https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/document/K0070.md
## K0072： 
### https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/document/K0072.md

##  How to upload the robot car's firmware(Taking an example of K0070)

### Download and install the Arduino IDE from https://www.arduino.cc/download_handler.php?f=/arduino-1.8.5-windows.exe
![Alt text](https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/imge/1.jpeg)

### Library installation instructions
1. Download our UCTRONICS_Smart_Robot_Car library from https://github.com/UCTRONICS/Smart-Robot-Car-Arduino.git
 ![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/1_downloadLibrary.gif) 
2. Unzip the downloaded file.
3. (In the Arduino IDE) Sketch -> Include Library -> Add .ZIP Library... -> select the `UCTRONICS_Smart_Robot_Car` subfolder of the unzipped file -> Open
4. Connect your Robot Car to your computer with a USB cable.
5. Tools -> Port -> select the serial port of the Robot Car
6. File -> Examples -> UCTRONICS_Smart_Robot_Car -> example -> [example of your choice] 
7. Click the Upload button
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/3_downloadDemo.gif) 

##  This firmware support Graphical programming(Taking an example of K0070)
### Download the mblock software.
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/4_DownloadMblock.gif) 
### Install the Mblock software
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/5_installMblock.gif) 
### Install the the UCTRONICS_Smart_RobotCar extension
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/6_installUCBlock.gif) 
### Demo operation
![EasyBehavior](https://github.com/UCTRONICS/pic/blob/master/K0070GIF/7_demoOperation.gif) 

##  Module test code:
###  For motor:
- https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/UCTRONICS_Smart_Robot_Car/example/MotorTest/MotorTest.ino
###  For IR:
- https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/UCTRONICS_Smart_Robot_Car/example/NECTest/NECTest.ino
###  For ultrasonic:
- https://github.com/UCTRONICS/Smart-Robot-Car-Arduino/blob/master/UCTRONICS_Smart_Robot_Car/example/ultrasonicTest/ultrasonicTest.ino

