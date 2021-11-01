#include <Arduino.h>
#include <EEPROM.h>

#include <Button2.h>

#define USE_LittleFS

// #include <FS.h>
#include <LittleFS.h>


#include "Morse.h"
#include "Display.h"

using namespace qsoTrainer;

#define Version 1.0b

#define P_DOT    12   // Connects to the dot lever of the paddle
#define P_DASH   13   // Connects to the dash lever of the paddle
#define P_WPM_UP  14
#define P_WPM_DOWN  2
#define P_WPM_FARNS 0
#define P_AUDIO  15   // Audio output
#define P_CW A0

//const int minWpm = 6;
//const int maxWpm = 50;


int actualWpm; // = 20;  5 - 50
int actualFarnsWpm; // = 10; 5-10

int actualWpmPrev; // = 20;  5 - 50
int actualFarnsWpmPrev; // = 10; 5-10

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



Button2 wpm_Up = Button2(P_WPM_UP);
Button2 wpm_Down = Button2(P_WPM_DOWN);
Button2 wpm_Farns = Button2(P_WPM_FARNS);


Morse morse(P_AUDIO,P_CW);


// Initializing the Arduino
void setup() {
  Serial.begin(115200);
  
  
  
  pinMode(P_DOT, INPUT_PULLUP);
  pinMode(P_DASH, INPUT_PULLUP); 


  wpm_Up.setClickHandler(singleClick);
  wpm_Down.setClickHandler(singleClick);
  wpm_Farns.setClickHandler(singleClick);
  wpm_Farns.setDoubleClickHandler(doubleClick);
  

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
  
  wpm_Up.loop();
  wpm_Down.loop();
  wpm_Farns.loop();


  

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
  //actualWpm = 20;
  //actualFarnsWpm = 10;
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
    mode = 1;
    qsoDisplay::resetSpeed();  
    updateElementLength();
    if (actualWpmPrev != actualWpm || actualFarnsWpmPrev != actualFarnsWpm) {
      int i = writeToEeprom(actualWpm, actualFarnsWpm);
    return;
    } 
  }

  qsoDisplay::setSpeed();
  qsoDisplay::printWpm(actualWpm);
  qsoDisplay::printFarnsWpm(actualFarnsWpm);
  actualWpmPrev = actualWpm; // = 20;  5 - 50
  actualFarnsWpmPrev = actualFarnsWpm; // = 10; 5-10
  mode = 2;
  
}

void singleClick(Button2& btn) {
  Serial.println("click:  ");
  if (mode == 2) { 
    if (btn == wpm_Up) {
      ++actualWpm;
      qsoDisplay::printWpm(actualWpm);
      Serial.print("up ");
    }
    if (btn == wpm_Down) {
      --actualWpm;
      qsoDisplay::printWpm(actualWpm);      
      Serial.print("down ");
    } 

    if (btn == wpm_Farns) {
      actualFarnsWpm = (actualFarnsWpm <=9) ? actualFarnsWpm+=1 :  0;   
      if  (actualFarnsWpm < 5 and actualFarnsWpm != 0) {
        actualFarnsWpm = 5;
      }
      qsoDisplay::printFarnsWpm(actualFarnsWpm);    
      Serial.print("farns "); 
    }
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
