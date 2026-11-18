#include "CCloseAttackState.h"

CCloseAttackState::CCloseAttackState()
{
	type = AISTATE_CLOSEATTACK;
	name = CAIState::stateNames[type];
}

CCloseAttackState::~CCloseAttackState()
{
}

void CCloseAttackState::onInit(CAIBrain *brain, Script &obj)
{
}

void CCloseAttackState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CCloseAttackState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CCloseAttackState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
