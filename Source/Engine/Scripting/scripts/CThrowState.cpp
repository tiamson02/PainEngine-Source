#include "CThrowState.h"

CThrowState::CThrowState()
{
	type = AISTATE_THROW;
	name = CAIState::stateNames[type];
}

CThrowState::~CThrowState()
{
}

void CThrowState::onInit(CAIBrain *brain, Script &obj)
{
}

void CThrowState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CThrowState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CThrowState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
