
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

#define P_DOT    00   // Connects to the dot lever of the paddle
#define P_DASH   13   // Connects to the dash lever of the paddle

#define P_UP  12  //sda
#define P_DOWN  14  //scl
#define P_BUTTON 02  //d3

#define P_AUDIO    15 // Audio output

#define CLICKS_PER_STEP   4   // this number depends on your rotary encoder 

//const int minWpm = 6;
//const int maxWpm = 50;


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
  Serial.println("Setup Speed");

  EEPROM.begin(512);
  int addr=0;
  addr += EEPROM.get(addr, actualWpm);
  addr += EEPROM.get(addr, actualFarnsWpm);
  EEPROM.commit();
  EEPROM.end();
  /*Serial.print("Setting WPM to ");
  Serial.println(actualWpm);
  Serial.print("Setting Farnsworth to ");
  Serial.println(actualFarnsWpm);
  Serial.println("EEPROM"); */
  
  if (actualWpm < 5 || actualWpm > 61) {
      Serial.println("Save!");
      actualWpm = 20;
      actualFarnsWpm = 10;
      Serial.print("Setting WPM to ");
    Serial.println(actualWpm);
    Serial.print("Setting Farnsworth to ");
    Serial.println(actualFarnsWpm);
    Serial.println("NEU");
    EEPROM.begin(512);
    addr=0;
    addr += EEPROM.put(addr, actualWpm);
    addr += EEPROM.put(addr, actualFarnsWpm);
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
    ditState = digitalRead(P_DOT);
    dahState = digitalRead(P_DASH); 
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
    elementLengthMsFarns = 1200 / actualFarnsWpm;
    Serial.print("elementLengthMsFarns: ");
    morse.farnsLength = elementLengthMsFarns;  
    Serial.println(elementLengthMsFarns);
  }

  morse.dotLength = elementLengthMs;
  morse.farnsLength = elementLengthMsFarns;
  morse.useFansw = true;
  
}


int writeToEeprom(int wpm,int fwpm){
  Serial.println("Save!");
  
  EEPROM.begin(512);
  int addr=0;
  addr += EEPROM.put(addr, actualWpm);
  addr += EEPROM.put(addr, actualFarnsWpm);
  EEPROM.end();

  return 1;
}

int readFromEeprom(int wpm,int fwpm){
  return 1;
}


void doubleClick(Button2& btn) {
  Serial.print("doubleclick:  ");
  if (mode == 2) { 
    return;
  }
  if (mode == 3) {     
    qsoDisplay::resetSpeed();      
    updateElementLength();

    if (actualWpmPrev != actualWpm || actualFarnsWpmPrev != actualFarnsWpm) {
      int i = writeToEeprom(actualWpm, actualFarnsWpm);
    } 
    mode = 1;
    return;
  }
    mode = 3;
    qsoDisplay::setSpeed();
    rotary.setUpperBound(11);
    rotary.setLowerBound(-1);
    rotary.resetPosition(actualFarnsWpm,false);
    qsoDisplay::printWpm(actualWpm);
    qsoDisplay::printFarnsWpm(actualFarnsWpm);
  }

void singleClick(Button2& btn) {
  Serial.println("click:  ");
  if (mode == 3) { 
    return;
  }
  if (mode == 2) { 
    
    qsoDisplay::resetSpeed();  
    updateElementLength();
    if (actualWpmPrev != actualWpm) {
      int i = writeToEeprom(actualWpm, actualFarnsWpm);
    } 
    mode = 1;       
    return;
  }
  mode = 2;
  qsoDisplay::setSpeed();
  rotary.setUpperBound(51);
  rotary.setLowerBound(4);
  rotary.resetPosition(actualWpm,false); 
  qsoDisplay::printWpm(actualWpm);
  qsoDisplay::printFarnsWpm(actualFarnsWpm);
}

void rotate(ESPRotary& r) {
   if (mode == 2) {    
    actualWpm = r.getPosition();
    Serial.println(actualWpm);
    qsoDisplay::printWpm(actualWpm);  
  } else if (mode == 3) {
    actualFarnsWpm = r.getPosition();
    Serial.println(actualFarnsWpm);
    qsoDisplay::printFarnsWpm(actualFarnsWpm);  
  }
}



//    actualFarnsWpm = (actualFarnsWpm <=6) ? actualFarnsWpm+=1 :  0;
//    Serial.println(actualFarnsWpm);
//    qsoDisplay::printFarnsWpm(actualFarnsWpm);
  


/*
  wpm_Up.loop();
  wpm_Down.loop();
  wpm_Farns.loop();
  */
