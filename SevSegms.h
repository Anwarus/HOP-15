#ifndef SEVSEGMS_h
#define SEVSEGMS_h

#include <Arduino.h>
#include <SevSeg.h>
#include <math.h>

class ProgramState;

class SevSegms
{
private:

	SevSeg _sevSeg;
	
	byte _numDigits = 2;
	byte _digitPins[2] = { 37, 35 };
	byte _segmentPins[8] = { 41, 39, 23, 29, 27, 33, 31, 25 };

	byte _number;
	byte _previousNumber = 0;

public:
	SevSegms();

	void display(byte number);
	void reset();
};

#endif


