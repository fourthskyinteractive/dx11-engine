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
			unsigned int currentVertIndex = 0;
			for(unsigned int i = 0; i < numVerts; i += 4)
			{
				jointIndex[i].x = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[0].blendingIndex;
				weights[i].x = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[0].blendingWeight;

				jointIndex[i].y = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[1].blendingIndex;
				weights[i].y = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[1].blendingWeight;

				jointIndex[i].z = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[2].blendingIndex;
				weights[i].z = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[2].blendingWeight;

				jointIndex[i].w = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[3].blendingIndex;
				weights[i].w = (float)tempModel->meshData->vertices[currentVertIndex].vertexBlendingInfo[3].blendingWeight;

				currentVertIndex += 1;
			}
		}

#pragma endregion

		XMFLOAT4X4* flattenedMatricesF;
		unsigned int numJoints = tempModel->meshData->skeleton.joints.size();
#pragma region Flatten Matrix Heirarchy

		flattenedMatricesF = new XMFLOAT4X4[numJoints];
		for(unsigned int i = 0; i < numJoints; ++i)
		{
			int parentIndex = i;

			FbxAMatrix currentMatrix;
			currentMatrix.SetIdentity();
			while(parentIndex != -1)
			{
 				currentMatrix = currentMatrix * tempModel->meshData->skeleton.joints[parentIndex].globalBindposeInverseMatrix;
				parentIndex = tempModel->meshData->skeleton.joints[parentIndex].parentIndex;
			}

			flattenedMatricesF[i]._11 = (float)currentMatrix.mData[0][0];
			flattenedMatricesF[i]._12 = (float)currentMatrix.mData[0][1];
			flattenedMatricesF[i]._13 = (float)currentMatrix.mData[0][2];
			flattenedMatricesF[i]._14 = (float)currentMatrix.mData[0][3];
			flattenedMatricesF[i]._21 = (float)currentMatrix.mData[1][0];
			flattenedMatricesF[i]._22 = (float)currentMatrix.mData[1][1];
			flattenedMatricesF[i]._23 = (float)currentMatrix.mData[1][2];
			flattenedMatricesF[i]._24 = (float)currentMatrix.mData[1][3];
			flattenedMatricesF[i]._31 = (float)currentMatrix.mData[2][0];
			flattenedMatricesF[i]._32 = (float)currentMatrix.mData[2][1];
			flattenedMatricesF[i]._33 = (float)currentMatrix.mData[2][2];
			flattenedMatricesF[i]._34 = (float)currentMatrix.mData[2][3];
			flattenedMatricesF[i]._41 = (float)currentMatrix.mData[3][0];
			flattenedMatricesF[i]._42 = (float)currentMatrix.mData[3][1];
			flattenedMatricesF[i]._43 = (float)currentMatrix.mData[3][2];
			flattenedMatricesF[i]._44 = (float)currentMatrix.mData[3][3];
		}

#pragma endregion


	 	object->object->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &positions[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * numVerts);
	 	object->object->AddVertexBufferComponent(VERTEX_NORMAL_COMPONENT, &normals[0], sizeof(XMFLOAT3), sizeof(XMFLOAT3) * numVerts);
	 	object->object->AddVertexBufferComponent(VERTEX_TEXCOORD_COMPONENT, &UVs[0], sizeof(XMFLOAT2), sizeof(XMFLOAT2) * numVerts);
		object->object->AddVertexBufferComponent(VERTEX_ANIMATIONBLEND_COMPONENT, &jointIndex[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * numVerts);
		object->object->AddVertexBufferComponent(VERTEX_ANIMATIONBLEND_COMPONENT, &weights[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * numVerts);
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