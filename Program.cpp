#include "Program.h"

#include "IntroductionState.h"

void Program::init()
{
	_introductionState.setProgram(this);
	_menuState.setProgram(this);
	_programState.setProgram(this);
	_ManualControlState.setProgram(this);

	_introductionState.init();
	_menuState.init();
	_programState.init();
	_ManualControlState.init();

	_stateManager.setState(0, &_introductionState);
	_stateManager.setState(1, &_menuState);
	_stateManager.setState(2, &_programState);
	_stateManager.setState(3, &_ManualControlState);

	_stateManager.changeState(0);
}

void Program::step()
{
	_stateManager.getCurrent()->react();
}

