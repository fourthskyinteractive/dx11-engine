#ifndef CONSTANTBUFFERUPDATEFUNCTIONS_H
#define CONSTANTBUFFERUPDATEFUNCTIONS_H

#include "../../Game/Game.h"
#include "../WorldObject.h"
#include "../Lights/LightManager.h"


//class ConstantBufferUpdateFunctions
//{
//public:
//	static void* CBUpdateViewMatrix(){return &Game::camera->GetViewMatrixF();}
//	static void* CBUpdateProjectionMatrix(){return &Game::camera->GetProjectionMatrixF();}
//}

static void* CBUpdateViewMatrix(){return &Game::camera->GetViewMatrixF();}
static void* CBUpdateProjectionMatrix(){return &Game::camera->GetProjectionMatrixF();}
static void* CBUpdateWorldMatrix(){return &((WorldObject*)Game::baseObject)->GetWorldMatrixF();}
static void* CBUpdateNumPointLights(){return &LightManager::numPointLights;}
static void* CBUpdateNumDirectionalLights(){return &LightManager::numDirectionalLights;}

#endif