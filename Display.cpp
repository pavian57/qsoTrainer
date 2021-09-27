#include "Display.h"
#include "oled.h"
#include <Arduino.h>



namespace qsoDisplay {
  

OLED oled = OLED(4,5,16);
String line;

void initDisplay()
{
  oled.begin(); // Instead of Serial.begin(9600)
  oled.clear();
  oled.setTTYMode(true); // This set the TTY mode
  line = "";
} 

void addString(String txt)
{
    txt += " ";
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
  addString(">");
  oled.display();
}


}

