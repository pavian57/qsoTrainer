/*
  Morse . h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#ifndef Morse_H
#define Morse_H

#include <Arduino.h>

namespace qsoTrainer
{

	static const struct
	{
		const char letter, *code;
	} MorseMap[] =
		{
			{'a', ".-"},
			{'b', "-..."},
			{'c', "-.-."},
			{'d', "-.."},
			{'e', "."},
			{'f', "..-."},
			{'g', "--."},
			{'h', "...."},
			{'i', ".."},
			{'j', ".---"},
			{'k', "-.-"}, // fixed
			{'l', ".-.."},
			{'m', "--"},
			{'n', "-."},
			{'o', "---"},
			{'p', ".--."},
			{'q', "--.-"},
			{'r', ".-."},
			{'s', "..."},
			{'t', "-"},
			{'u', "..-"},
			{'v', "...-"},
			{'w', ".--"},
			{'x', "-..-"},
			{'y', "-.--"},
			{'z', "--.."},

			{'1', ".----"},
			{'2', "..---"},
			{'3', "...--"},
			{'4', "....-"},
			{'5', "....."},
			{'6', "-...."},
			{'7', "--..."},
			{'8', "---.."},
			{'9', "----."},
			{'0', "-----"},

			{' ', "     "},	 //Gap between word, seven units
			{'.', ".-.-.-"}, // fixed
			{',', "--..--"},
			{'?', "..--.."},
			{'!', "-.-.--"},
			{':', "---..."},
			{';', "-.-.-."},
			{'(', "-.--."},
			{')', "-.--.-"},
			{'"', ".-..-."},
			{'@', ".--.-."},
			{'&', ".-..."},
			{'/', "-..-."} // added
	};

	enum qsoType
	{
		NONE,
		TRAINING,
		CHASER,
		ACTIVATE,
		CALLSIGN,
		ABBREVIATION,
		CHAR
	};

	enum qsoStatechase
	{
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
		RRC,
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
		EE,
		CSECHO,
		CSWAIT,
		CSKEYOK,
		ABBRECHO,
		ABBRWAIT,
		ABBROK,
		CHARECHO,
		CHARWAIT,
		CHAROK
	};

	struct index
	{
		long number;
		long pos;
	};

	class Morse
	{
	public:
		Morse(int soundPin);
		int useFansw;
		int dotLength;
		int farnsLength = 0;
		int frequency = 700;

		qsoStatechase State;
		qsoType Type;
		String tlg;
		bool nextStep;
		bool sotaqso;
		void doQso(void);
		void dot();
		void dash();
		void cwOn();
		void cwOff();
		void sendCode(String text);
		String decode(String text);
		String encode(String code);
		String randomCall();
		String randomRST();
		String randomSummit();
		String randomcwAbbr();
		String randomcwChar();
		bool LittleFSActive = false;

	private:
		int _soundPin;
		int _calllength;
		int _repeats;
		int _numChars = 1;
		String _hisCall;
		String _ourCall;
		String _ourAbbr;
		String _ourChar;
		String _ourName;
		String _summit;
		String _mysummit;
		String _RST;
		String _hello = "gd";
		struct index _idx;
		bool _checkRst(String _rst);
		void _qsoChaser();
		void _qsoActivate();
		void _qsoTraining();
		void _qsoCallSign();
		void _qsoCWabbreviation();
		void _qsoCWchar();
	};

}

#endif
