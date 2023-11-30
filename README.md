# Simple cw qsoTrainer by hb9fvk (ruedi) for Arduino 

This is a small program for esp8266 Arduino that creates a cw SOTA qsoTrainer, either as a Chaser or Activator, depending what you give with your paddle.


Entering `**sksk**` goes back to the first step. 

1. As an activator, you start by asking `**qrl?**`, after that you may gon on with cq sota. You are the activator.  

You			: cq sota de xy2abc xy/jo023 k  
qsoTrainer	: xy3cba  
You			: xy3cba gd 559 559 bk  
qsoTrainer	: rr gd 559 559 ref xy/ja134 bk  
You			: 73 tu ee  
qsoTrainer	: xy2ghj

2. If you give a `**kaka**`, the qsoTrainer will act as an Activator, you are the chaser.  

qsoTrainer	: qrl? cq sota de xy3cba xy/ja134 k  
You			: xy2abc  
qsoTrainer	: xy2abc gd 589 589 bk  
You			: rr gd 5nn 5nn ref xy/ja134 73 tu bk  
qsoTrainer	: 73 tu ee  
You			: xy2ghj


3. If you give a `**asas**`, the qsoTrainer will echo everything you give with the paddle.  

give `**asas**` again to leave the trainer mode, the reset button does the same.

4. If you give a `**btbt**`, the qsoTrainer will echo Callsign you give back with the paddle.  

give `**btbt**` again to leave the Callsign mode, the reset button does the same.

5. If you give a `**veve**`, the qsoTrainer will echo CW Abbreviations you give back with the paddle.  

give `**veve**` again to leave the Abbreviation  mode, the reset button does the same.

6. to change WPM, Farnsworth, SOTA Mode and Paddle Polarity, doubclick the Black Knob (rotaryeoncoder), to enter Menu, to change a value click once.

## Requirements
It runs on any compatible ESP8266 with a small Oled dispay 0.96".  preferable Wifikit from Heltec. 

I took names.csv from https://hb9sota.ch/names_hb9bin/names.csv, cleaned up a little bit and created a Index on linenumbers for easy access. I did the same to summitslist.csv, that can be downloaded here https://www.sotadata.org.uk/summitslist.csv.

There is not much space for a complete summitlist, i have stripped down to to summit only. As this was'nt enough, i purged all not european countries. 

I made files for all continents.
Summitlist for other Continents ar in the Folder Summits, just copy to data Folder.
If there are no files, random call and summit will be made by the code.

**Software**
- Arduino and VS Code  
- Arduino IDE
- LittleFS

**Hardware**

- ESP8266 Microcontroller , i used Wifikit 8 from Heltec with Oled Display 128x32 I2C  
    https://www.aliexpress.com/w/wholesale-heltec-wifi-kit-8.html?spm=a2g0o.home.search.0
- simple Rotaryencoder 
    https://www.aliexpress.com/w/wholesale-360-Rotary-Encoder.html?spm=a2g0o.detail.search.0
- 3.5mm Jack 3pol female for Paddle and Headset
    https://www.aliexpress.com/w/wholesale-3.5mm-socket.html?spm=a2g0o.productlist.search.0
- 10K Potentiometer
    https://www.aliexpress.com/w/wholesale-RV09-Potentiometer.html?spm=a2g0o.detail.search.0
- 100 Ohm Reistor
- Small Loudspeaker
    https://www.aliexpress.com/w/wholesale-Loudspeaker-1W-8ohm.html?spm=a2g0o.productlist.search.0



## Installation

**LITTTLEFS**
Install LittleFS for access of the Memory on the ESP8266.
Follow these Information.
`https://github.com/esp8266/Arduino`

and these instructions.

`https://randomnerdtutorials.com/install-esp8266-nodemcu-littlefs-arduino/`


**Oled**
get the Oled Library from Github  
`https://github.com/durydevelop/arduino-lib-oled`  

Change line 99 in oled.cpp to  
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // ~  space   
makes a space instead a ~.  

**source**

First, you have to fetch the source code from GitHub. 

`git clone https://github.com/pavian57/qsoTrainer`

You can also download a ZIP file from Github

`Code -> Download ZIP`

and unzip it.


### ESP8266
Use the Arduino IDE or VS Code

## Set-up

1. Connect the ESP8266, setup SPIFFS and upload data directory.   
2. Upload Sketch.  
3. connect paddle and headset and start exploring qsoTrainer

## Logging

To see what you are keying, connect a Serial Terminal or use a Tablet and connect to WifiAp = qsoTrainer no Password, us a Telneclient to listen to 192.168.4.22:8088, i use ConnectBot for Android. 
https://play.google.com/store/apps/details?id=org.connectbot&hl=en_US





## Future Improvements


## Contact and Community

73 de hb9fvk (ruedi)

