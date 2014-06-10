# About

This was my final year thesis project.
I used rcswitch-pi for receiving data from cheap 433MHz Tx connected to arduino. The program 'dtr.ino' is for transmission.
I used wiringpi for Serial Communication between OBD port of car and Pi. [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi).
SQLite is for putting data into a database.

#Programs

dtr.ino:
It receives data from a GSM modem and the data is transmitted using 433MHz Transmitter.


RFSniffer.cpp:
It receives the data sent from the arduino and processes the data it received. It gives suitable voice outputs according to data it received. Then it checks for the speed of car  and if it is above threshold it gives warnings and the violation is stored into a database.



## Usage

First you have to install the [wiringpi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) library.
Install SQLite and all its dependencies.
Install Festival Text to Speech for voice output.
Go to the terminal and type make
Then run it as ./RFSniffer


