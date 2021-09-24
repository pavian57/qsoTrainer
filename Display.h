#ifndef Display_H
#define Display_H
#include <Arduino.h>



namespace qsoDisplay {

void initDisplay();
void addString(String txt);
void setFont(boolean bold); //true set bold

void setSpeed();
void printWpm(int value);
void printFarnsWpm(int value);
void resetSpeed();

} //namespace

#endif