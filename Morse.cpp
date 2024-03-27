/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include <Arduino.h>
#include <LittleFS.h>
#include "Display.h"
#include "Morse.h"

namespace qsoTrainer
{

  Morse::Morse(int soundPin)
  {
    pinMode(soundPin, OUTPUT);
    _soundPin = soundPin;
    useFansw = 0;
    farnsLength = 0; // 5 = 240ms 6 = 200ms 7 = 141ms
    dotLength = 75;  //16wpm
    State = NADA;
    Type = NONE;
    _calllength = 6;
    _repeats = 1;
    sotaqso = false;
    randomSeed(analogRead(A0));   
    if (LittleFS.begin()) {
      LittleFSActive = true;
    }
  }

  void Morse::dot()
  {
    tone(_soundPin, 622);
    delay(dotLength);
    noTone(_soundPin);
    delay(dotLength);
  }

  void Morse::dash()
  {
    tone(_soundPin, 622);
    delay(dotLength * 3);
    noTone(_soundPin);
    delay(dotLength);
  }

  String Morse::decode(String text)
  {
    int i;
    int j;
    String code = "";
    for (i = 0; text[i]; ++i) {
      for (j = 0; j < sizeof MorseMap / sizeof *MorseMap; ++j) {
        if (tolower(text[i]) == MorseMap[j].letter)
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
    if (code == "...-.-")
    {
      return "<sk>";
    }
    if (code == "-.-.-")
    {
      return "<ka>";
    }

    if (code == ".-...")
    {
      return "<as>";
    }
    if (code == "-...-")
    {
      return "<bt>";
    }
    if (code == "...-.")
    {
      return "<ve>";
    }
    if (code == ".-.-.")
    {
      return "<ar>";
    }
    if (code == "-....-")
    {
      return "<ba>";
    }


    /*
      Pro signs 
               <as> .-...
               <ka> -.-.-
               <kn> -.--.
               <sk> ...-.-
               <ve> ...-.
               <bt> -...-
               <ar> .-.-.
               <hh> ........
               <cl> -.-.
    */
      
               
    code.concat(" ");
    int lastPos = 0;
    int pos = code.indexOf(' ');
    while (lastPos <= code.lastIndexOf(' '))
    {
      for (int i = 0; i < sizeof MorseMap / sizeof *MorseMap; ++i)
      {
        if (code.substring(lastPos, pos) == MorseMap[i].code)
        {
          msg += MorseMap[i].letter;
        }
      }

      lastPos = pos + 1;
      pos = code.indexOf(' ', lastPos);

      // Handle white-spaces between words (7 spaces)
      while (code[lastPos] == ' ' && code[pos + 1] == ' ')
      {
        pos++;
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
    for (int i = 0; code[i]; i++)
    {
      switch (code[i])
      {
      case '.': //dit
        dot();
        break;

      case '-': //dah
        dash();
        break;

      case ' ': //gap
        if (farnsLength == 0)
        {
          delay(dotLength * 3);
        }
        else
        {
          delay(farnsLength * 3);
        }
      }
    }
    if (farnsLength == 0)
    {
      delay(dotLength * 7);
    }
    else
    {
      delay(farnsLength * 7);
    }
  }

  String Morse::randomcwChar(){
    int i = 0;
    //Serial.printf("numChars %d\n",_numChars);

    if (_numChars == 1) {
      _ourChar =  MorseMap[random(0, 35)].letter;
      return _ourChar;
    } else {
      _ourChar.clear();
    }
    while (i < _numChars) {
        _ourChar += MorseMap[random(0, 35)].letter;
        i++;
      }
    
    return _ourChar;
    //callsign += MorseMap[random(26, 35)].letter;  
  }

  String Morse::randomcwAbbr(){
    int i = 0;
    char idxbuffer[64]; //declare a buffer - hopefully this speeds things up...
    long fsize = 0;
    long randNumber;
    
    if (LittleFS.exists(F("/abbr.idx"))  &&  LittleFSActive) {
      File f1 = LittleFS.open("/abbr.txt", "r");
      File fx = LittleFS.open("/abbr.idx", "r");
      
      fsize = fx.size();
      randNumber = random(0, (fsize / 8) - 1);

      fx.seek(randNumber * 8);
      fx.read((byte *)&_idx, 8);

      f1.seek(_idx.pos);
      int l = f1.readBytesUntil(' ', idxbuffer, sizeof(idxbuffer));
      idxbuffer[l] = 0;
      _ourAbbr = String(idxbuffer);
      _ourAbbr.toLowerCase();
      
      fx.close();
      f1.close();
      return _ourAbbr;
    } else {
      return "no abbreviation found";
    }
  }
  

  String Morse::randomCall()
  {
    int i = 0;
    char idxbuffer[64]; //declare a buffer - hopefully this speeds things up...
    long fsize = 0;
    long randNumber;
    
    if (!LittleFS.exists(F("/names.idx"))  &&  LittleFSActive)
    {
     
      String callsign;
      callsign += MorseMap[random(0, 25)].letter;
      callsign += MorseMap[random(0, 25)].letter;
      callsign += MorseMap[random(26, 35)].letter;
      while (i < 3) {
        callsign += MorseMap[random(0, 25)].letter;
        i++;
      }
      return callsign;
    }
    else
    {
      File f1 = LittleFS.open("/names.txt", "r");
      File fx = LittleFS.open("/names.idx", "r");
      randomSeed(millis());
      fsize = fx.size();
      randNumber = random(0, (fsize / 8) - 1);

      fx.seek(randNumber * 8);
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
    
    String rst = "";
    rst += MorseMap[random(28, 30)].letter;
    rst += MorseMap[random(29, 35)].letter;
    rst += MorseMap[random(29, 35)].letter;
    return rst;
  }

  String Morse::randomSummit()
  {
    int i = 0;
    char idxbuffer[64]; //declare a buffer - hopefully this speeds things up...
    long fsize = 0;
    long randNumber;
    String summit = "";
    if (!LittleFS.exists(F("/summitslist.idx"))  &&  LittleFSActive){
      summit += MorseMap[random(0, 25)].letter;
      summit += MorseMap[random(0, 25)].letter;
      summit += "/";
      summit += MorseMap[random(0, 25)].letter;
      summit += MorseMap[random(0, 25)].letter;
      while (i < 3) {
        summit += MorseMap[random(26, 35)].letter;
        i++;
      }
      return summit;
    } else {
      File f1 = LittleFS.open("/summitslist.txt", "r");
      File fx = LittleFS.open("/summitslist.idx", "r");
      
      fsize = fx.size();
      randNumber = random(0, (fsize / 8) - 1);

      fx.seek(randNumber * 8);
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
      summit.replace("-", "");
      summit.replace("\n", "");
      summit.replace("\r", "");
      //Serial.print(" Name: ");
      fx.close();
      f1.close();
      return summit;
    }
  }

  bool Morse::_checkRst(String _rst)
  {

    if (isDigit(_rst.charAt(0)))
    {
      if (isDigit(_rst.charAt(1)) || _rst.charAt(1) == 'n')
      {
        if (isDigit(_rst.charAt(2)) || _rst.charAt(2) == 'n')
        {
          return true;
        }
      }
    }
    return false;
  }

  void Morse::doQso(void)
  {
    tlg.trim();

    if (strstr(tlg.c_str(), "<sk><sk>") != NULL)
    {
      qsoDisplay::addString("<sk>");
      tlg.replace("<sk><sk>", "");
      qsoDisplay::addString("73");
      sendCode("73");
      Type = NONE;
      State = NADA;
    }

    if (strstr(tlg.c_str(), "qrl?") != NULL && Type != TRAINING)
    {
      qsoDisplay::addString("qrl?");
      tlg.replace("qrl?", "");
      if (Type == CHASER) {
        Type = NONE;
        State = NADA;  
        qsoDisplay::addString("rr");
        sendCode("rr");
        nextStep = false;
        return;
      }
      Type = CHASER;
      State = CQ;      
    }
    
    if (strstr(tlg.c_str(), "<ve><ve>") != NULL && Type != TRAINING) 
    {
      randomSeed(analogRead(A0));   
      qsoDisplay::addString("<ve>");
      tlg.replace("<ve><ve>", "");
      if (Type == ABBREVIATION) {
        Type = NONE;
        State = NADA;          
        qsoDisplay::addString("rr");
        sendCode("rr");
        nextStep = false;
        return;
      }   
      Type = ABBREVIATION;
      State = ABBRECHO;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "<ka><ka>") != NULL && Type != TRAINING)
    {
      qsoDisplay::addString("<ka>");
      tlg.replace("<ka><ka>", "");
      if (Type == ACTIVATE) {
        Type = NONE;
        State = NADA;  
        qsoDisplay::addString("rr");
        sendCode("rr");
        nextStep = false;
        return;
      }
      Type = ACTIVATE;
      State = STARTQSO;
    }
    if (strstr(tlg.c_str(), "<as><as>") != NULL)
    {
      qsoDisplay::addString("<as>");
      if (Type == TRAINING) {
        Type = NONE;
        State = NADA;  
        qsoDisplay::addString("rr");
        sendCode("rr");
        nextStep = false;
        return;
      }
      Type = TRAINING;
      State = NADA;
      nextStep = true;
    }
    if (strstr(tlg.c_str(), "<bt><bt>") != NULL)
    {
      randomSeed(analogRead(A0));   
      qsoDisplay::addString("<bt>");
      if (Type == CALLSIGN) {
        Type = NONE;
        State = NADA;  
        qsoDisplay::addString("rr");
        sendCode("rr");
        nextStep = false;
        return;
      }
      Type = CALLSIGN;
      State = CSECHO;
      nextStep = true;
    }
    if (strstr(tlg.c_str(), "<ar><ar>") != NULL)
    {
      randomSeed(analogRead(A0));   
      qsoDisplay::addString("<ar>");
      if (Type == CHAR) {
        Type = NONE;
        State = NADA;  
        qsoDisplay::addString("rr");
        sendCode("rr");
        _numChars = 1;
        nextStep = false;
        return;
      }
      Type = CHAR;
      State = CHARECHO;
      nextStep = true;
    }


    switch (Type)
    {
    case ACTIVATE:
      _qsoActivate();
      break;
    case (CHASER):
      _qsoChaser();
      break;
    case (TRAINING):
      _qsoTraining();
      break;
    case (CALLSIGN):
      _qsoCallSign();
      break;
    case (ABBREVIATION):
      _qsoCWabbreviation();
      break;
    case (CHAR):
      _qsoCWchar();
      break;
    default:
      break;
    }
  }

  void Morse::_qsoActivate(void)
  {
    tlg.trim();
    nextStep = false;
    if (State == STARTQSO)
    {
      sendCode("qrl?");
      qsoDisplay::addString("qrl?");
      sendCode("cq");
      qsoDisplay::addString("cq");
      if (sotaqso) {
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
      } else {
        sendCode("de");
        qsoDisplay::addString("de");
        _ourCall = Morse::randomCall();
        sendCode(_ourCall);
        qsoDisplay::addString(_ourCall);
      }
      sendCode("k");
      qsoDisplay::addString("k");
      State = HISCALL1;
    }

    if (tlg.length() >= _calllength && State == HISCALL1)
    {
      _hisCall = tlg;
      qsoDisplay::addString(_hisCall);
      sendCode(_hisCall);
      qsoDisplay::addString(_hisCall);
      sendCode(_hello);
      qsoDisplay::addString(_hello);
      _RST = randomRST();
      sendCode(_RST);
      qsoDisplay::addString(_RST);
      sendCode(_RST);
      qsoDisplay::addString(_RST);
      sendCode("bk");
      qsoDisplay::addString("bk");
      State = RRC;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "rr") != NULL && State == RRC)
    {
      tlg.replace("rr", "");
      qsoDisplay::addString("rr");
      State = GD;
      nextStep = true;
    }

    if ((strstr(tlg.c_str(), "gd") || strstr(tlg.c_str(), "gm") || strstr(tlg.c_str(), "ga") || strstr(tlg.c_str(), "ge")) && State == GD) {
      if (strstr(tlg.c_str(), "gd")) { qsoDisplay::addString("gd"); _hello = "gd"; }
      if (strstr(tlg.c_str(), "gm")) { qsoDisplay::addString("gm"); _hello = "gm"; }
      if (strstr(tlg.c_str(), "ga")) { qsoDisplay::addString("ga"); _hello = "ga"; }
      if (strstr(tlg.c_str(), "ge")) { qsoDisplay::addString("ge"); _hello = "ge"; }
      State = RST1;
      nextStep = true;
    }


    if (_checkRst(tlg) && State == RST2)
    {
      qsoDisplay::addString(tlg);
      if (sotaqso) {
        State = REF;
      } else {
        State = BYE;
      }
      nextStep = true;
    }

    if (_checkRst(tlg) && State == RST1)
    {
      qsoDisplay::addString(tlg);
      State = RST2;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "ref") != NULL && State == REF)
    {
      tlg.replace("ref", "");
      qsoDisplay::addString("ref");
      State = MYSUMMIT;
      nextStep = true;
    }

    if (tlg.length() >= _calllength && State == MYSUMMIT)
    {
      int i = tlg.indexOf("/");
      if (i > 1)
      {
        _mysummit = tlg;
        i = _mysummit.length();

        State = BYE;
        nextStep = true;
        if (isDigit(_mysummit[i]))
        {
          if (isDigit(_mysummit[i - 1]))
          {
            if (isDigit(_summit[i - 2]))
            {

              State = BYE;
              nextStep = true;
            }
          }
        }

        qsoDisplay::addString(_mysummit);
      }
    }

    if (strstr(tlg.c_str(), "73") != NULL && State == BYE) {
      tlg.replace("73", "");
      qsoDisplay::addString("73");
      State = END1;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "tu") != NULL && State == END1) {
      tlg.replace("tu", "");
      qsoDisplay::addString("tu");
      nextStep = true;
      State = BK;
    }


    if (strstr(tlg.c_str(), "bk") != NULL && State == BK) {
      tlg.replace("bk", "");
      qsoDisplay::addString("bk");

      sendCode("73");
      qsoDisplay::addString("73");
      sendCode("tu");
      qsoDisplay::addString("tu");      
      sendCode("ee");
      qsoDisplay::addString("ee");      
      State = HISCALL1;
      nextStep = true;
    }
  }



  void Morse::_qsoChaser(void) {
    tlg.trim();
    nextStep = false;
    if (strstr(tlg.c_str(), "cq") != NULL && State == CQ) {
      tlg.replace("cq", "");
      qsoDisplay::addString("cq");
      if (sotaqso) {
        State = SOTA;
      } else {
        State = DE;
      }  
      nextStep = true;
    }
    if (strstr(tlg.c_str(), "sota") != NULL && State == SOTA) {
      tlg.replace("sota", "");
      qsoDisplay::addString("sota");
      State = DE;
      nextStep = true;
    }
    if (strstr(tlg.c_str(), "de") != NULL && State == DE) {
      tlg.replace("de", "");
      qsoDisplay::addString("de");
      tlg = "";
      State = HISCALL1;
      nextStep = true;
    }

    if (tlg.length() >= _calllength && State == HISCALL1) {
      _hisCall = tlg;
      tlg.replace(_hisCall, "");
      qsoDisplay::addString(_hisCall);
      tlg = "";
      if (sotaqso) {
        State = SUMMIT;
      } else {
        State = K;
      }  
      nextStep = true;
    }

    if (tlg.length() >= _calllength && State == SUMMIT) {
      int i = tlg.indexOf("/");
      if (i > 1) {
        _summit = tlg;
        i = _summit.length();
        tlg.replace(_summit, "");
        qsoDisplay::addString(_summit);
        State = K;
        nextStep = true;
        if (isDigit(_summit[i])) {
          if (isDigit(_summit[i - 1])) {
            if (isDigit(_summit[i - 2])) {
              qsoDisplay::addString(_summit);
              State = K;
              nextStep = true;
            }
          }
        }
      }
      tlg = "";
    }

    if (strstr(tlg.c_str(), "qrz") != NULL && State == QRZ) {
      qsoDisplay::addString("qrz");
      State = NEXTCALL;
      nextStep = true;
    }

    if ((strstr(tlg.c_str(), "k") != NULL && State == K) || State == NEXTCALL) {
      if (State != NEXTCALL) {
        tlg.replace("k", "");
        qsoDisplay::addString("k");
        nextStep = true;
      }
      _ourCall = randomCall();
      sendCode(_ourCall);
      qsoDisplay::addString(_ourCall);
      State = OURCALL;
    }

    if (strstr(tlg.c_str(), _ourCall.c_str()) && State == OURCALL) {
      qsoDisplay::addString(tlg);
      tlg.replace(tlg, "");
      State = GD;
      nextStep = true;
    } 
     if (State == OURCALL) {      
      if (_repeats >= 5) {
        sendCode(_ourCall);
        _repeats = 1;
      }
      Serial.println(_repeats);
      _repeats++;
    }
    

    if ((strstr(tlg.c_str(), "gd") || strstr(tlg.c_str(), "gm") || strstr(tlg.c_str(), "ga") || strstr(tlg.c_str(), "ge")) && State == GD) {
      if (strstr(tlg.c_str(), "gd")) { qsoDisplay::addString("gd"); _hello = "gd"; }
      if (strstr(tlg.c_str(), "gm")) { qsoDisplay::addString("gm"); _hello = "gm"; }
      if (strstr(tlg.c_str(), "ga")) { qsoDisplay::addString("ga"); _hello = "ga"; }
      if (strstr(tlg.c_str(), "ge")) { qsoDisplay::addString("ge"); _hello = "ge"; }
      State = RST1;
      nextStep = true;
    }

    if (tlg.length() == 3 && State == RST1) {
      if (_checkRst(tlg)) {
        _RST = tlg;
        tlg.replace(_RST, "");
        qsoDisplay::addString(_RST);
        State = RST2;
        nextStep = true;
      }
    }

    if (tlg == _RST && State == RST2) {
      qsoDisplay::addString(_RST);
      State = BK;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "bk") != NULL && State == BK) {
      tlg.replace("bk", "");
      qsoDisplay::addString("bk");
      sendCode("rr");
      qsoDisplay::addString("rr");
      sendCode(_hello);
      qsoDisplay::addString(_hello);
      _RST = randomRST();
      sendCode(_RST);
      qsoDisplay::addString(_RST);
      sendCode(_RST);
      qsoDisplay::addString(_RST);
      if (sotaqso) {
        sendCode("ref");
        qsoDisplay::addString("ref");
        _summit = Morse::randomSummit();
        sendCode(_summit);
        qsoDisplay::addString(_summit);
        sendCode(_summit);
        qsoDisplay::addString(_summit);
      }      
      sendCode("73");
      qsoDisplay::addString("73");
      sendCode("tu");
      qsoDisplay::addString("tu");
      sendCode("bk");
      qsoDisplay::addString("bk");
      State = BYE;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "73") != NULL && State == BYE) {
      tlg.replace("73", "");
      qsoDisplay::addString("73");
      State = END1;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "tu") != NULL && State == END1) {
      tlg.replace("tu", "");
      qsoDisplay::addString("tu");
      State = BYE;
      nextStep = true;
    }

    if (strstr(tlg.c_str(), "ee") != NULL && State == BYE) {
      tlg.replace("ee", "");
      qsoDisplay::addString("ee");
      nextStep = true;
      _ourCall = randomCall();
      sendCode(_ourCall);
      qsoDisplay::addString(_ourCall);
      State = OURCALL;
    }
  }

  void Morse::_qsoTraining(void) {
    qsoDisplay::addString(tlg);
  }

  void Morse::_qsoCallSign(void) {
    if (State == CSECHO) {
      Serial.println("Callsign Training");
      _ourCall = Morse::randomCall();
      sendCode(_ourCall);
      qsoDisplay::addString(_ourCall);
      State = CSWAIT;
      _repeats = 0;     
    } else {
      tlg.trim();
      if (strstr(tlg.c_str(), _ourCall.c_str()) && State == CSWAIT) {
        qsoDisplay::addString(tlg);
        tlg.replace(tlg, "");
        _ourCall = Morse::randomCall();
        sendCode(_ourCall);
        qsoDisplay::addString(_ourCall);
        _repeats = 0;
      }    
      if (_repeats >= 5) {
        sendCode(_ourCall);
        _repeats = 0;
      }    
      Serial.println(_repeats);
      _repeats++;
    }
  }
 

  void Morse::_qsoCWabbreviation(void) {
    if (State == ABBRECHO) {
      Serial.println("Abbreviation Training");
      _ourAbbr = Morse::randomcwAbbr();
      sendCode(_ourAbbr);
      qsoDisplay::addString(_ourAbbr);
      State = ABBRWAIT;
      _repeats = 0;
    } else {
      tlg.trim();
      if (strstr(tlg.c_str(), _ourAbbr.c_str()) && State == ABBRWAIT) {
        qsoDisplay::addString(tlg);
        tlg.replace(tlg, "");
        _ourAbbr = Morse::randomcwAbbr();
        sendCode(_ourAbbr);
        qsoDisplay::addString(_ourAbbr);
        _repeats = 0;
      }
      if (_repeats >= 5) {
        sendCode(_ourAbbr);
        _repeats = 0;     
      }
      _repeats++; 
    }
  }

  void Morse::_qsoCWchar(void) {
    if (_repeats >= 5) {
      sendCode(_ourChar);
      _repeats = 0;
    }    
    if (State == CHARECHO) {
      Serial.println("single char Training");
      _ourChar = Morse::randomcwChar();
      sendCode(_ourChar);
      qsoDisplay::addString(_ourChar);
      State = CHARWAIT;
      _repeats = 0;
    } else {
      tlg.trim();
      if (strstr(tlg.c_str(), "ar")) {  // +1
        _numChars++;
        sendCode(String(_numChars));
      }
      if (strstr(tlg.c_str(), "ba")) {  //-1
        if(_numChars > 1) _numChars--;
        sendCode(String(_numChars));
      }
      if (strstr(tlg.c_str(), _ourChar.c_str()) && State == CHARWAIT) {
        tlg.replace(tlg, "");
        _ourChar = Morse::randomcwChar();
        _repeats = 0;
        qsoDisplay::addString(_ourChar);
        sendCode(_ourChar);        
      }
    _repeats++;          
    }

  }

}

/*

Activator: CQ SOTA DE XY2ABC XY/KB023 K
Chaser   : XY2DEF
Activator: XY2DEF GD 559 559 BK
Chaser   : RR GD 559 559 REF XY/KB045 XY/KB045 73 TU BK
Activator: 73 TU EE
Chaser   : XY2GHJ

*/