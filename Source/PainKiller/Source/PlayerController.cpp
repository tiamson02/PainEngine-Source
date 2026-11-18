#include "Engine.h"
#include "Engine/PCFSystem.h"
#include "Engine/BaseGame.h"
#include "PlayerController.h"

PlayerController::PlayerController()
:   MoveType(EMovePhysics)
{
}

void PlayerController::Action(const DWORD )
{
}

void PlayerController::Rotate(const Vector& rotation)
{
    mCamera.Rotation+=rotation;
    // clamp vertical rotation
    if(mCamera.Rotation.Y>+80.0f) mCamera.Rotation.Y=+80.0f;
    if(mCamera.Rotation.Y<-80.0f) mCamera.Rotation.Y=-80.0f;
}
