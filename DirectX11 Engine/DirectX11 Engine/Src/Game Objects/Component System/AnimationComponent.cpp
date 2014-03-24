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

	if(currentAnimationTime > .05f)
	{
		currentFrame++;
		if(currentFrame >= numFrames)
			currentFrame = 0;

		currentAnimationTime = 0.0f;
		memcpy(_currentFrameMemoryPointer, animationFrames[currentFrame].bones, sizeof(XMFLOAT4X4) * numBones);
	}

	//Find what frames I am between
	//unsigned int preFrame = (unsigned int)(currentAnimationTime / timePerFrame);
	//unsigned int postFrame = preFrame + 1;
	//float lerpPercentage = (currentAnimationTime - (timePerFrame * preFrame)) / timePerFrame;
	//LerpAnimationFrames(preFrame, postFrame, lerpPercentage, _currentFrameMemoryPointer);
}

void AnimationInformation::LerpAnimationFrames(unsigned int _preFrame, unsigned int _postFrame, float _percentageThroughFrame, void* _currentFrameMemoryPointer)
{
	if( _preFrame < 0 || _preFrame > (numBones - 1) ||
		_postFrame < 0 || _postFrame > (numBones - 1))
		return;

	XMMATRIX preFrame;
	XMMATRIX postFrame;
	XMMATRIX lerpedMatrixM;

	preFrame = XMLoadFloat4x4(animationFrames[_preFrame].bones);
	postFrame = XMLoadFloat4x4(animationFrames[_postFrame].bones);

	lerpedMatrixM = (preFrame + ((postFrame - preFrame) * _percentageThroughFrame));

	XMFLOAT4X4 lerpedMatrix;
	XMStoreFloat4x4(&lerpedMatrix, lerpedMatrixM);
	memcpy(_currentFrameMemoryPointer, &lerpedMatrix, sizeof(XMFLOAT4X4));
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