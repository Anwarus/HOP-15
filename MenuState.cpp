#include "MenuState.h"

#include "Program.h"
#include "DeviceManager.h"

#include "Lcd.h"
#include "SimpleKeypad.h"
#include "Buzzer.h"
#include "lib/A4988.h"
#include "RotaryEncoder.h"

#include "ProgramState.h"
#include "SetValueElement.h"
#include "SetByteElement.h"
#include "SetFloatElement.h"
#include "EncoderState.h"

bool MenuState::setElement(byte index, char* description)
{
	if (index >= 0 && index < _itemsCount)
	{
		_itemNames[index] = description;
		return true;
	}

	return false;
}

bool MenuState::setElement(byte index, SetValueElement* element)
{
	if (index >= 0 && index < _itemsCount)
	{
		_itemNames[index] = element->getName();

		_itemBinds[index].index = index;
		_itemBinds[index].item = element;

		return true;
	}

	return false;
}

void MenuState::init()
{
	_featherAmount = SetByteElement("Piora", this, 2, 32, 6, 1);
	_cycleAmount = SetByteElement("Cykle", this, 1, 16, 1, 1);
	_speed = SetByteElement("Predkosc", this, 5, 100, 30, 5);

	_cutterAngle = SetFloatElement("Kat obrotu", this, 1.0, 60.0, 24.0, 0.1);

	DeviceManager* deviceManager = _program->getDeviceManager();

	_lcd = deviceManager->requestLcd();
	_simpleKeypad = deviceManager->requestSimpleKeypad();
	_buzzer = deviceManager->requestBuzzer();
	_dividerMotor = deviceManager->requestDividerMotor();
	_tableMotor = deviceManager->requestTableMotor();
	_rotaryEncoder = deviceManager->requestRotaryEncoder();

	setElement(0, &_featherAmount);
	setElement(1, &_cycleAmount);
	setElement(2, "Rozpocznij");
	setElement(3, "Test podzielnicy");
	setElement(4, "Test stolu");
	setElement(5, "Podziel. szybko");
	setElement(6, "Podziel. wolno");
	setElement(7, "Stol szybko");
	setElement(8, "Stol wolno");
	setElement(9, &_speed);
	setElement(10, &_cutterAngle);
	setElement(11, "Wylacz silniki");
}

void MenuState::react()
{
	if (_isFocused && _itemBinds[_selectedIndex].index != -1)
		_itemBinds[_selectedIndex].item->react();
	else
	{
		char key = _simpleKeypad->getPressedKey();

		if (key != KEY_NONE)
			_buzzer->playOnPress();

		if (key == KEY_UP)
			up();
		else if (key == KEY_DOWN)
			down();
		else if (key == KEY_ENTER)
			enter();
	}

	if (_disrupted)
	{
		_lcd->begin();
		_needRedraw = true;
		_disrupted = false;
	}

	if (_needRedraw)
	{
		_lcd->manage(this);
		_needRedraw = false;
	}
}

void MenuState::reset()
{

}

void MenuState::up()
{
	if (_selectedIndex == 0)
		_selectedIndex = _itemsCount - 1;
	else
		_selectedIndex--;

	_needRedraw = true;
}

void MenuState::down()
{
	if (_selectedIndex == _itemsCount - 1)
		_selectedIndex = 0;
	else
		_selectedIndex++;

	_needRedraw = true;
}

void MenuState::enter()
{
	if (_itemBinds[_selectedIndex].index != -1)
		_isFocused = !_isFocused;

	//If we starting new program
	if (_selectedIndex == 2)
	{
		ProgramState* programState = getProgram()->getProgramState();
		programState->setFeathers(reinterpret_cast<SetByteElement*>(_itemBinds[0].item)->getValue());
		programState->setCycles(reinterpret_cast<SetByteElement*>(_itemBinds[1].item)->getValue());
		programState->setCutterAngle(reinterpret_cast<SetFloatElement*>(_itemBinds[10].item)->getValue());
		programState->reset();

		getProgram()->getStateManager()->changeState(2);
	}
	//If we testing divider
	else if (_selectedIndex == 3)
	{
		ProgramState* programState = getProgram()->getProgramState();

		programState->setFeathers(reinterpret_cast<SetByteElement*>(_itemBinds[0].item)->getValue());
		programState->setCycles(reinterpret_cast<SetByteElement*>(_itemBinds[1].item)->getValue());
		programState->setCutterAngle(reinterpret_cast<SetFloatElement*>(_itemBinds[10].item)->getValue());
		//programState->setCutterAngle(getProgram()->getEncoderState()->getCutterAngle());
		programState->reset();
		programState->testDividerMotor();

		getProgram()->getStateManager()->changeState(2);
	}
	//If we testing table
	else if (_selectedIndex == 4)
	{
		ProgramState* programState = getProgram()->getProgramState();

		programState->setFeathers(reinterpret_cast<SetByteElement*>(_itemBinds[0].item)->getValue());
		programState->setCycles(reinterpret_cast<SetByteElement*>(_itemBinds[1].item)->getValue());
		programState->setCutterAngle(reinterpret_cast<SetFloatElement*>(_itemBinds[10].item)->getValue());
		//programState->setCutterAngle(getProgram()->getEncoderState()->getCutterAngle());
		programState->reset();
		programState->testTableMotor();

		getProgram()->getStateManager()->changeState(2);
	}
	//If we moving divider quickly
	else if (_selectedIndex == 5)
	{
		_rotaryEncoder->setOperationType(RotaryEncoder::QUICKLY_MOVE);

		EncoderState* encoderState = getProgram()->getEncoderState();
		//encoderState->setOperation(EncoderState::MOVING_DIVIDER_MOTOR);
		encoderState->reset();
		encoderState->setOperation(MOVE_DIVIDER_MOTOR);

		getProgram()->getStateManager()->changeState(3);
	}
	//If we moving divider slowly
	else if (_selectedIndex == 6)
	{
		_rotaryEncoder->setOperationType(RotaryEncoder::SLOWLY_MOVE);

		EncoderState* encoderState = getProgram()->getEncoderState();
		encoderState->reset();
		encoderState->setOperation(MOVE_DIVIDER_MOTOR);

		getProgram()->getStateManager()->changeState(3);
	}
	//If we moving table quickly
	else if (_selectedIndex == 7)
	{
		_rotaryEncoder->setOperationType(RotaryEncoder::QUICKLY_MOVE);

		EncoderState* encoderState = getProgram()->getEncoderState();
		encoderState->reset();
		encoderState->setOperation(MOVE_TABLE_MOTOR);

		getProgram()->getStateManager()->changeState(3);
	}
	//If we moving table slowly
	else if (_selectedIndex == 8)
	{
		_rotaryEncoder->setOperationType(RotaryEncoder::SLOWLY_MOVE);

		EncoderState* encoderState = getProgram()->getEncoderState();
		encoderState->reset();
		encoderState->setOperation(MOVE_TABLE_MOTOR);

		getProgram()->getStateManager()->changeState(3);
	}
	//If we want to disable motors
	else if (_selectedIndex == 11)
	{
		_dividerMotor->disable();
		_tableMotor->disable();
	}

	_needRedraw = true;
}

const char* MenuState::getNext()
{
	byte index;

	if (_selectedIndex == _itemsCount - 1)
		index = 0;
	else
		index = _selectedIndex + 1;

	return _itemNames[index].c_str();
}

const char* MenuState::getCurrent()
{
	char valueBuf[20] = { 0 };
	_itemNames[_selectedIndex].toCharArray(valueBuf, 20);

	return _itemNames[_selectedIndex].c_str();
}

const char* MenuState::getPrev()
{
	byte index;

	if (_selectedIndex == 0)
		index = _itemsCount - 1;
	else
		index = _selectedIndex - 1;

	return _itemNames[index].c_str();
}

const char* MenuState::getNextValue()
{
	byte index;

	if (_selectedIndex == _itemsCount - 1)
		index = 0;
	else
		index = _selectedIndex + 1;

	if (_itemBinds[index].index != -1)
		return _itemBinds[index].item->getValueStr().c_str();

	return "";
}

const char* MenuState::getCurrentValue()
{
	if (_itemBinds[_selectedIndex].index != -1)
		return _itemBinds[_selectedIndex].item->getValueStr().c_str();

	return "";
}

const char* MenuState::getPrevValue()
{
	byte index;

	if (_selectedIndex == 0)
		index = _itemsCount - 1;
	else
		index = _selectedIndex - 1;

	if (_itemBinds[index].index != -1)
		return _itemBinds[index].item->getValueStr().c_str();

	return "";
}

char* MenuState::getTip()
{
	return "A-Gora D-Dol *-Enter";
}

//float MenuState::getValueAtIndex(byte index)
//{
//	if (_itemBinds[index].index != -1)
//		return _itemBinds[index].item->getValue();
//	else
//		return NULL;
//}