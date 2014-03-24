#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <vector>
using namespace std;

#include "GameObjectComponent.h"
#include "../../Renderer/D3D11Renderer.h"

enum ANIMATION_TYPE{ANIMATION_IDLE = 0, ANIMATION_WALK};

struct FlattenedSkeleton
{
	XMFLOAT4X4* bones;

	FlattenedSkeleton() : bones(NULL){}
};

class AnimationInformation
{
public:
	AnimationInformation();
	AnimationInformation(const AnimationInformation& _animationComponent);
	~AnimationInformation();
	void Update(float _dt, void* _currentFrameMemoryPointer);
	void LerpAnimationFrames(unsigned int _preFrame, unsigned int _postFrame, float _percentageThroughFrame, void* _currentFrameMemoryPointer);

	ANIMATION_TYPE animationType;
	unsigned int numBones;
	unsigned int numFrames;
	FlattenedSkeleton* animationFrames;
	XMFLOAT4X4* inverseBindPose;

	unsigned int currentFrame;
	float timePerFrame;
	float totalAnimationTime;
	float currentAnimationTime;
};

class AnimationComponent : public GameObjectComponent
{
public:
	AnimationComponent();
	AnimationComponent(const AnimationComponent& _animationComponent);
	~AnimationComponent();
	void AddAnimation(AnimationInformation* _animationInformation);
	void RemoveAnimation(ANIMATION_TYPE _animationType);
	void ChangeAnimation(ANIMATION_TYPE _animationType);
	void SetCurrentAnimation(int _currentAnimation){currentAnimation = _currentAnimation;}
	void Update(float _dt);

	//Getters
	void* GetCurrentFrameMemoryPointer(){return currentFrameMemoryPointer;}
	void* GetCurrentAnimationIBPMemoryPointer(){return currentAnimationIBPMemoryPointer;}
	vector<AnimationInformation*> GetAnimations(){return animations;}

private:
	unsigned int animationCount;
	int currentAnimation;

	vector<AnimationInformation*> animations;
	void* currentFrameMemoryPointer;
	void* currentAnimationIBPMemoryPointer;
};

#endif