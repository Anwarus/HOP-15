#include "program.h"

Program program = Program();

void setup() 
{
	//Date rate in bits per seconds for serial data transmission
	Serial.begin(9600);
}

void loop() 
{
	program.step();
}