#include "DcMotor.h"

#include "Endstop.h"
#include "Potentiometer.h"

DcMotor::DcMotor(Endstop* endstop, Potentiometer* potentiometer)
{
	_endstop = endstop;
	_potentiometer = potentiometer;
	
	pinMode(_pwmPin, OUTPUT);
	pinMode(_dirPinA, OUTPUT);
	pinMode(_dirPinB, OUTPUT);
	pinMode(_enablePin, OUTPUT);

	enable(false);
}

void DcMotor::setSpeed()
{
	_potentiometer->setValue();
	_speed = _potentiometer->getValue();
	
	analogWrite(_pwmPin, _speed);
}

void DcMotor::enable(bool e)
{
	if (e)
		digitalWrite(_enablePin, HIGH);
	else
	{
		stop();
		digitalWrite(_enablePin, LOW);
	}
}

bool DcMotor::home()
{
	if (_endstop->isClicked())
	{
		stop();
		return true;
	}
	else
	{
		moveBackward();
		return false;
	}
}

void DcMotor::moveBackward()
{
	//Start moving backward
	digitalWrite(_dirPinA, HIGH);
	digitalWrite(_dirPinB, LOW);

	//Move for given time
	delay(_moveTime);

	stop();
}

void DcMotor::moveForward()
{
	//Start moving forward
	digitalWrite(_dirPinA, LOW);
	digitalWrite(_dirPinB, HIGH);

	//Move for given time
	delay(_moveTime);

	stop();
}

void DcMotor::stop()
{
	//Disable pins responsible for moving
	digitalWrite(_dirPinA, LOW);
	digitalWrite(_dirPinB, LOW);
	
	//Set speed to zero
	analogWrite(_pwmPin, 0);
}
