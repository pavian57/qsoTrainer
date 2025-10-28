
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

#define DIT_L    0x01 // Dit latch
#define DASH_L   0x02 // Dash latch
#define DIT_PROC 0x04 // Dit is being processed
#define IAMBICA  0x00 // 0: Iambic A, 1: Iambic B
#define IAMBICB  0x10 // 0: Iambic A, 1: Iambic B

enum KSTYPE {IDLE, CHK_DIT, CHK_DASH, KEYED_PREP, KEYED, INTER_ELEMENT };

int actualWpm = 18; // = 20;  5 - 50
int actualFarnsWpm = 5; // = 10; 0-10

int actualWpmPrev; // = 20;  5 - 50
int actualFarnsWpmPrev; // = 10; 0-10

int sotaqso = 0; // no sota
int paddlepolarity = 0; // 0 = . Left, 1 = - Left
int iambicab = 0; // 0 = A, 1 = B
int frequency = 1;
int eepromused = 99;


int ditState = 0;        
int dahState = 0;
int LastElement = 0;



int elementLengthMs;
int elementLengthMsFarns;

unsigned long previousMillis = 0 ;
unsigned long currentMillis;

unsigned char keyerControl, keyerState;
unsigned long interWordTimer = 0;
unsigned long interCharTimer = 0;


String actSign;
String actWord;
String txtSign;
String cwAbbr;

int menuitem = 1;
int lastMenuItem = 1;


int page = 1;

bool inMenu = false;
bool chvalue = false;
bool abbrAvailable = false;




int mode = 1;
int farnsworth = 2;


Button2 button; 
ESPRotary rotary;


Morse morse(P_AUDIO);



void update_PaddleLatch() {
  if (paddlepolarity) {
    if (digitalRead(P_DASH) == LOW) keyerControl |= DIT_L;  // DITS   on TIP ( Stereo Plug )
    if (digitalRead(P_DOT) == LOW) keyerControl |= DASH_L; // DASHES on RING ( Stereo Plug )
  } else {
    if (digitalRead(P_DOT) == LOW) keyerControl |= DIT_L;  // DITS   on TIP ( Stereo Plug )
    if (digitalRead(P_DASH) == LOW) keyerControl |= DASH_L; // DASHES on RING ( Stereo Plug )
  }
}


// Initializing the Arduino
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  interCharTimer = 4294967000;                       // almost the biggest possible unsigned long number :-) - do not output extra spaces!                       
  interWordTimer = 4294967000; 
  
  
  pinMode(P_DOT, INPUT_PULLUP);
  pinMode(P_DASH, INPUT_PULLUP); 

  keyerState   = IDLE;
 
  rotary.begin(P_DOWN, P_UP, CLICKS_PER_STEP);
  rotary.setChangedHandler(rotate);

  button.begin(P_BUTTON);
  button.setClickHandler(singleClick);
  button.setDoubleClickHandler(doubleClick);

  actSign = "";
  actWord = "";
  qsoDisplay::initDisplay(); // Init Display
  
  currentMillis = millis();
  
  Serial.println();
  if (morse.LittleFSActive) {
    Serial.println("LittleFS Active");
  } else {
    Serial.println("Unable to activate LittleFS");
  }

  int addr=0;
  // addr = writeToEeprom();

  
  EEPROM.begin(512);
  EEPROM.get(addr, eepromused);
  addr += sizeof(eepromused);
  Serial.println(eepromused);
  if (eepromused == 99) {
    EEPROM.get(addr, actualWpm);
    addr += sizeof(actualWpm);
    EEPROM.get(addr, actualFarnsWpm);
    addr += sizeof(actualFarnsWpm);
    EEPROM.get(addr, sotaqso);
    addr += sizeof(sotaqso);
    EEPROM.get(addr, paddlepolarity);
    addr += sizeof(paddlepolarity);
    EEPROM.get(addr, iambicab);
    addr += sizeof(iambicab);
    EEPROM.get(addr, frequency);  
  }
  EEPROM.end();
  
  


  morse.sotaqso = sotaqso;
  morse.frequency = frequency;
  
  if (actualWpm < 5 || actualWpm > 61) {
    actualWpm = 20;
    actualFarnsWpm = 0;
  }

  Serial.println();
  Serial.println("Setup CW Speed");
  updateWpm();
  
  Serial.printf("Sota mode %d\n",sotaqso);    
  Serial.printf("Polarity  %d\n",paddlepolarity);
  Serial.printf("Iambic B mode %d\n",iambicab);
  Serial.printf("Frequency  %d\n",frequency);  
  Serial.printf("Frequency  Hz %d\n",morse.frequencys[morse.frequency]);
  
  

  if (iambicab == 0) {
      keyerControl = IAMBICA;
  } else {
      keyerControl = IAMBICB;
  }
  int freq = morse.frequencys[frequency];    
  qsoDisplay::intro(actualWpm,actualFarnsWpm, sotaqso,paddlepolarity,iambicab, freq);  
  qsoDisplay::addString(">");
  Serial.println("done");
}

// Main routine
void loop()
{
  static long ktimer;
  static long ctimer;
  

  rotary.loop();
  button.loop();
  

  qsoDisplay::handleTelnet();


  

  if ( mode == 1 ) {
    
    
    currentMillis = millis();

  switch (keyerState) { 
    
    case IDLE: if (millis() > interCharTimer) {             
             actSign.concat(" ");            
             txtSign = morse.encode(actSign);
             actWord.concat(txtSign);           
             Serial.print(actSign);             
             actSign.clear();             
             interCharTimer = 4294967000;                       // almost the biggest possible unsigned long number :-) - do not output extra spaces!             
          }
          if (millis() > interWordTimer) {
            morse.tlg = actWord;
            Serial.println(actWord);
            if (actWord == "//?") {                             
              int freq = morse.frequencys[frequency];
              qsoDisplay::intro(actualWpm,actualFarnsWpm,morse.sotaqso,paddlepolarity,iambicab, freq);
              qsoDisplay::addString(">");
              actWord.clear();  
            } else {          
              morse.doQso();
            
              Serial.print("State: "); Serial.print(morse.State);
              Serial.print(" Type: "); Serial.println(morse.Type);  
        
              if (!morse.nextStep){
                qsoDisplay::printTelnet(actWord);
              }               
              actWord.clear();  
              interWordTimer = 4294967000;                       // almost the biggest possible unsigned long number :-) - do not output extra spaces!             
            }
          }

              
              if ((digitalRead(P_DOT) == LOW) || (digitalRead(P_DASH) == LOW) || (keyerControl & 0x03)) {
                 update_PaddleLatch(); 
                 keyerState = CHK_DIT; 
               } 
             
               break;
 
    case CHK_DIT: if (keyerControl & DIT_L) { 
                    keyerControl |= DIT_PROC;
                    // Some of PP5VX's fingers are here...
                    // ktimer = ditTime;    // If you don't use Weight Control
                    ktimer = elementLengthMs;     // Dit Time+Weight
                    
                    keyerState = KEYED_PREP;
                    //Serial.print(".");
                    actSign.concat(".");
                  } else keyerState = CHK_DASH;
                  break;
 
    case CHK_DASH: if (keyerControl & DASH_L) { // Some of PP5VX's fingers are here...
                   
                     ktimer = elementLengthMs*3; // Dash Time+Weight                                         
                     //Serial.print("-");
                     actSign.concat("-");
                     keyerState = KEYED_PREP; 
                   } else keyerState = IDLE;
                  break;
 
    case KEYED_PREP: 
                     //tone(ST_Pin, key_tone);                     
                     ktimer += millis();
                     keyerControl &= ~(DIT_L + DASH_L);
                     keyerState = KEYED;
                     morse.cwOn();
                     break;
 
    case KEYED: if (millis() > ktimer) { 
                  morse.cwOff();
                  
                  //noTone(ST_Pin);                                    
                  ktimer = millis()+elementLengthMs;  
                  keyerState = INTER_ELEMENT; 
                  interWordTimer = millis() + 7*elementLengthMs;
                  interCharTimer = millis() + 2*elementLengthMs;
                } else if (keyerControl & IAMBICB) {
                  update_PaddleLatch();
                }                
                break;
 
    case INTER_ELEMENT: update_PaddleLatch();
                  //      interWordTimer = millis() + 2*ditTime;
                        if (millis() > ktimer) { 
                          
                          if (keyerControl & DIT_PROC) {  
                            keyerControl &= ~(DIT_L + DIT_PROC);
                            keyerState = CHK_DASH; 
                          } else  { keyerControl &= ~(DASH_L);
                            keyerState = IDLE;                                    
                          }
                        } 
                      
                        
                        
                        
                        break;
  } // end Case






  
  }
}

/*
 * Calculate dit length in milliseconds from WPM
 */

void updateWpm()
{

  elementLengthMs = 1200 / actualWpm;
  morse.dotLength = elementLengthMs;
  Serial.print("elementLengthMs: ");
  Serial.println(elementLengthMs);

  
  if (actualFarnsWpm > 0) {
    if (actualWpm >= actualFarnsWpm) exit;
    elementLengthMsFarns = 1200 / (actualWpm - actualFarnsWpm );
    Serial.print("elementLengthMsFarns: ");
    morse.farnsLength = elementLengthMsFarns;  
    Serial.println(elementLengthMsFarns);
    morse.useFansw = true;
  }

  morse.dotLength = elementLengthMs;
  morse.farnsLength = elementLengthMsFarns;
  
  
}


int writeToEeprom(){
  /*Serial.println("Save all to EEPROM!");
  Serial.println("--------------------1-------------------------");
  Serial.printf("actual WPM %d\n",actualWpm);
  Serial.printf("Setting Farnsworth to %d\n",actualFarnsWpm);
  Serial.printf("Sota mode %d\n",sotaqso);  
  Serial.printf("Sota mode morse %d\n",morse.sotaqso);  
  Serial.printf("Polarity  %d\n",paddlepolarity);
  Serial.printf("Iambic B mode %d\n",iambicab);
  Serial.printf("Frequency  %d\n",frequency);
  Serial.printf("Frequency morse %d\n",morse.frequency);
  Serial.printf("Frequency HZ %d\n",morse.frequencys[morse.frequency]);*/

  int addr=0;
  EEPROM.begin(512);  
  EEPROM.put(addr, eepromused);
  addr += sizeof(eepromused);
  EEPROM.put(addr, actualWpm);
  addr += sizeof(actualWpm);
  EEPROM.put(addr, actualFarnsWpm);
  addr += sizeof(actualFarnsWpm);
  EEPROM.put(addr, sotaqso);
  addr += sizeof(sotaqso);
  EEPROM.put(addr, paddlepolarity);
  addr += sizeof(paddlepolarity);
  EEPROM.put(addr, iambicab);
  addr += sizeof(iambicab);
  EEPROM.put(addr, frequency);  
  boolean rc = EEPROM.commit();
  EEPROM.end();

  /*Serial.println("--------------------2-------------------------");
  Serial.printf("EEprom commit variable is: %s\n", rc ? "true" : "false");  
  Serial.printf("actual WPM %d\n",actualWpm);
  Serial.printf("Setting Farnsworth to %d\n",actualFarnsWpm);
  Serial.printf("Sota mode %d\n",sotaqso);  
  Serial.printf("Sota mode morse %d\n",morse.sotaqso);  
  Serial.printf("Polarity  %d\n",paddlepolarity);
  Serial.printf("Iambic B mode %d\n",iambicab);
  Serial.printf("Frequency  %d\n",frequency);
  Serial.printf("Frequency morse %d\n",morse.frequency);
  Serial.printf("Frequency HZ %d\n",morse.frequencys[morse.frequency]);*/

  

  /*addr=0;
  EEPROM.begin(512);
  actualWpm =   EEPROM.read(addr);
  actualFarnsWpm = EEPROM.read(addr+sizeof(uint8_t));
  sotaqso = EEPROM.read(addr+sizeof(uint8_t));
  paddlepolarity = EEPROM.read(addr+sizeof(uint8_t));
  iambicab = EEPROM.read(addr+sizeof(uint8_t));
  frequency = EEPROM.read(addr+sizeof(uint8_t));  
  EEPROM.end();


  Serial.println("--------------------3-------------------------");
  Serial.printf("actual WPM %d\n",actualWpm);
  Serial.printf("Setting Farnsworth to %d\n",actualFarnsWpm);
  Serial.printf("Sota mode %d\n",sotaqso);  
  Serial.printf("Sota mode morse %d\n",morse.sotaqso);  
  Serial.printf("Polarity  %d\n",paddlepolarity);
  Serial.printf("Iambic B mode %d\n",iambicab);
  Serial.printf("Frequency  %d\n",frequency);
  Serial.printf("Frequency morse %d\n",morse.frequency);
  Serial.printf("Frequency HZ %d\n",morse.frequencys[morse.frequency]);
  Serial.println("----------------------------------------------");*/

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
    qsoDisplay::printMenu(page, actualWpm, actualFarnsWpm, morse.sotaqso);    
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
      qsoDisplay::resetMenuPointertoValues(menuitem);
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
            qsoDisplay::setMenuPointertoValues(4);
          break;
          case 5:            
            qsoDisplay::setMenuPointertoValues(5);
          break;
          case 6:
            rotary.setUpperBound(5);
            rotary.setLowerBound(-1);
            rotary.resetPosition(frequency,false);  
            qsoDisplay::setMenuPointertoValues(6);            
          break;
        default: break;                     
     }
    }   
  }
  Serial.println("click:  "); 
}

  /*String menu1 = "WPM";
  String menu2 = "Farnsworth";
  String menu3 = "SOTA Mode";
  String menu4 = "Paddle Polarity";
  String menu5 = "IAMBIC B";*/

void rotate(ESPRotary& r) {

  if (inMenu && !chvalue) {
    lastMenuItem = menuitem;
    menuitem = rotary.getPosition();   
    if (menuitem >= 4) {   
      if (page != 2) {
        qsoDisplay::prepareMenu();        
        qsoDisplay::printMenu(2, paddlepolarity, iambicab, morse.frequencys[frequency]);            
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
    } else if (menuitem == 4){ 
        paddlepolarity =  (paddlepolarity == 0) ? 1 : 0;        
        qsoDisplay::updateValues2d(1,paddlepolarity);                 
    } else if (menuitem == 5) {         
        iambicab =  (iambicab == 0) ? 1 : 0;
        qsoDisplay::updateValues2d(10,iambicab);                
    } else if (menuitem == 6) {                   
        frequency = rotary.getPosition();              
        int freq = morse.frequencys[frequency];  
        qsoDisplay::updateValues3d(19,freq); 
        morse.frequency = frequency; 
        
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



