# Simple cw qsoTrainer by hb9fvk (ruedi) for Arduino 

This is a small program for esp8266 Arduino that creates a cw SOTA qsoTrainer, either as a Chaser or Activator, depending what you give with your key.

https://www.aliexpress.com/item/4001018066912.html?spm=a2g0o.cart.0.0.532b3c00ry3sqO&mp=1

Entering `**sksk**` goes back to the first step. 

As an activator, you start by asking `**qrl?**`, after that you may gon on with cq sota. You are the activator.  

You			: cq sota de xy2abc xy/jo023 k  
qsoTrainer	: xy3cba  
You			: xy3cba ur 559 559 bk  
qsoTrainer	: tu 559 559 ref xy/ja134 bk  
You			: rr 73 ee  
qsoTrainer	: xy2ghj



After a QRZ, the next Call is coming.  


if you give a `**kaka**`, the qsoTrainer will act as an Activator, you are the chaser.  

qsoTrainer	: qrl? cq sota de xy3cba xy/ja134 k  
You			: xy2abc  
qsoTrainer	: xy2abc ur 589 589 bk  
You			: tu 5nn 5nn ref xy/ja134 bk  
qsoTrainer	: rr 73 ee  
You			: xy2ghj



## Requirements
It runs on any compatible ESP8266 with a small Oled dispay 0.96".  preferable Wifikit from Heltec. 

I took names.csv from https://hb9sota.ch/names_hb9bin/names.csv, cleaned up a little bit and created a Index on linenumbers for easy access. I did the same to summitslist.csv, that can be downloaded here https://www.sotadata.org.uk/summitslist.csv.
There is not much space for a complete summitlist, i have stripped down to to summit only. As this was'nt enough, i purged all not european countries. 
If there are no files, random call and summit will be made by the code.

I made files for all continents.


**Software**
- Arduino and VS Code  
- Arduino IDE
- LittleFS

**Hardware**

- ESP8266 Microcontroller , i used Wifikit 8 from Heltec.  
- Oled Display 128x32 I2C  
- 3 Pushbutton  
- 3.5mm Jack 3pol female for keyer
- 3.5mm Jack 3pol female for headset


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
3. connect keyer and or beeper or headset and start exploring qsoTrainer


## Future Improvements


## Contact and Community

73 de hb9fvk (ruedi)

