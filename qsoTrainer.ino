
#include <Arduino.h>

#include <EEPROM.h>

// https://github.com/LennartHennigs/
#include <Button2.h>
#include <ESPRotary.h>

#define USE_LittleFS

// #include <FS.h>
#include <LittleFS.h>


#include "Morse.h"
#include "Display.h"

using namespace qsoTrainer;

#define Version 1.0b

#define P_DOT    13   // Connects to the dot lever of the paddle
#define P_DASH   00   // Connects to the dash lever of the paddle

#define P_UP  14  //sda
#define P_DOWN  12  //scl
#define P_BUTTON 02  //d3

#define P_AUDIO    15 // Audio output

#define CLICKS_PER_STEP   4   // this number depends on your rotary encoder 

int actualWpm; // = 20;  5 - 50
int actualFarnsWpm; // = 10; 0-10

int actualWpmPrev; // = 20;  5 - 50
int actualFarnsWpmPrev; // = 10; 0-10



int ditState = 0;        
int dahState = 0;
int LastElement = 0;



int elementLengthMs;
int elementLengthMsFarns;

unsigned long previousMillis = 0 ;
unsigned long currentMillis;


String actSign;
String actWord;
String txtSign;
String cwAbbr;

int menuitem = 1;
int lastMenuItem = 1;
int sotaqso = 0; // no sota
int paddlepolarity = 0; // 0 = . Left, 1 = - Left

int page = 1;

bool inMenu = false;
bool chvalue = false;
bool abbrAvailable = false;




int mode = 1;
int farnsworth = 2;

Button2 button; 
ESPRotary rotary;


Morse morse(P_AUDIO);


// Initializing the Arduino
void setup() {
  Serial.begin(115200);
  
  
  
  pinMode(P_DOT, INPUT_PULLUP);
  pinMode(P_DASH, INPUT_PULLUP); 


  rotary.begin(P_DOWN, P_UP, CLICKS_PER_STEP);
  rotary.setChangedHandler(rotate);

  button.begin(P_BUTTON);
  button.setClickHandler(singleClick);
  button.setDoubleClickHandler(doubleClick);

  actSign = "";
  actWord = "";
  qsoDisplay::initDisplay(); // Init Display
  
  currentMillis = millis();
  qsoDisplay::addString(">");
  Serial.println();
  if (morse.LittleFSActive) {
    Serial.println("LittleFS Active");
  } else {
    Serial.println("Unable to activate LittleFS");
  }
  Serial.println("Setup CW Speed");

  EEPROM.begin(512);
  int addr=0;
  addr += EEPROM.get(addr, actualWpm);
  addr += EEPROM.get(addr, actualFarnsWpm);
  addr += EEPROM.get(addr, sotaqso);
  addr += EEPROM.get(addr, paddlepolarity);
  EEPROM.commit();
  EEPROM.end();
  morse.sotaqso = sotaqso;
  
  if (actualWpm < 5 || actualWpm > 61) {
      Serial.println("Save!");
      actualWpm = 20;
      actualFarnsWpm = 0;
      Serial.print("Setting WPM to ");
    Serial.println(actualWpm);
    Serial.print("Setting Farnsworth to ");
    Serial.println(actualFarnsWpm);

    EEPROM.begin(512);
    addr=0;
    addr += EEPROM.put(addr, actualWpm);
    addr += EEPROM.put(addr, actualFarnsWpm);
    addr += EEPROM.put(addr, sotaqso);
    addr += EEPROM.put(addr, paddlepolarity);
    EEPROM.end();    
    
  };





    updateWpm();
  
  Serial.println("done");
}

// Main routine
void loop()
{

  rotary.loop();
  button.loop();
  

  qsoDisplay::handleTelnet();


  

  if ( mode == 1 ) {
    if (paddlepolarity) {
      dahState = digitalRead(P_DOT);
      ditState = digitalRead(P_DASH); 
    } else {
      ditState = digitalRead(P_DOT);
      dahState  = digitalRead(P_DASH); 
    }
    currentMillis = millis();
 
  if ((ditState == LOW)&(dahState == HIGH)) {       
    LastElement = 1;
    morse.dot();           // ... send a dot at the given elementLengthMs    
    actSign = actSign + ".";
    previousMillis = currentMillis;
  } else if ((dahState == LOW)&(ditState == HIGH)) {  
    LastElement = 2;      
    morse.dash();       // ... send a dash at the given elementLengthMs  
    actSign = actSign + "-";    
    previousMillis = currentMillis;
      
  } else if ((dahState == LOW)&(ditState == LOW)) {  
      if (LastElement == 2) {
        LastElement = 1;
        morse.dot();           // ... send a dot at the given elementLengthMs    
        actSign = actSign + ".";
        previousMillis = currentMillis;
      }  else {
          LastElement = 2;
          morse.dash();  // ... send a dash at the given elementLengthMs  
          actSign = actSign + "-";    
          previousMillis = currentMillis;
        }    
  }


  
  if (currentMillis - previousMillis > elementLengthMs*3){
    if (actSign != "") {       
        Serial.print(" ");
        Serial.print(actSign);
        txtSign = morse.encode(actSign);
        actWord = actWord + txtSign; 
        actSign = "";
        previousMillis = currentMillis;
      }
  }

  if (currentMillis - previousMillis > elementLengthMs*7){   
      if (actWord != "") {
        Serial.println();
        Serial.print("Word: >");
        Serial.print(actWord);
        Serial.println("<");
        morse.tlg = actWord;
        morse.doQso();
        Serial.print("State: "); Serial.print(morse.State);
        Serial.print(" Type: "); Serial.println(morse.Type);  
        //Serial.println(morse.nextStep);
        if (!morse.nextStep){
          qsoDisplay::printTelnet(actWord);
        }       
        
        actWord = "";  

      }
    } 
  }
}


void updateWpm()
{
  Serial.print("Setting WPM to ");
  Serial.println(actualWpm);
  Serial.print("Setting Farnsworth to ");
  Serial.println(actualFarnsWpm);

  rotary.setUpperBound(51);
  rotary.setLowerBound(4);
  rotary.resetPosition(actualWpm,false);


     // Calculate millisecond bit length from new WPM
  updateElementLength();
  
}

/*
 * Calculate dit length in milliseconds from WPM
 */
void updateElementLength()
{
  elementLengthMs = 1200 / actualWpm;
  morse.dotLength = elementLengthMs;
  Serial.print("elementLengthMs: ");
  Serial.println(elementLengthMs);

  if (actualFarnsWpm > 0) {
    elementLengthMsFarns = 1200 / (actualWpm - actualFarnsWpm );
    Serial.print("elementLengthMsFarns: ");
    morse.farnsLength = elementLengthMsFarns;  
    Serial.println(elementLengthMsFarns);
  }

  morse.dotLength = elementLengthMs;
  morse.farnsLength = elementLengthMsFarns;
  morse.useFansw = true;
  
}


int writeToEeprom(){
  Serial.println("Save all!");

  EEPROM.begin(512);
  int addr=0;
  addr += EEPROM.put(addr, actualWpm);
  addr += EEPROM.put(addr, actualFarnsWpm);
  addr += EEPROM.put(addr, sotaqso);
  addr += EEPROM.put(addr, paddlepolarity);
  EEPROM.commit();
  EEPROM.end();

  return 1;
}




void doubleClick(Button2& btn) {
  Serial.println("doubleclick:  ");
  if (mode == 2) { 
    qsoDisplay::clearMenu();    
    updateWpm();
    int i = writeToEeprom();
    mode = 1;
    inMenu = false;
  } else {
    mode = 2;
    inMenu = true;
    menuitem = 1;
    lastMenuItem = 1;  
    qsoDisplay::prepareMenu();
    qsoDisplay::printMenu(page, actualWpm, actualFarnsWpm, sotaqso);    
    rotary.setUpperBound(7);
    rotary.setLowerBound(0);
    rotary.resetPosition(1,false);
  }
 
}

void singleClick(Button2& btn) {

  if (inMenu) {    
    if (chvalue) {            
      rotary.setUpperBound(7);
      rotary.setLowerBound(0);
      rotary.resetPosition(menuitem,false);
      qsoDisplay::restsetMenuPointertoValues(menuitem);
      chvalue = false;
    } else {
      chvalue = true;      
      switch(menuitem){
         case 1: 
            rotary.setUpperBound(51);
            rotary.setLowerBound(4);
            rotary.resetPosition(actualWpm,false);
            qsoDisplay::setMenuPointertoValues(1);
          break;
          case 2:
            rotary.setUpperBound(11);
            rotary.setLowerBound(-1);
            rotary.resetPosition(actualFarnsWpm,false);                        
            qsoDisplay::setMenuPointertoValues(2);
          break;
          case 3:
            qsoDisplay::setMenuPointertoValues(3);            
            break;
          case 4: 
            rotary.setUpperBound(11);
            rotary.setLowerBound(-1);
            rotary.resetPosition(actualFarnsWpm,false);                        
            qsoDisplay::setMenuPointertoValues(4);
          break;
          case 5:            
            qsoDisplay::setMenuPointertoValues(5);
          break;
          case 6:
            qsoDisplay::setMenuPointertoValues(6);            
            break;
        default: break;                     
     }
    }   
  }
  Serial.println("click:  "); 
}

void rotate(ESPRotary& r) {

  if (inMenu && !chvalue) {
    lastMenuItem = menuitem;
    menuitem = rotary.getPosition();   
    if (menuitem >= 4) {   
      if (page != 2) {
        qsoDisplay::prepareMenu();
        qsoDisplay::printMenu(2, actualFarnsWpm, morse.sotaqso, paddlepolarity);            
      }
      page = 2;                  
    } else {      
      if (page != 1) {
        qsoDisplay::prepareMenu();
        qsoDisplay::printMenu(1, actualWpm, actualFarnsWpm, morse.sotaqso);
        
      }
      page = 1;            
    }  
    qsoDisplay::updateMenu(page, menuitem,lastMenuItem);        
  } 

  if (inMenu && chvalue ) {    
    if (menuitem == 1) {           
        actualWpm = rotary.getPosition();
        qsoDisplay::updateValues(1,actualWpm);         
    } else if (menuitem == 2){ 
        actualFarnsWpm = rotary.getPosition();
        qsoDisplay::updateValues(10,actualFarnsWpm); 
    } else if (menuitem == 3) { 
        sotaqso = morse.sotaqso;        
        sotaqso =  (sotaqso == 0) ? 1 : 0;
        qsoDisplay::updateValues2d(19,sotaqso); 
        morse.sotaqso = sotaqso;
        
    } else if (menuitem == 4) {           
        actualFarnsWpm = rotary.getPosition();
        qsoDisplay::updateValues(1,actualFarnsWpm);         
    } else if (menuitem == 5){ 
        sotaqso = morse.sotaqso;        
        sotaqso =  (sotaqso == 0) ? 1 : 0;
        qsoDisplay::updateValues2d(10,sotaqso); 
        morse.sotaqso = sotaqso;
    } else if (menuitem == 6) { 
        paddlepolarity =  (paddlepolarity == 0) ? 1 : 0;        
        qsoDisplay::updateValues2d(19,paddlepolarity);         
    }
  } 

  Serial.println("-----------------------------------------");
  Serial.print("menuitem: ");
  Serial.println(menuitem);

  Serial.print("lastMenuItem: ");
  Serial.println(lastMenuItem);   

 
  Serial.print("page: ");
  Serial.println(page);
}



