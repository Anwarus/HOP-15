#include "RotaryEncoder.h"

#include "MenuState.h"
#include "Program.h"
#include "DeviceManager.h"
#include "SimpleKeypad.h"

RotaryEncoder::RotaryEncoder()
{
	digitalWrite(_pinA, INPUT_PULLUP);
	digitalWrite(_pinB, INPUT_PULLUP);
}

void RotaryEncoder::init(MenuState* state)
{
	_state = state;

	DeviceManager* deviceManager = state->getProgram()->getDeviceManager();

	_simpleKeypad = deviceManager->requestSimpleKeypad();
}

void RotaryEncoder::setOperationType(OperationType operationType)
{
	switch (operationType)
	{
	case SLOWLY_MOVE:
		_changeAmount = 1;
		break;
	case QUICKLY_MOVE:
		_changeAmount = 200;
		break;
	case SPEED_CHANGE:
		_changeAmount = 5;
		break;
	}
}

short RotaryEncoder::getValue()
{
	_currentTime = millis();
	_lastTime = _currentTime;

	if (_currentTime >= _lastTime + 5)
	{
		_encA = digitalRead(_pinA);
		_encB = digitalRead(_pinB);

		if ((!_encA) && (_lastA))
		{
			if (_encB)
				return _changeAmount;
			else
				return _changeAmount * -1;
		}
		else
			return 0;
	}

	_lastA = _encA;
	_lastTime = _currentTime;

	char key = _simpleKeypad->getPressedKey();

	if (key == KEY_ENTER)
		_state->back();
}