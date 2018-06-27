#include "StepperMotor.h"
#include "Endstop.h"

StepperMotor::StepperMotor(byte dirPin, byte stepPin, byte enablePin, Endstop* endstop)
{
	_dirPin = dirPin;
	_stepPin = stepPin;
	_enablePin = enablePin;

	_endstop = endstop;

	_stepper = new Stepper(_motorSteps, _dirPin, _stepPin);
	_stepper->setSpeed(_speed);

	pinMode(_enablePin, OUTPUT);
	enable(true);
}

StepperMotor::~StepperMotor()
{
	delete _stepper;
	delete _endstop;
}

void StepperMotor::setMotorSteps(byte motorSteps)
{
	_motorSteps = motorSteps;
}

void StepperMotor::setMotorSpeed(byte speed)
{
	_speed = speed;
	_stepper->setSpeed(_speed);
}

void StepperMotor::setMicroSteps(byte microSteps)
{
	_microSteps = microSteps;
}

void StepperMotor::enable(bool e)
{
	if (e)
		digitalWrite(_enablePin, LOW);
	else
		digitalWrite(_enablePin, HIGH);

	_isEnable = e;
}

void StepperMotor::home()
{
	if (_endstop != nullptr)
	{
		while (!_endstop->isClicked())
			rotate(-5);
	}
}

void StepperMotor::rotate(int angle)
{
	float stepsToRotate = angle * _motorSteps * _microSteps / 360;

	_stepper->step(stepsToRotate);
}

void StepperMotor::manage(ProgramState* ProgramState)
{
	if(ProgramState->canChangeFeather())
	{
		ProgramState->setRotatedInCycle(true);

		byte feather = ProgramState->getCurrentFeather();
		byte feathers = ProgramState->getFeathersCount();
		byte cycle = ProgramState->getCurrentCycle();
		byte cycles = ProgramState->getCyclesCount();
		int rotateAngle = ProgramState->getRotateAngle();

		//delay(1000);
		rotate(rotateAngle);

		feather++;
		if (feather > feathers)
		{
			feather = 1;
			cycle++;

			if (cycle > cycles)
				ProgramState->finish();
			else
			{
				ProgramState->setCurrentFeather(feather);
				ProgramState->setCurrentCycle(cycle);
			}
		}
		else
			ProgramState->setCurrentFeather(feather);
	}
}