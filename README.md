# RPIPicoWeatherStation
Raspberry Pi Pico W Weather Station

This project is a Weather Station powered by a Raspberry Pi Pico W. It is based on a Wind and Rain Sensors kit from [SparkFun](https://learn.sparkfun.com/tutorials/weather-meter-hookup-guide/all). Then adding additional sensors.

## Status
Project is still evolving and I am documenting the progress on my YouTube channel [drjonea](https://youtube.com/@drjonea).

Current status:
+ PCB First version is complete. This isn't perfect as some of the extension are not working as intended byt basic functions work
+ Code - Basic code to read and publish data to a webservice exist but is evolving
+ Server side code will be in a seperate repo and is work in progress right now


## PCB
The PCB folder contains a KiCAD model of schematic and board

Major components:
+ Raspberry Pi Pico W
+ Wind and Rain Sensor Kit [SparkFun](https://learn.sparkfun.com/tutorials/weather-meter-hookup-guide/all)
+ ChronoDot v2 DS3231 RTC
+ AHT10 Temperature and Humidity Sensor
+ SEN0500 [DFRobot](https://wiki.dfrobot.com/SKU_SEN0500_Fermion_Multifunctional_Environmental_Sensor) Composite sensor

## Tests
I use CPPUTest to build test framework. Currently these test are mainly about isolated testing of the component, to make sure each of the components on the board is working.

Tests can be found in the folder *test*. The test_main.cpp controls which tests are run.

## Exp
This contains some early experiments with the hardware and concepts.  Will probably be deleted at some point.

## Code
This is a Pico SDK and FreeRTOS Kernel application. It uses some of my own libraries for  Web Service publication.

## Build
Firmware can be build using the normal build process:
```
mkdir build
cd build
cmake ..
make
```
