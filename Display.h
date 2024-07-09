 #ifndef DISPLAY_H
 #define DISPLAY_H
#include <Arduino.h>



namespace qsoDisplay {

    void initDisplay();
    void intro(int wpm, int fans, int sota, int polari, int iambic, int freq);
    void addString(String txt);    

    void prepareMenu();
    void printMenu(int page, int val1, int val2, int val3);
    void updateMenu(int page, int menupos,int lastmenupos);
    void updateValues(int position, int val);
    void updateValues2d(int position, int val);
    void updateValues3d(int position, int val);
    void setMenuPointertoValues(int pos);
    void resetMenuPointertoValues(int pos);    
    void clearMenu();
    
    void handleTelnet();
    void printTelnet(String txt);

    


} //namespace

 #endif