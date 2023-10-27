
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "Display.h"
#include "oled.h"


namespace qsoDisplay {
  

OLED oled = OLED(4,5,16);
String line;

int port = 8088;
WiFiServer TelnetServer(port);
WiFiClient Client;;
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,22);
IPAddress subnet(255,255,255,0);    
bool firstrun = false;

void initDisplay()
{
  oled.begin(); // Instead of Serial.begin(9600)
  oled.clear();
  oled.setTTYMode(true); // This set the TTY mode
  line = "";

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("qsoTrainer") ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  TelnetServer.begin();
  Serial.print("Starting telnet server on port " + (String)port);

} 

void addString(String txt)
{
    if (txt == "") {
      txt = line;
      line = "";
    } else {
      txt += " ";
    }
    int i = txt.length();
    int j = line.length();
    
    if (21-j >= i){
        oled.print(txt);
        line += txt;
    } else {
       line = txt;
       oled.println();   
       oled.print(line);
      
    }
  printTelnet("->"+txt);
}


void setFont(bool bold)
{
  if (bold) {}
}


void setSpeed() {
  oled.setTTYMode(false);
  oled.clear();
  oled.draw_string(30,0,"Settings");
  oled.draw_string(10,10,"WPM: ");
  oled.draw_string(10,20,"FarnsWPM: ");
  oled.display();
}
void printWpm(int value){
  oled.draw_string(70,10,"~~",OLED::NORMAL_SIZE , OLED::BLACK); 
  //draw_character(uint_fast8_t x, uint_fast8_t y, char c, tSize scaling=NORMAL_SIZE, tColor color=WHITE);
  oled.display();
  oled.printf(70,10,"%02d",value);
  oled.display();
}

void printFarnsWpm(int value){
  oled.draw_string(70,20,"~~~",OLED::NORMAL_SIZE , OLED::BLACK); 
  //draw_character(uint_fast8_t x, uint_fast8_t y, char c, tSize scaling=NORMAL_SIZE, tColor color=WHITE);
  oled.display();
  oled.printf(70,20,"%02d",value);
  oled.display();
}

void resetSpeed() {
  oled.setCursor(0,0);
  oled.setTTYMode(true);
  delay(1000);
  oled.clear();
  addString("");
  oled.display();
}

void handleTelnet(){

  

  if (TelnetServer.hasClient()){
    
       
  

  	// client is connected
    if (!Client || !Client.connected()){
      if(Client) Client.stop();          // client disconnected
      Client = TelnetServer.available(); // ready for new client
    } else {
      TelnetServer.available().stop();  // have client, block new conections
     
    
     }
    }
  


  if (Client && Client.connected() && Client.available()){
     
    // client input processing
    while(Client.available())
      Serial.write(Client.read()); // pass through
      // do other stuff with client input here
  } 



}

void printTelnet(String txt) {
  
  if (Client.connected() == 1) {
    Client.println(txt);
  }
  
}


}

