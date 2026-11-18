#include "CFarAttackState.h"

CFarAttackState::CFarAttackState()
{
	type = AISTATE_FARATTACK;
	name = CAIState::stateNames[type];
}

CFarAttackState::~CFarAttackState()
{
}

void CFarAttackState::onInit(CAIBrain *brain, Script &obj)
{
}

void CFarAttackState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CFarAttackState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CFarAttackState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
