#include "SetValueElement.h"

#include "MenuState.h"
#include "Program.h"
#include "DeviceManager.h"

#include "Lcd.h"
#include "SimpleKeypad.h"

SetValueElement::SetValueElement(char* name, MenuState* state, double minValue, double maxValue, double currentValue, double stepValue) : Element(name)
{
	_state = state;

	DeviceManager* deviceManager = state->getProgram()->getDeviceManager();

	_lcd = deviceManager->requestLcd();
	_simpleKeypad = deviceManager->requestSimpleKeypad();
	_buzzer = deviceManager->requestBuzzer();
	_rotaryEncoder = deviceManager->requestRotaryEncoder();

	_minValue = minValue;
	_maxValue = maxValue;
	_currentValue = currentValue;
	_stepValue = stepValue;
}

void SetValueElement::react()
{
	char key = _simpleKeypad->getPressedKey();

	short value = _rotaryEncoder->read();

	bool decimalPartKey = false;

	if(key != KEY_NONE)
		_buzzer->playOnPress();

	if (key == KEY_UP || value == 1)
		increase();
	else if (key == KEY_DOWN || value == -1)
		decrease();
	else if (key == KEY_ENTER)
		_state->back();

	if (key == KEY_0)
		_manualKeyValue = 0;
	else if (key == KEY_1)
		_manualKeyValue = 1;
	else if (key == KEY_2)
		_manualKeyValue = 2;
	else if (key == KEY_3)
		_manualKeyValue = 3;
	else if (key == KEY_4)
		_manualKeyValue = 4;
	else if (key == KEY_5)
		_manualKeyValue = 5;
	else if (key == KEY_6)
		_manualKeyValue = 6;
	else if (key == KEY_7)
		_manualKeyValue = 7;
	else if (key == KEY_8)
		_manualKeyValue = 8;
	else if (key == KEY_9)
		_manualKeyValue = 9;
	else if (key == KEY_RETURN)
		decimalPartKey = true;
	else
		_manualKeyValue = -1;

	if (_manualKeyValue != -1)
		_state->needRedraw();

	if (_manualKeyValue == 0 && _manualInput)
		if (!_decimalPart)
			_currentValue *= 10;
		else
			_currentDecimalNumberIndex++;

	else if (_manualInput && decimalPartKey)
		_decimalPart = true;
	else if (_manualKeyValue > 0)
	{
		if (!_manualInput)
		{
			_currentValue = 0;
			_manualInput = true;
		}

		if (!_decimalPart)
		{
			_currentValue *= 10;
			_currentValue += _manualKeyValue;
		}
		else
		{
			_currentValue += pow(10, _currentDecimalNumberIndex * -1) * _manualKeyValue;
			_currentDecimalNumberIndex++;
		}
		
	}

	//If during decimal input we hit max decimal numbers count accept value and go back to menu
	if(_currentDecimalNumberIndex > _maxDecimalNumbers)
		_state->back();
		
}

void SetValueElement::increase()
{
	_manualInput = false;
	_currentValue += _stepValue;

	if (_currentValue > _maxValue)
		_currentValue = _maxValue;

	_state->needRedraw();
}

void SetValueElement::decrease()
{
	_manualInput = false;
	_currentValue -= _stepValue;

	if (_currentValue < _minValue)
		_currentValue = _minValue;

	_state->needRedraw();
}

void SetValueElement::init()
{
	_manualInput = false;
	_manualKeyValue = -1;
}

void SetValueElement::validate()
{
	if (_currentValue > _maxValue)
		_currentValue = _maxValue;

	if (_currentValue < _minValue)
		_currentValue = _minValue;
}