#include "CSpiderJumpState.h"

CSpiderJumpState::CSpiderJumpState()
{
	type = AISTATE_SPIDERJUMP;
	name = CAIState::stateNames[type];
}

CSpiderJumpState::~CSpiderJumpState()
{
}

void CSpiderJumpState::onInit(CAIBrain *brain, Script &obj)
{
}

void CSpiderJumpState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CSpiderJumpState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CSpiderJumpState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
