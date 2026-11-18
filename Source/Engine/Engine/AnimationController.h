
#include "Core/Math/LeMath.h"

class AnimationParams
{
public:
	ModelAnimation*		Animation;
	float				FrameTime;
	float				Weight;
	float				TimeScale;
	bool				Looped;
	bool				StartAfterBlend;

	float				FrameBlendTime;
	float				BlendTime;

	AnimationParams()
	{
	}

	AnimationParams(ModelAnimation*	animation, bool looped, bool startAfterBlend, float timeScale, float blendTime) 
		: Animation(animation), BlendTime(blendTime), Looped(looped), StartAfterBlend(startAfterBlend), 
		  TimeScale(timeScale), FrameTime(0), FrameBlendTime(0), Weight(0)
	{
	}

    friend FileObject& operator<<(FileObject& file, const AnimationParams& ap)
	{
		return file 
			<< ap.FrameTime << ap.Weight << ap.TimeScale << ap.Looped 
			<< ap.StartAfterBlend << ap.FrameBlendTime << ap.BlendTime;
	}

    friend FileObject& operator>>(FileObject& file, AnimationParams& ap)
	{
		return file 
			>> ap.FrameTime >> ap.Weight >> ap.TimeScale >> ap.Looped 
			>> ap.StartAfterBlend >> ap.FrameBlendTime >> ap.BlendTime;
	}

	Vector AnimationParams::GetMovementVector(const float deltaTime) const;
	void Tick(float timeDelta);
};

class AnimationController
{
public:
	DynamicArray<AnimationParams>	Animations;

	void ActivateAnimation(ModelAnimation *animation, bool looped, bool startAfterBlend, float animScale, float blendTime);

	void SetAnimationTimeScale(ModelAnimation *animation, float timeScale);
	float GetAnimationTimeScale(ModelAnimation *animation);

	void Tick(float timeDelta);

	bool HasAnyAnimations() const;

	float GetAnimationFrameTime(ModelAnimation *animation) const;
	void SetAnimationFrameTime(ModelAnimation *animation, float time);

	Vector GetAnimationMovement(ModelAnimation *animation, float deltaTime) const;
};
