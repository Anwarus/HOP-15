#include "program.h"
#include <Arduino.h>

Program program;

void setup()
{
	program.init();
}

void loop()
{
	program.step();
}