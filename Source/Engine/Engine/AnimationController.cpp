#include "Engine.h"
#include "Engine/ModelAnimation.h"
#include "Engine/AnimationController.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

bool AnimationController::HasAnyAnimations() const
{ 
	return Animations.Size()>0; 
}

void AnimationController::ActivateAnimation(ModelAnimation *animation, bool looped, bool startAfterBlend, float animScale, float blendTime) 
{ 
	if (!animation)
	{
		Animations.ClearArray();
		return;
	}

	int id = -1;

	for (int iAnim = 0; iAnim<Animations.Size(); iAnim++)
		if (Animations[iAnim].Animation == animation) 
		{
			id = iAnim;
			break;
		}

	if (id==-1) 
	{
		Animations.Insert(AnimationParams(animation, looped, false, animScale, blendTime), 0);
	}
	else 
	{
		AnimationParams aParams;

		if (!looped) // restart
			aParams = AnimationParams(animation, looped, false, animScale, blendTime);
		else
			aParams = Animations[id];

		Animations.Remove(id);
		Animations.Insert(aParams, 0);
	}

	if (startAfterBlend)
		for (int iAnim=1; iAnim<Animations.Size(); iAnim++)
			Animations[iAnim].StartAfterBlend = true;

	if (Animations.Size()==1)
		Animations[0].Weight = 1;

//	GLog.Print("%s!\n", animation->Name.AsChar());
}

float AnimationController::GetAnimationFrameTime(ModelAnimation *animation) const
{
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
	{
		if (Animations[iAnim].Animation == animation)
		{
			return Animations[iAnim].FrameTime;
		}
	}

	return 0.0f;
}

void AnimationController::SetAnimationFrameTime(ModelAnimation *animation, float time)
{
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
	{
		if (Animations[iAnim].Animation == animation)
		{
			Animations[iAnim].FrameTime = time;
			return;
		}
	}
}

Vector AnimationController::GetAnimationMovement(ModelAnimation *animation, float deltaTime) const
{
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
	{
		if (Animations[iAnim].Animation == animation)
		{
			return Animations[iAnim].GetMovementVector(deltaTime);
		}
	}

	return Vector(0,0,0);
}

void AnimationController::SetAnimationTimeScale(ModelAnimation *animation, float timeScale)
{
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
	{
		if (Animations[iAnim].Animation == animation)
		{
			Animations[iAnim].TimeScale = timeScale;
			return;
		}
	}
}

float AnimationController::GetAnimationTimeScale(ModelAnimation *animation)
{
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
	{
		if (Animations[iAnim].Animation == animation)
		{
			return Animations[iAnim].TimeScale;
		}
	}
	return 1.0f;
}

void AnimationController::Tick(float timeDelta)
{
	int size = Animations.Size();

	for (int iAnim=0; iAnim<size; iAnim++)
	{
		Animations[iAnim].Tick(timeDelta);
	}

	if (size == 1) return;

	float weightDelta = 1;
	if (Animations[0].BlendTime>0)
		weightDelta = timeDelta / Animations[0].BlendTime;

	Animations[0].Weight += weightDelta;

	if (Animations[0].Weight > 1.0f) 
	{
		Animations[0].Weight = 1.0f;

		while (Animations.Size()!=1)
			Animations.FastRemove(1);
	}
	else
	{
		float totalWeight = Animations[0].Weight;

		for (int iAnim=2; iAnim<Animations.Size();)
		{
			Animations[iAnim].Weight -= weightDelta;

			if (Animations[iAnim].Weight < 0.0f) 
				Animations.Remove(iAnim);
			else 
			{
				totalWeight += Animations[iAnim].Weight;
				iAnim ++;
			}
		}

		Animations[1].Weight = 1.0f - totalWeight;
	}
/*
	for (int iAnim=0; iAnim<Animations.Size(); iAnim++)
		GLog.Print("%s %f ", Animations[iAnim].Animation->Name.AsChar(), Animations[iAnim].Weight);
	GLog.Print("\n");
*/
}

void AnimationParams::Tick(float timeDelta)
{
	if (StartAfterBlend && Weight<1) {
		// do not update FrameTime until Weight reaches 1 if StartAfterBlend 
		return;
	}

	FrameTime		+= timeDelta*TimeScale;

	if (Looped)
	{
		FrameTime = fmodf(FrameTime+Animation->Time, Animation->Time);
	}
	else if (FrameTime > Animation->Time)
	{
		FrameTime = Animation->Time;
	}
}

Vector AnimationParams::GetMovementVector(const float deltaTime) const
{
	return Animation->GetMovement(FrameTime+deltaTime*TimeScale) - Animation->GetMovement(FrameTime);
}
