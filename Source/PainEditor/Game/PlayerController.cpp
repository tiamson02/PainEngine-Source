#include "stdafx.h"
#include "Engine.h"
#include "PlayerController.h"

PlayerController::PlayerController()
:   MoveType(EMovePhysics)
{
}

void PlayerController::Action(const DWORD action)
{
}

void PlayerController::Rotate(const Vector& rotation)
{
    mCamera.Rotation+=rotation;
    // clamp vertical rotation
    if(mCamera.Rotation.Y>+80.0f) mCamera.Rotation.Y=+80.0f;
    if(mCamera.Rotation.Y<-80.0f) mCamera.Rotation.Y=-80.0f;
}
