#include "ObjectManager.h"
#include "FBXModel.h"
#include "Lights/LightManager.h"
#include "../Camera/CameraManager.h"
#include "../Renderer/TextureManager.h"

#include "WorldObject.h"
#include "ScreenSpaceObject.h"

vector<GameObject*>		ObjectManager::dynamicObjects;
vector<GameObject*>		ObjectManager::staticObjects;
BaseObject*				ObjectManager::finalPassObject;

ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::Initialize()
{
	finalPassObject = new ScreenSpaceObject();

	//TODO:: WORK ON THE COMPUTE LIGHTS!
	XMFLOAT4 pos = XMFLOAT4(0.0f, 0.0f, 5.0f, 1.0f);
	for(int i = 1; i < 6; ++i)
	{
		finalPassObject->AddTexture(D3D11Renderer::shaderResourceView[i]);
	}
	finalPassObject->AddBaseComponent(RENDER_COMPONENT);
	finalPassObject->AddRenderComponent(VERTEX_BUFFER_RENDER_COMPONENT);
	finalPassObject->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &pos, sizeof(XMFLOAT4), sizeof(XMFLOAT4));
	finalPassObject->AddRenderComponent(CONSTANT_BUFFER_RENDER_COMPONENT);
	finalPassObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &LightManager::numLights, sizeof(XMFLOAT4), LightManager::GetNumberOfLightsMemory());
	finalPassObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, (*CameraManager::currentCamera)->GetWidthHeightNearFarP(), sizeof(XMFLOAT4), (*CameraManager::currentCamera)->GetWidthHeightNearFarP());
	finalPassObject->AddConstantBufferComponent(VIEW_MATRIX_COMPONENT, (*CameraManager::currentCamera)->GetViewMatrixP(), sizeof(XMFLOAT4X4), (*CameraManager::currentCamera)->GetViewMatrixP());
	finalPassObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, (*CameraManager::currentCamera)->GetFrustumExtentsP(), sizeof(XMFLOAT4), (*CameraManager::currentCamera)->GetFrustumExtentsP());
	finalPassObject->AddComputeShaderBuffer(LightManager::GetPointLightsMemory(), sizeof(PointLightCompressed), sizeof(PointLightCompressed) * LightManager::GetNumberPointLights());
	finalPassObject->SetShaders(1, 0, 1, 0);
	finalPassObject->FinalizeObject();
}

void ObjectManager::AddObject(OBJECT_TYPE _objectType, string _modelPath, XMFLOAT4X4 _worldMatrix)
{
	 	GameObject* object = new GameObject();
	 
		object->object = new WorldObject();
	 	FBXModel* tempModel = new FBXModel(_modelPath.c_str());
		
		for(unsigned int i = 0; i < tempModel->GetTextureIndices().size(); ++i)
		{
			object->object->AddTexture(TextureManager::GetTexture(tempModel->GetTextureIndices()[i]));
		}

	 	object->object->AddBaseComponent(RENDER_COMPONENT);
	 	object->object->AddRenderComponent(VERTEX_BUFFER_RENDER_COMPONENT);
	 	object->object->AddRenderComponent(INDEX_BUFFER_RENDER_COMPONENT);
	 	object->object->AddRenderComponent(CONSTANT_BUFFER_RENDER_COMPONENT);
	 	object->object->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &tempModel->meshData->positions[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * tempModel->meshData->positions.size());
	 	object->object->AddVertexBufferComponent(VERTEX_NORMAL_COMPONENT, &tempModel->meshData->normals[0], sizeof(XMFLOAT3), sizeof(XMFLOAT3) * tempModel->meshData->normals.size());
	 	object->object->AddVertexBufferComponent(VERTEX_TEXCOORD_COMPONENT, &tempModel->meshData->UVs[0], sizeof(XMFLOAT2), sizeof(XMFLOAT2) * tempModel->meshData->UVs.size());
	 	object->object->AddIndexBufferComponent(INDICIES_COMPONENT, &tempModel->meshData->indices[0], sizeof(unsigned long), sizeof(unsigned long) * tempModel->meshData->indices.size());
	 	((WorldObject*)object->object)->SetWorldMatrix(_worldMatrix);
		object->object->AddConstantBufferComponent(WORLD_MATRIX_COMPONENT, ((WorldObject*)object->object)->GetWorldMatrixP(), sizeof(XMFLOAT4X4), ((WorldObject*)object->object)->GetWorldMatrixP());
		object->object->AddConstantBufferComponent(VIEW_MATRIX_COMPONENT, (*CameraManager::currentCamera)->GetInvViewMatrixP(), sizeof(XMFLOAT4X4), (*CameraManager::currentCamera)->GetInvViewMatrixP());
		object->object->AddConstantBufferComponent(PROJECTION_MATRIX_COMPONENT, (*CameraManager::currentCamera)->GetProjectionMatrixP(), sizeof(XMFLOAT4X4), (*CameraManager::currentCamera)->GetProjectionMatrixP());
		object->object->SetShaders(0, -1, 0, -1);
		object->object->FinalizeObject();

		if(_objectType == STATIC_OBJECT)
		{
			staticObjects.push_back(object);
		}
		else if(_objectType == DYNAMIC_OBJECT)
		{
			dynamicObjects.push_back(object);
		}
}

void ObjectManager::UpdateObjects(float _dt)
{
	//CALL THE OBJECTS PHYSICAL COMPONENT TO UPDATE ITS POSITION
}

void ObjectManager::RenderObjects()
{
	for(unsigned int i = 0; i < staticObjects.size(); ++i)
	{
		if(staticObjects[i]->object->IsRenderable())
			staticObjects[i]->object->Render();
	}

	for(unsigned int i = 0; i < dynamicObjects.size(); ++i)
	{
		if(dynamicObjects[i]->object->IsRenderable())
			dynamicObjects[i]->object->Render();
	}

	finalPassObject->Render();
}