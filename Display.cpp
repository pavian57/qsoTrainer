
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

void intro(int wpm, int fans, int sota, int polari, int iambic, int freq){
  oled.clear();
  oled.display();
  oled.setCursor(0,0);
  oled.printf("Wpm=%d, Farnsworth=%d\n", wpm,fans);  
  oled.printf("Sota=%d, Iambicb=%d\n", sota, iambic);
  oled.printf("Paddle Polarity=%d\n", polari);  
  oled.display();  
  delay(5000);
  oled.clear();
 // oled.display();
  

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


void prepareMenu(){
  oled.clear();
  oled.display();
}



void printMenu(int page, int val1,int val2, int val3){
  String menu1 = "WPM";
  String menu2 = "Farnsworth";
  String menu3 = "SOTA Mode";
  String menu4 = "Paddle Polarity";
  String menu5 = "IAMBIC B";
  String menu6 = "Frequency";

  if (page == 1) {
    oled.printf(10,1,"%s",menu1.c_str());      
    oled.printf(108,1,"%02d",val1);        
    oled.printf(10,10,"%s",menu2.c_str());      
    oled.printf(108,10,"%02d",val2);        
    oled.printf(10,19,"%s",menu3.c_str());      
    oled.printf(108,19,"%2d",val3);        
  } else if (page == 2) {
    oled.printf(10,1,"%s",menu4.c_str());      
    oled.printf(108,1,"%2d",val1);        
    oled.printf(10,10,"%s",menu5.c_str());      
    oled.printf(108,10,"%2d",val2);        
    oled.printf(10,19,"%s",menu6.c_str());      
    oled.printf(108,19,"%3d",val3);       
  }   
  oled.display();
}

void updateMenu(int page, int menupos,int lastmenupos){
  
  if (page == 1) {
    if (lastmenupos == 1) { 
      oled.draw_string(0,1,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
    } else if (lastmenupos == 2) { 
      oled.draw_string(0,10,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
    } else if (lastmenupos == 3) { 
      oled.draw_string(0,19,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
    } 
  } else if (page == 2) {      
    if (lastmenupos == 4) {       
      oled.draw_string(0,1,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
    } else if (lastmenupos == 5) { 
      oled.draw_string(0,10,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
    } else if (lastmenupos == 6) { 
      oled.draw_string(0,19,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
    }
  }
  
  if (page == 1) {      
    if (menupos == 1) { 
      oled.printf(0,1,">");
    } else if (menupos == 2) { 
      oled.printf(0,10,">");
    } else if (menupos == 3) { 
      oled.printf(0,19,">");
    }
  } else if (page == 2) {      
    if (menupos == 4) { 
      oled.printf(0,1,">");
    } else if (menupos == 5) { 
      oled.printf(0,10,">");
    } else if (menupos == 6) { 
      oled.printf(0,19,">");
    }
  }
  oled.display();
}



void clearMenu(){
  oled.clear();
  oled.display();
  oled.print(line);
}

void updateValues(int position, int val){
  oled.draw_string(108,position,"~~~",OLED::NORMAL_SIZE , OLED::BLACK); 
  oled.printf(108,position,"%02d",val);  
  oled.display();
}

void updateValues2d(int position, int val){
  oled.draw_string(108,position,"~~~",OLED::NORMAL_SIZE , OLED::BLACK); 
  oled.printf(108,position,"%2d",val);  
  oled.display();
}

void updateValues3d(int position, int val){
  oled.draw_string(108,position,"~~~",OLED::NORMAL_SIZE , OLED::BLACK); 
  oled.printf(108,position,"%3d",val);  
  oled.display();
}




void setMenuPointertoValues(int pos){
  switch(pos){
         case 1: 
            oled.draw_string(0,1,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
            oled.draw_string(100,1,">",OLED::NORMAL_SIZE , OLED::WHITE); 
            break;
          case 2:
            oled.draw_string(0,10,"~",OLED::NORMAL_SIZE , OLED::BLACK);
            oled.draw_string(100,10,">",OLED::NORMAL_SIZE , OLED::WHITE); 
            break;
          case 3:
            oled.draw_string(0,19,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
            oled.draw_string(100,19,">",OLED::NORMAL_SIZE , OLED::WHITE); 
            break;
          case 4: 
            oled.draw_string(0,1,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
            oled.draw_string(100,1,">",OLED::NORMAL_SIZE , OLED::WHITE); 
            break;
          case 5:
            oled.draw_string(0,10,"~",OLED::NORMAL_SIZE , OLED::BLACK);
            oled.draw_string(100,10,">",OLED::NORMAL_SIZE , OLED::WHITE); 
            break;
          case 6:
            oled.draw_string(0,19,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
            oled.draw_string(100,19,">",OLED::NORMAL_SIZE , OLED::WHITE); 
            break;
        default: break;                     
     }
     oled.display();
}

void resetMenuPointertoValues(int pos){
  switch(pos){
      case 1:
        oled.draw_string(100,1,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
        oled.draw_string(0,1,">",OLED::NORMAL_SIZE , OLED::WHITE);    
        break;
      case 2:
        oled.draw_string(100,10,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
        oled.draw_string(0,10,">",OLED::NORMAL_SIZE , OLED::WHITE);    
        break;
      case 3:
        oled.draw_string(100,19,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
        oled.draw_string(0,19,">",OLED::NORMAL_SIZE , OLED::WHITE);    
        break;
      case 4:
        oled.draw_string(100,1,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
        oled.draw_string(0,1,">",OLED::NORMAL_SIZE , OLED::WHITE);    
        break;
      case 5:
        oled.draw_string(100,10,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
        oled.draw_string(0,10,">",OLED::NORMAL_SIZE , OLED::WHITE);    
        break;
      case 6:
        oled.draw_string(100,19,"~",OLED::NORMAL_SIZE , OLED::BLACK); 
        oled.draw_string(0,19,">",OLED::NORMAL_SIZE , OLED::WHITE);    
        break;
      default: break;  
  }
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

