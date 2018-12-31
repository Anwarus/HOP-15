#include "Lcd.h"

#include "IntroductionState.h"
#include "MenuState.h"
#include "ProgramState.h"
#include "ManualControlState.h"

Lcd::Lcd()
{
	begin();
}

void Lcd::begin()
{
	_lcd.begin(20, 4);
}

void Lcd::clearLine(byte lineNumber)
{
	for (byte i = 0; i < _cols; i++)
	{
		_lcd.setCursor(i, lineNumber);
		_lcd.print(' ');
	}

	resetPointers(lineNumber);
}

void Lcd::resetPointers(byte lineNumber)
{
	_linesPointers[lineNumber].leftPointer = 0;
	_linesPointers[lineNumber].rightPointer = _cols - 1;
}

void Lcd::clearScreen()
{
	_lcd.clear();
}

void Lcd::writeLine(byte lineNumber, char* text)
{
	if (_textToLeft)
	{
		byte leftPointer = _linesPointers[lineNumber].leftPointer;

		_lcd.setCursor(leftPointer, lineNumber);
		_lcd.print(text);
		_linesPointers[lineNumber].leftPointer = leftPointer + strlen(text);
	}
	else
	{
		byte rightPointer = _linesPointers[lineNumber].rightPointer - strlen(text);

		_lcd.setCursor(rightPointer, lineNumber);
		_lcd.print(text);
		_linesPointers[lineNumber].rightPointer = rightPointer;
	}
}

void Lcd::writeNewLine(byte lineNumber, char* text)
{
	clearLine(lineNumber);
	writeLine(lineNumber, text);
}

void Lcd::manage(MenuState* menuState)
{
	//Line 1
	char* prevName = (char*)menuState->getPrev();
	bool itemFocused = menuState->isItemFocused();

	if (prevName != "" && !itemFocused)
	{
		writeNewLine(0, "  ");
		writeLine(0, prevName);
		changeLeftOrientation(false);
		char* prevNameValue = (char*)menuState->getPrevValue();

		if (prevNameValue != "")
			writeLine(0, prevNameValue);
	}
	else
		writeNewLine(0, "  ");

	changeLeftOrientation(true);

	//Line 2
	char* currentName = (char*)menuState->getCurrent();
	if (currentName != "")
	{
		writeNewLine(1, "* ");
		writeLine(1, currentName);
		changeLeftOrientation(false);

		char* currentNameValue = (char*)menuState->getCurrentValue();

		if (currentNameValue != "")
			writeLine(1, currentNameValue);
	}
	changeLeftOrientation(true);

	//Line 3
	char* nextName = (char*)menuState->getNext();
	if (nextName != "" && !itemFocused)
	{
		writeNewLine(2, "  ");
		writeLine(2, nextName);
		changeLeftOrientation(false);
		char* nextNameValue = (char*)menuState->getNextValue();

		if (nextNameValue != "")
			writeLine(2, nextNameValue);
	}
	else
		writeNewLine(2, "  ");

	changeLeftOrientation(true);

	//Line 4
	writeNewLine(3, menuState->getTip());
}

void Lcd::manage(IntroductionState* introductionState)
{
	writeNewLine(0, introductionState->getCompanyName());
	writeNewLine(1, introductionState->getProgramName());
	writeNewLine(2, introductionState->getMachineAuthor());
	writeNewLine(3, introductionState->getInfo());
}

void Lcd::manage(ProgramState* programState)
{
	if (programState->isFinished())
	{	
		if(!programState->isEnded())
		{ 
			clearScreen();
			writeNewLine(0, "     Powrot do");
			writeNewLine(2, "  pierwszego piora");
		}
		else
		{
			clearScreen();
			writeNewLine(0, "     Zakonczono     ");
			writeNewLine(3, "* by wrocic do menu");
		}	
	}
	else
	{
		byte feather = programState->getCurrentFeather();
		byte feathers = programState->getFeathersCount();
		byte cycle = programState->getCurrentCycle();
		byte cycles = programState->getCyclesCount();
		double cutterAngle = programState->getCutterAngle();

		char cutterLine[6] = { 0 };
		strcpy(cutterLine, "");

		if (cutterAngle - int(cutterAngle) != 0)
			dtostrf(cutterAngle, 2, 2, &cutterLine[strlen(cutterLine)]);
		else
			dtostrf(cutterAngle, 2, 0, &cutterLine[strlen(cutterLine)]);

		char featherLine[20] = { 0 };
		char cycleLine[20] = { 0 };

		sprintf(featherLine, "Pioro nr %d z %d", feather, feathers);
		sprintf(cycleLine, "Cykl nr %d z %d", cycle, cycles);

		writeNewLine(1, featherLine);
		writeNewLine(2, cycleLine);

		if (programState->isPaused())
		{
			writeNewLine(0, "       Pauza");
			writeNewLine(3, "*Wznow #Stop 0-Obroc");
		}	
		else
		{
			//writeNewLine(0, "Przetwarzanie w toku");
			writeNewLine(0, "Ostrzenie ");
			writeLine(0, cutterLine);
			writeNewLine(3, "*-Pauza #-Zatrzymaj");
		}
	}
}

void Lcd::manage(ManualControlState* ManualControlState)
{
	int positionInSteps = ManualControlState->getPositionInSteps();
	int angleOrmm = ManualControlState->getAngleOrmm();
	unsigned int stepCount = ManualControlState->getStepCount();

	bool movingInSteps = ManualControlState->movingInSteps();
	bool changingStepCount = ManualControlState->changingStepCount();
	bool runningDuringProcess = ManualControlState->runningDuringProcess();

	char positionInStepsLine[20] = { 0 };
	char angleOrmmLine[20] = { 0 };
	char stepCountLine[20] = { 0 };
	
	sprintf(positionInStepsLine, "Kroki:%d", positionInSteps);

	if (ManualControlState->getOperation() == "MOVE_DIVIDER_MOTOR")
	{
		sprintf(stepCountLine, "Ruch podzielnicy:%d", stepCount);
		sprintf(angleOrmmLine, "  Stopnie:%d", angleOrmm);
	}
	else if (ManualControlState->getOperation() == "MOVE_TABLE_MOTOR")
	{
		sprintf(stepCountLine, "Ruch stolu:%d", stepCount);
		sprintf(angleOrmmLine, "  mm:%d", angleOrmm);
	}

	writeNewLine(0, stepCountLine);
	writeNewLine(1, positionInStepsLine);
	writeLine(1, angleOrmmLine);

	if (movingInSteps)
		writeNewLine(2, "--------       C    ");
	else
		writeNewLine(2, "    B       --------");

	if (changingStepCount)
		writeNewLine(3, "* by potwierdzic");
	else
	{
		if (!runningDuringProcess)
			writeNewLine(3, "* by wrocic do menu");
		else
			writeNewLine(3, "* by ostrzyc ");
	}	
}

void Lcd::manage(bool direction)
{
	

}