/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include <Arduino.h>
#include <LittleFS.h>
#include "Display.h"
#include "Morse.h"

namespace qsoTrainer {

Morse::Morse(int soundPin, int keyPin)
{
  pinMode(keyPin, OUTPUT);
  pinMode(soundPin, OUTPUT);
  _keyPin = keyPin;
  _soundPin = soundPin;
  useFansw = 0;
  farnsLength = 0; // 5 = 240ms 6 = 200ms 7 = 141ms
  dotLength = 75; //16wpm
  State = NADA;
  Type = NONE;
  _calllength = 6;  
  _repeats = 1;
  if (LittleFS.begin()) {
    LittleFSActive = true;
  } 
}

void Morse::dot()
{
  digitalWrite(_keyPin, HIGH);
  tone(_soundPin, 622);
  delay(dotLength);
  noTone(_soundPin);
  digitalWrite(_keyPin, LOW);
  delay(dotLength);
}

void Morse::dash()
{
  digitalWrite(_keyPin, HIGH);
  tone(_soundPin, 622);
  delay(dotLength*3);
  noTone(_soundPin);
  digitalWrite(_keyPin, LOW);
  delay(dotLength);
}

String Morse::decode(String text)
{
  int  i;
  int  j;
  String code = "";
  for( i = 0; text[i]; ++i )
  {
    for( j = 0; j < sizeof MorseMap / sizeof *MorseMap; ++j )
    {
      if( tolower(text[i]) == MorseMap[j].letter )
      {
        code += MorseMap[j].code;
        break;
      }
    }
    code += " "; //Add tailing space to seperate the chars
  }
  return code;  
}

String Morse::encode(String code)
{
  String msg = "";
  code.trim();
  if (code == "...-.-") {
    return "<sk>";
  }
  if (code == "-.-.-") {
    return "<ka>";
  }
  code.concat(" ");
  int lastPos = 0;
  int pos = code.indexOf(' ');
  while( lastPos <= code.lastIndexOf(' ') )
  {    
    for( int i = 0; i < sizeof MorseMap / sizeof *MorseMap; ++i )
    {
      if( code.substring(lastPos, pos) == MorseMap[i].code )
      {
        msg += MorseMap[i].letter;
      }
    }

    lastPos = pos+1;
    pos = code.indexOf(' ', lastPos);
    
    // Handle white-spaces between words (7 spaces)
    while( code[lastPos] == ' ' && code[pos+1] == ' ' )
    {
      pos ++;
    }
  }

  return msg;
}

void Morse::sendCode(String text)
{
  String code;
  text.trim();
  Serial.println(text);
  code = decode(text);
  Serial.println(code);
  for(int i=0; code[i]; i++)
  {
    switch( code[i] )
    {      
      case '.': //dit      
        dot();          
        break;

      case '-': //dah
        dash();
        break;

      case ' ': //gap
        if (farnsLength == 0) {         
          delay(dotLength*3);  
        } else {
          delay(farnsLength*3);          
        }
    }
  }
  if (farnsLength == 0) {
    delay(dotLength*7);  
  } else {
    delay(farnsLength*7);
  }
}

String Morse::randomCall()
{
  //int i = 0;
  char idxbuffer[64];   //declare a buffer - hopefully this speeds things up...
  long fsize = 0;
  long randNumber;
  if (!LittleFSActive && !LittleFS.exists(F("/names.idx"))) {
    randomSeed(millis());
    String callsign;
    int i;
    callsign += MorseMap[random(0,25)].letter;
    callsign += MorseMap[random(0,25)].letter;
    callsign += MorseMap[random(26,35)].letter;
    while(i<3) {
      callsign += MorseMap[random(0,25)].letter;
      i++; 
    }
    return callsign;
  } else {
    File f1 = LittleFS.open("/names.txt", "r");
    File fx = LittleFS.open("/names.idx", "r");
    randomSeed(millis());
    fsize = fx.size();
    randNumber = random(0,(fsize/8)-1 );
            
    fx.seek(randNumber*8);
    fx.read((byte *)&_idx, 8);

    f1.seek(_idx.pos);
    int l = f1.readBytesUntil(',', idxbuffer, sizeof(idxbuffer));
    idxbuffer[l] = 0;
    _ourCall = String(idxbuffer);
    _ourCall.toLowerCase();
    l = f1.readBytesUntil('\n', idxbuffer, sizeof(idxbuffer));
    idxbuffer[l] = 0;
    _ourName = String(idxbuffer);
    fx.close();
    f1.close();
    return _ourCall;
  }

}

String Morse::randomRST()
{
  int i = 0;
  randomSeed(millis());
  String rst = "";
  rst += MorseMap[random(28,30)].letter;
  rst += MorseMap[random(29,35)].letter;
  rst += MorseMap[random(29,35)].letter;
  return rst;
}

String Morse::randomSummit()
{
  int i = 0;
  char idxbuffer[64];   //declare a buffer - hopefully this speeds things up...
  long fsize = 0;
  long randNumber;
  String summit = "";
  if (!LittleFSActive && !LittleFS.exists(F("/summitslist.idx"))) {
    summit += MorseMap[random(0,25)].letter;
    summit += MorseMap[random(0,25)].letter;
    summit += "/";
    summit += MorseMap[random(0,25)].letter;
    summit += MorseMap[random(0,25)].letter;
    while(i<3) {
      summit += MorseMap[random(26,35)].letter;
      i++;
    }
    return summit;
  } else {
    
    File f1 = LittleFS.open("/summitslist.txt", "r");
    File fx = LittleFS.open("/summitslist.idx", "r");
    randomSeed(millis());
    fsize = fx.size();
    randNumber = random(0,(fsize/8)-1 );
            
    fx.seek(randNumber*8);
    fx.read((byte *)&_idx, 8);

    /*Serial.print("Line:  ");
    Serial.print(idx.number);
    Serial.print(" Position: ");
    Serial.println(idx.pos);       */
    f1.seek(_idx.pos);
    int l = f1.readBytesUntil('\n', idxbuffer, sizeof(idxbuffer));
    idxbuffer[l] = 0;
    summit = String(idxbuffer);
    summit.toLowerCase();
    summit.replace("-","");
    summit.replace("\n","");
    summit.replace("\r","");
    //Serial.print(" Name: ");
    fx.close();
    f1.close();
    return summit;
  
  }
}


bool Morse::_checkRst(String _rst)
{
      
  if (isDigit(_rst.charAt(0))) {    
    if (isDigit(_rst.charAt(1)) || _rst.charAt(1) == 'n') {
      if (isDigit(_rst.charAt(2)) || _rst.charAt(2) == 'n') {
        return true;
      }
    }
  }   
  return false;
}

void Morse::doQso(void)
{
  tlg.trim();
  if (strstr(tlg.c_str(), "<sk><sk>") != NULL) {
    qsoDisplay::addString("<sk>");
    tlg.replace("<sk><sk>", "");
    sendCode("73");
    Type = NONE; 
    State = NADA; 
  }

  if (strstr(tlg.c_str(), "qrl?") != NULL) {
    qsoDisplay::addString("qrl?");
    Type = CHASER;
    State = CQ; 
  } 
  if (strstr(tlg.c_str(), "<ka><ka>") != NULL) {
    qsoDisplay::addString("<ka>");
    tlg.replace("<ka><ka>", "");
    Type = ACTIVATE;
    State = STARTQSO; 
  }
  switch (Type) {
    case ACTIVATE:
      _qsoActivate();
      break;
    case (CHASER): 
      _qsoChaser();
      break;
    default: 
      break;
  }



}


void Morse::_qsoActivate(void)
{
  tlg.trim();
  
  if (State == STARTQSO) {
    sendCode("qrl?");
    qsoDisplay::addString("qrl?");
    sendCode("cq");
    qsoDisplay::addString("cq");
    sendCode("sota");
    qsoDisplay::addString("sota");
    sendCode("de");
    qsoDisplay::addString("de");
    _ourCall = Morse::randomCall();
    sendCode(_ourCall);
    qsoDisplay::addString(_ourCall);
    _summit = Morse::randomSummit(); 
    sendCode(_summit);
    qsoDisplay::addString(_summit);
    sendCode("k");
    qsoDisplay::addString("k");
    State = HISCALL1; 
  }

if (tlg.length() >= _calllength && State == HISCALL1) {
  _hisCall = tlg;
  qsoDisplay::addString(_hisCall);
  sendCode(_hisCall);
  
  qsoDisplay::addString(_hisCall);
  _RST = randomRST();
  sendCode(_RST);
  qsoDisplay::addString(_RST);
  sendCode(_RST);
  qsoDisplay::addString(_RST);
  sendCode("bk");
  qsoDisplay::addString("bk");
  State = TU;  
}

if (strstr(tlg.c_str(), "tu") != NULL && State == TU) {
    tlg.replace("tu", "");
    qsoDisplay::addString("tu");
    State = RST1;
  }

if (_checkRst(tlg) && State == RST2) {
  qsoDisplay::addString(tlg);
  State = REF;
  
}

if (_checkRst(tlg) && State == RST1) {
  qsoDisplay::addString(tlg);
  State = RST2;
  
}

if (strstr(tlg.c_str(), "ref") != NULL && State == REF) {
    tlg.replace("ref", "");
    
    qsoDisplay::addString("ref");
    State = MYSUMMIT;
}

if (tlg.length() >= _calllength && State == MYSUMMIT) {
      int i = tlg.indexOf("/");
      if (i > 1) {
        _mysummit = tlg;
        i = _mysummit.length();
        
        State = BK;  
        if (isDigit(_mysummit[i])) {
          if (isDigit(_mysummit[i-1])) {
            if (isDigit(_summit[i-2])) {
              
              State = BK;  
            }
          }
        }
        
        qsoDisplay::addString(_mysummit);  
      }
  }


if (strstr(tlg.c_str(), "bk") != NULL && State == BK) {
    tlg.replace("bk", "");
    qsoDisplay::addString("bk");

    sendCode("rr");
    qsoDisplay::addString("rr");
    sendCode("73");
    qsoDisplay::addString("73");
    sendCode("e");
    qsoDisplay::addString("e");
    sendCode("e");
    qsoDisplay::addString("e");
    State = E1;
  }

if (strstr(tlg.c_str(), "ee") != NULL && State == E1) {
    qsoDisplay::addString("ee");
    tlg.replace("ee", "");
    State = HISCALL1;
    sendCode("qrz");
    qsoDisplay::addString("qrz");
  }
}




void Morse::_qsoChaser(void)
{
  tlg.trim();
    
  if (strstr(tlg.c_str(), "cq") != NULL && State == CQ) {
    tlg.replace("cq", "");
    qsoDisplay::addString("cq");
    State = SOTA;
  }
  if (strstr(tlg.c_str(), "sota") != NULL && State == SOTA) {
    tlg.replace("sota", "");
    qsoDisplay::addString("sota");
    State = DE;
  }
  if (strstr(tlg.c_str(), "de") != NULL && State == DE) {
    tlg.replace("de", "");
    qsoDisplay::addString("de");
    tlg = "";
    State = HISCALL1;
  }

  if (tlg.length() >= _calllength && State == HISCALL1) {
      _hisCall = tlg;
      tlg.replace(_hisCall, "");
      qsoDisplay::addString(_hisCall);
      tlg = "";
      State = SUMMIT;   
  }
 
  if (tlg.length() >= _calllength && State == SUMMIT) {
      int i = tlg.indexOf("/");
      if (i > 1) {
        _summit = tlg;
        i = _summit.length();
        tlg.replace(_summit, "");
        qsoDisplay::addString(_summit);
        State = K;  
        if (isDigit(_summit[i])) {
          if (isDigit(_summit[i-1])) {
            if (isDigit(_summit[i-2])) {
              qsoDisplay::addString(_summit);
              State = K;  
            }
          }
        }
      }
    tlg = "";
  }

  if (strstr(tlg.c_str(), "qrz") != NULL && State == QRZ) {
    qsoDisplay::addString("qrz");
    State = NEXTCALL;
  }

  if ((strstr(tlg.c_str(), "k") != NULL && State == K) || State == NEXTCALL) {
    if (State != NEXTCALL) {
      tlg.replace("k", "");
      qsoDisplay::addString("k");
    }
    _ourCall = randomCall();
    sendCode(_ourCall);
    qsoDisplay::addString(_ourCall);
    State = OURCALL;
  }

  if (tlg == _ourCall && State == OURCALL) {      
      qsoDisplay::addString(tlg);
      tlg.replace(tlg, "");
      State = RST1;  
  }

  if (tlg == _RST && State == RST2) {
    qsoDisplay::addString(tlg);  
    State = BK;
  }

  if (tlg.length() == 3 && State == RST1) {
    if (_checkRst(tlg)) {
      _RST = tlg;
      qsoDisplay::addString(tlg);
      State = RST2;
    }
 }

if (strstr(tlg.c_str(), "bk") != NULL && State == BK) {
    tlg.replace("bk", "");
    qsoDisplay::addString("bk");  
    sendCode("tu");
    qsoDisplay::addString("tu");      
    _RST = randomRST();
    sendCode(_RST);
    qsoDisplay::addString(_RST);
    sendCode(_RST);    
    qsoDisplay::addString(_RST);
    sendCode("ref");
    qsoDisplay::addString("ref");
    _summit = Morse::randomSummit(); 
    sendCode(_summit);
    qsoDisplay::addString(_summit);
    sendCode(_summit);
    qsoDisplay::addString(_summit);
    sendCode("bk");
    qsoDisplay::addString("bk");
    State = RR;
  }

if (strstr(tlg.c_str(), "rr") != NULL && State == RR) {
    tlg.replace("rr", "");
    qsoDisplay::addString("rr");
    State = BYE;
  }

if (strstr(tlg.c_str(), "73") != NULL && State == BYE) {
    tlg.replace("73", "");
    qsoDisplay::addString("73");
    State = END1;
  }

if (strstr(tlg.c_str(), "e") != NULL && State == END2) {
    tlg.replace("e", "");    
    qsoDisplay::addString("e");
    sendCode("ee");
    qsoDisplay::addString("ee");
    State = QRZ;
  }  

if (strstr(tlg.c_str(), "e") != NULL && State == END1) {    
    tlg.replace("e", "");
    qsoDisplay::addString("e");
    State = END2;            
  }  



}

}
  
  /*

Activator: CQ SOTA DE XY2ABC XY/KB023 K
Chaser   : XY2DEF
Activator: XY2DEF 559 559 BK
Chaser   : TU 559 559 REF XY/KB045 XY/KB045 BK
Activator: RR 73 E E
Chaser   : EE
Activator: QRZ



*/