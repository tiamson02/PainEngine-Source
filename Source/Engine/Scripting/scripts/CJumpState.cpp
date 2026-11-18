#include "CJumpState.h"

CJumpState::CJumpState()
{
	type = AISTATE_JUMP;
	name = CAIState::stateNames[type];
}

CJumpState::~CJumpState()
{
}

void CJumpState::onInit(CAIBrain *brain, Script &obj)
{
}

void CJumpState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CJumpState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CJumpState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
