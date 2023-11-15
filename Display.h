 #ifndef DISPLAY_H
 #define DISPLAY_H
#include <Arduino.h>



namespace qsoDisplay {

    void initDisplay();
    void addString(String txt);    

    void prepareMenu();
    void printMenu(int val1, int val2, int val3);
    void updateMenu(int menupos,int lastmenupos);
    void updateValues(int position, int val);
    void setMenuPointertoValues(int pos);
    void restsetMenuPointertoValues(int pos);    
    void clearMenu();
    
    void handleTelnet();
    void printTelnet(String txt);

    


} //namespace

 #endif