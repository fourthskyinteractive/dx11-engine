#include "AnimationComponent.h"

AnimationInformation::AnimationInformation() : animationFrames(NULL), numBones(0), numFrames(0), inverseBindPose(NULL), currentFrame(0)
{

}

AnimationInformation::AnimationInformation(const AnimationInformation& _animationComponent)
{
	memcpy(this, &_animationComponent, sizeof(AnimationInformation));
}

AnimationInformation::~AnimationInformation()
{

}

void AnimationInformation::Update(float _dt, void* _currentFrameMemoryPointer)
{
	currentAnimationTime += _dt;

	while(currentAnimationTime >= totalAnimationTime)
	{
		currentAnimationTime = 0.0f + (currentAnimationTime - totalAnimationTime);
	}

	//Find what frames I am between
	unsigned int preFrame = (unsigned int)(currentAnimationTime / timePerFrame);
	unsigned int postFrame = preFrame + 1;
	if(postFrame >= numFrames)
	{
		postFrame = 0;
	}

	float lerpPercentage = (currentAnimationTime - (timePerFrame * preFrame)) / timePerFrame;
	LerpAnimationFrames(preFrame, postFrame, lerpPercentage, _currentFrameMemoryPointer);

// 	char* currentBoneMemory = (char*)_currentFrameMemoryPointer;
// 	for(unsigned int i = 0; i < numBones; ++i)
// 	{
// 		memcpy(currentBoneMemory, &animationFrames[preFrame].bones[i], sizeof(XMFLOAT4X4));
// 		currentBoneMemory += sizeof(XMFLOAT4X4);
// 	}
}

void AnimationInformation::LerpAnimationFrames(unsigned int _preFrame, unsigned int _postFrame, float _percentageThroughFrame, void* _currentFrameMemoryPointer)
{
	if( _preFrame < 0 || _preFrame > (numFrames - 1) ||
		_postFrame < 0 || _postFrame > (numFrames - 1))
		return;

	XMVECTOR preFrameRotation;
	XMVECTOR postFrameRotation;
	XMVECTOR lerpedQuaternionRotation;

	XMVECTOR prePosition;
	XMVECTOR postPosition;

	char* currentBoneMemory = (char*)_currentFrameMemoryPointer;
	for(unsigned int i = 0; i < numBones; ++i)
	{
		preFrameRotation = animationFrames[_preFrame].rotationQuaternions[i];
		postFrameRotation = animationFrames[_postFrame].rotationQuaternions[i];
		lerpedQuaternionRotation = XMQuaternionSlerp(preFrameRotation, postFrameRotation, _percentageThroughFrame);

		prePosition = animationFrames[_preFrame].positions[i];
		postPosition = animationFrames[_postFrame].positions[i];
		XMVectorLerp(prePosition, postPosition, _percentageThroughFrame);
		XMFLOAT4 lerpedPositionF;
		XMStoreFloat4(&lerpedPositionF, XMVectorLerp(prePosition, postPosition, _percentageThroughFrame));
		lerpedPositionF.w = 1.0f;

		XMMATRIX finalMatrixM = XMMatrixRotationQuaternion(lerpedQuaternionRotation);
		XMFLOAT4X4 finalMatrixF;
		XMStoreFloat4x4(&finalMatrixF, finalMatrixM);
		memcpy(&finalMatrixF._41, &lerpedPositionF, sizeof(XMFLOAT4));
		memcpy(currentBoneMemory, &finalMatrixF, sizeof(XMFLOAT4X4));
		currentBoneMemory += sizeof(XMFLOAT4X4);
	}
}

AnimationComponent::AnimationComponent()
{
	animationCount = 0;
	animations.clear();
	currentAnimation = -1;
	currentFrameMemoryPointer = NULL;
	currentAnimationIBPMemoryPointer = NULL;
}

AnimationComponent::AnimationComponent(const AnimationComponent& _animationComponent)
{

}

AnimationComponent::~AnimationComponent()
{
	for(unsigned int i = 0; i < animationCount; ++i)
	{
		delete animations[i];
		animations[i] = NULL;
		animationCount--;
	}
}

void AnimationComponent::AddAnimation(AnimationInformation* _animationInformation)
{
	_animationInformation->animationType = ANIMATION_IDLE;
	animations.push_back(_animationInformation);
}

void AnimationComponent::RemoveAnimation(ANIMATION_TYPE _animationType)
{
	for(vector<AnimationInformation*>::iterator currAnimation = animations.begin(); currAnimation != animations.end(); currAnimation++)
	{
		if((*currAnimation)->animationType == _animationType)
		{
			animations.erase(currAnimation);
		}
	}
}

void AnimationComponent::ChangeAnimation(ANIMATION_TYPE _animationType)
{
	for(unsigned int i = 0; i < animations.size(); ++i)
	{
		if(animations[i]->animationType == _animationType)
		{
			currentAnimation = i;
			currentAnimationIBPMemoryPointer = malloc(sizeof(XMFLOAT4X4) * animations[i]->numBones);
			currentFrameMemoryPointer = malloc(sizeof(XMFLOAT4X4) * animations[i]->numBones);
			memcpy(currentAnimationIBPMemoryPointer, animations[i]->inverseBindPose, sizeof(XMFLOAT4X4) * animations[i]->numBones);
			memcpy(currentFrameMemoryPointer, animations[i]->animationFrames[0].bones, sizeof(XMFLOAT4X4) * animations[i]->numBones);
			//memcpy(currentAnimationIBPMemoryPointer, animations[i]->inverseBindPose, sizeof(XMFLOAT4X4) * animations[i]->numBones);
			//memcpy(currentFrameMemoryPointer, animations[currentAnimation]->animationFrames[0].bones, sizeof(XMFLOAT4X4) * animations[i]->numBones);
		}
	}
}

void AnimationComponent::Update(float _dt)
{
	animations[currentAnimation]->Update(_dt, currentFrameMemoryPointer);
}