/*
  Morse . h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#ifndef Morse_H
#define Morse_H
#include <Arduino.h>

namespace qsoTrainer {

static const struct {const char letter, *code;} MorseMap[] =
{
	{ 'a', ".-" },
	{ 'b', "-..." },
	{ 'c', "-.-." },
	{ 'd', "-.." },
	{ 'e', "." },
	{ 'f', "..-." },
	{ 'g', "--." },
	{ 'h', "...." },
	{ 'i', ".." },
	{ 'j', ".---" },
	{ 'k', "-.-" },  // fixed
	{ 'l', ".-.." },
	{ 'm', "--" },
	{ 'n', "-." },
	{ 'o', "---" },
	{ 'p', ".--." },
	{ 'q', "--.-" },
	{ 'r', ".-." },
	{ 's', "..." },
	{ 't', "-" },
	{ 'u', "..-" },
	{ 'v', "...-" },
	{ 'w', ".--" },
	{ 'x', "-..-" },
	{ 'y', "-.--" },
	{ 'z', "--.." },	
		
	{ '1', ".----" },
	{ '2', "..---" },
	{ '3', "...--" },
	{ '4', "....-" },
	{ '5', "....." },
	{ '6', "-...." },
	{ '7', "--..." },
	{ '8', "---.." },
	{ '9', "----." },
	{ '0', "-----" },

	{ ' ', "     " }, //Gap between word, seven units 	
	{ '.', ".–.–.–" }, // fixed
	{ ',', "--..--" },
	{ '?', "..--.." },
	{ '!', "-.-.--" },
	{ ':', "---..." },
	{ ';', "-.-.-." },
	{ '(', "-.--." },
	{ ')', "-.--.-" },
	{ '"', ".-..-." },
	{ '@', ".--.-." },
	{ '&', ".-..." },
  	{ '/', "-..-." } // added
};

enum qsoType{
	NONE,
	CHASER,
	ACTIVATE
};

enum qsoStatechase {
	NADA, 
    SKSK,
	CQ,
    SOTA,
    DE,
    HISCALL1,
    SSOTA,
    SUMMIT,
	K,
    OURCALL,
	GD,
	RST1, 
	RST2, 
    BK,
    TU,
	REF,
	MYSUMMIT, 
	RR,
    BYE,
	UR,
    SK,
	TU1,
    E1,
	E2,
	END1,
	END2,
    STARTQSO,
	QRZ,
	NEXTCALL,
	EE
};

// ich F9XYZ 579 579 BK [Report, break to him]
// er  TU ROB 429 429 BK
// ich RR 73 SK E E [Roger roger 73 end of contact]

struct index
    {
        long number;
        long pos;
    };



class Morse
{
  public:
    Morse(int soundPin, int keyPin);
	int useFansw;
	int dotLength;
	int farnsLength = 0;

    qsoStatechase State;
    qsoType Type;
    String tlg;
    


    void doQso(void);	
    void dot();
    void dash();
	void sendCode(String text);
	String decode(String text);
	String encode(String code);
	String randomCall();
	String randomRST();
	String randomSummit();
	bool LittleFSActive = false;
	
  private:
    int _keyPin;
	int _soundPin;
	int _calllength;
    int _repeats;
	String _hisCall;
    String _ourCall;
	String _ourName; 
    String _summit;
	String _mysummit;
	String _RST;
	struct index _idx;
	bool _checkRst(String _rst);
	void _qsoChaser();
	void _qsoActivate();
	
	
};


}

#endif