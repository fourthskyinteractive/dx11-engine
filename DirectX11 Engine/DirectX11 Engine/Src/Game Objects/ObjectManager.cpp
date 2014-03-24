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

	XMFLOAT4* positions;
	XMFLOAT3* normals;
	XMFLOAT2* UVs;
	XMFLOAT4* jointIndex;
	XMFLOAT4* weights;
	unsigned int numVerts = tempModel->meshData->vertices.size();
#pragma region //This is for compressing the Vertex Information so that it is ready for the VertexBuffers.
	positions = new XMFLOAT4[numVerts];
	normals = new XMFLOAT3[numVerts];
	UVs = new XMFLOAT2[numVerts];

	ZeroMemory(positions, sizeof(XMFLOAT3) * numVerts);
	ZeroMemory(normals, sizeof(XMFLOAT3) * numVerts);
	ZeroMemory(UVs, sizeof(XMFLOAT2) * numVerts);

	for(unsigned int i = 0; i < numVerts; ++i)
	{
		positions[i] = XMFLOAT4(tempModel->meshData->vertices[i].position.x, tempModel->meshData->vertices[i].position.y, tempModel->meshData->vertices[i].position.z, 1.0f);
		normals[i] = tempModel->meshData->vertices[i].normal;
		UVs[i] = tempModel->meshData->vertices[i].UV;
	}

	jointIndex = new XMFLOAT4[numVerts];
	weights = new XMFLOAT4[numVerts];
	ZeroMemory(jointIndex, sizeof(XMFLOAT4) * numVerts);
	ZeroMemory(weights, sizeof(XMFLOAT4) * numVerts);

	if(tempModel->meshData->isAnimated)
	{			
		for(unsigned int i = 0; i < numVerts; i++)
		{
			jointIndex[i].x = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[0].blendingIndex;
			weights[i].x = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[0].blendingWeight;

			jointIndex[i].y = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[1].blendingIndex;
			weights[i].y = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[1].blendingWeight;

			jointIndex[i].z = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[2].blendingIndex;
			weights[i].z = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[2].blendingWeight;

			jointIndex[i].w = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[3].blendingIndex;
			weights[i].w = (float)tempModel->meshData->vertices[i].vertexBlendingInfo[3].blendingWeight;
		}
	}

#pragma endregion

	if(tempModel->meshData->isAnimated)
	{
		object->object->AddBaseComponent(GAMEOBJECT_COMPONENT);
		object->object->AddGameObjectComponent(ANINMATION_GAMEOBJECT_COMPONENT);
		((WorldObject*)object->object)->AddAnimation(tempModel->meshData->animationData);
		((WorldObject*)object->object)->GetAnimationComponent()->ChangeAnimation(ANIMATION_IDLE);
	}

	XMFLOAT4 animationData = XMFLOAT4((float)tempModel->meshData->isAnimated, (float)tempModel->meshData->numBones, (float)tempModel->meshData->animationLength, 0.0f);
	object->object->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &positions[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * numVerts);
	object->object->AddVertexBufferComponent(VERTEX_NORMAL_COMPONENT, &normals[0], sizeof(XMFLOAT3), sizeof(XMFLOAT3) * numVerts);
	object->object->AddVertexBufferComponent(VERTEX_TEXCOORD_COMPONENT, &UVs[0], sizeof(XMFLOAT2), sizeof(XMFLOAT2) * numVerts);
	if(tempModel->meshData->isAnimated)
	{
		object->object->AddVertexBufferComponent(VERTEX_ANIMATIONBLEND_COMPONENT, &jointIndex[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * numVerts);
		object->object->AddVertexBufferComponent(VERTEX_ANIMATIONBLEND_COMPONENT, &weights[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * numVerts);
	}	
	object->object->AddIndexBufferComponent(INDICIES_COMPONENT, &tempModel->meshData->indices[0], sizeof(unsigned long), sizeof(unsigned long) * tempModel->meshData->indices.size());
	((WorldObject*)object->object)->SetWorldMatrix(_worldMatrix);
	object->object->AddConstantBufferComponent(WORLD_MATRIX_COMPONENT, ((WorldObject*)object->object)->GetWorldMatrixP(), sizeof(XMFLOAT4X4), ((WorldObject*)object->object)->GetWorldMatrixP());
	object->object->AddConstantBufferComponent(VIEW_MATRIX_COMPONENT, (*CameraManager::currentCamera)->GetInvViewMatrixP(), sizeof(XMFLOAT4X4), (*CameraManager::currentCamera)->GetInvViewMatrixP());
	object->object->AddConstantBufferComponent(PROJECTION_MATRIX_COMPONENT, (*CameraManager::currentCamera)->GetProjectionMatrixP(), sizeof(XMFLOAT4X4), (*CameraManager::currentCamera)->GetProjectionMatrixP());
	object->object->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &animationData, sizeof(XMFLOAT4), NULL);

	if(tempModel->meshData->isAnimated)
	{
		object->object->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, object->object->GetAnimationComponent()->GetCurrentFrameMemoryPointer(), sizeof(XMFLOAT4X4) * tempModel->meshData->numBones, object->object->GetAnimationComponent()->GetCurrentFrameMemoryPointer());
		object->object->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, object->object->GetAnimationComponent()->GetCurrentFrameMemoryPointer(), sizeof(XMFLOAT4X4) * tempModel->meshData->numBones, object->object->GetAnimationComponent()->GetCurrentAnimationIBPMemoryPointer());
	}
	else
	{
		object->object->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, NULL, 0, NULL);
		object->object->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, NULL, 0, NULL);
	}

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

	tempModel->Destroy();
}

void ObjectManager::UpdateObjects(float _dt)
{
	//CALL THE OBJECTS PHYSICAL COMPONENT TO UPDATE ITS POSITION
	for(unsigned int i = 0; i < dynamicObjects.size(); ++i)
	{
		dynamicObjects[i]->object->Update(_dt);
	}
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