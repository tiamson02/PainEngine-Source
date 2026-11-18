#include "CRunToPlayerState.h"

CRunToPlayerState::CRunToPlayerState()
{
	type = AISTATE_RUNTOPLAYER;
	name = CAIState::stateNames[type];
}

CRunToPlayerState::~CRunToPlayerState()
{
}

void CRunToPlayerState::onInit(CAIBrain *brain, Script &obj)
{
}

void CRunToPlayerState::onUpdate(CAIBrain *brain, Script &obj)
{
}

void CRunToPlayerState::onRelease(CAIBrain *brain, Script &obj)
{
}

float CRunToPlayerState::evaluate(CAIBrain *brain, Script &obj)
{
	return 0;
}
