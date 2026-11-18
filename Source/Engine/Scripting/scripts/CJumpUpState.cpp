#include "CJumpUpState.h"

CJumpUpState::CJumpUpState()
{
	type = AISTATE_JUMPUP;
	name = CAIState::stateNames[type];
}

CJumpUpState::~CJumpUpState()
{
}

void CJumpUpState::onInit(CAIBrain *brain, Script &obj)
{
}

void CJumpUpState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CJumpUpState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CJumpUpState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
