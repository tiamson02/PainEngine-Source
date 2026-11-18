#include "CxxxState.h"

CxxxState::CxxxState()
{
	type = AISTATE_XXX;
	name = CAIState::stateNames[type];
}

CxxxState::~CxxxState()
{
}

void CxxxState::onInit(CAIBrain *brain, Script &obj)
{
}

void CxxxState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CxxxState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CxxxState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
