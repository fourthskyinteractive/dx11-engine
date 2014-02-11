#include "Game.h"

#include "../Utility/Math/MathHelper.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Renderer/Effects/d3dx11effect.h"
#include "../Renderer/ShaderManager.h"
#include "../Utility/Model Loaders/ObjLoader.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "../Game Objects/ParentMeshObject.h"
#include "../Game Objects/Lights/LightManager.h"
#include "../Renderer/Shader Classes/BaseShader.h"
#include "../Utility/Misc/TerrainGenerator.h"
#include "../Game Objects/Component System/ConstantBufferUpdateFunctions.h"
#include "../Game Objects/FBXModel.h"

//Shaders
#include "../../GBufferVertexShader.csh"
#include "../../GBufferPixelShader.csh"
#include "../../DeferredLightingComputeShader.csh"
#include "../../DeferredLightingVertexShader.csh"
#include "../../DeferredLightingGeometryShader.csh"
#include "../../DeferredLightingPixelShader.csh"

//#include <D3DX11async.h>
#include <fstream>
using std::ifstream;
using std::ios_base;
#include <iostream>
using std::cin;

#include <cmath>

#ifdef _DEBUG
#include "../Utility/Misc/DebugOutput.h"
#include<sstream>
using std::stringstream;
#endif

#define NEAR_PLANE 0.10f
#define FAR_PLANE 1000.0f

BaseObject*						Game::baseObject;
BaseObject*						Game::secondObject;
BaseObject*						Game::computeObject;

XMFLOAT3						Game::lightPos;
ParentMeshObject*				Game::pointLight;
vector<XMFLOAT3>				Game::pointLightPos;
bool							Game::isRunning;
Timer							Game::timer;

ParentMeshObject*				Game::mesh = NULL;
ScreenSpaceObject*				Game::lightPass = NULL;
ScreenSpaceObject*				Game::skyBox = NULL;
Terrain*						Game::terrain = NULL;

ID3D11Buffer*					Game::boxVB;
ID3D11Buffer*					Game::boxIB;
ID3D11Buffer*					Game::constantBuffer;
ConstantBuffer					Game::constantBufferData;

XMFLOAT4X4						Game::viewMatrix;

ID3D11InputLayout*				Game::inputLayout;
HWND							Game::hwnd;

Camera*							Game::camera;
DirectInput*					Game::directInput;
int								Game::currMouseX;
int								Game::currMouseY;
int								Game::prevMouseX;
int								Game::prevMouseY;

bool							Game::backfaceCulling;
bool							Game::backFaceSwap = false;

XMFLOAT2						Game::cameraRotation;
XMFLOAT4						Game::widthHeightNearFar;
XMFLOAT4						Game::frustumExtentsXY;

bool Game::Initialize(HINSTANCE _hInstance, HWND _hWnd, bool _fullscreen, bool _bVsync, int _screenWidth, int _screenHeight)
{
	hwnd = _hWnd;
	ShowCursor(false);
	bool result;
	backfaceCulling = true;

	directInput = new DirectInput;
	result = directInput->Initialize(_hInstance, _hWnd, _screenWidth, _screenHeight);
	widthHeightNearFar.x = (float)_screenWidth;
	widthHeightNearFar.y = (float)_screenHeight;
	widthHeightNearFar.z = (float)NEAR_PLANE;
	widthHeightNearFar.w = (float)FAR_PLANE;
	cameraRotation.x = 0;
	cameraRotation.y = 0;

	camera = new Camera(XMFLOAT3(0.0f, 100.0f, 200.0f), XMFLOAT3(0.0f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
	camera->SetLens(XMConvertToRadians(50), ((float)_screenWidth / (float)_screenHeight), NEAR_PLANE, FAR_PLANE);
	camera->UpdateViewMatrix();

	frustumExtentsXY.y = ((2.0f * tan(XMConvertToRadians(50)) / 2.0f) * FAR_PLANE);
	frustumExtentsXY.x = (frustumExtentsXY.y * ((float)_screenWidth / (float)_screenHeight));
	frustumExtentsXY.w = ((2.0f * tan(XMConvertToRadians(50)) / 2.0f) * NEAR_PLANE);
	frustumExtentsXY.z = (frustumExtentsXY.w * ((float)_screenWidth / (float)_screenHeight));

	//frustumExtentsXY.y /= 1000.0f;
	//frustumExtentsXY.x /= 1000.0f;

	timer.Init();

	lightPos = XMFLOAT3(0.0f, 10.0f, 0.0f);
	
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, false, _screenWidth, _screenHeight, false);

	LoadCompiledShaders();
	InitializeLights();
	InitializeObjects();

	if(bResult)
	{
		isRunning = true;
	}
	else
	{
		isRunning = false;
	}

	return true;
}

void Game::Run()
{
	Update();
	Render();
}

void Game::Render()
{
	D3D11Renderer::ClearScene(reinterpret_cast<const float*>(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	D3D11Renderer::d3dImmediateContext->OMSetBlendState(NULL, 0, 0xffffffff);

	baseObject->Render();
	secondObject->Render();
	//D3D11Renderer::d3dImmediateContext->OMSetBlendState(D3D11Renderer::blendState, 0, 0xffffffff);
	//lightPass->Render();
	computeObject->Render();

	D3D11Renderer::Present(D3D11Renderer::vsyncEnabled, 0);
}

void Game::Update()
{
	timer.TimeStep();
	float deltaTime = (timer.GetDeltaTimeFloat() / 1000.0f);

	//Get Input
	Input(deltaTime);
	mesh->Update(deltaTime);

	CalculateFrameStats();
	camera->UpdateViewMatrix();
}

void Game::Input(float _deltaTime)
{
	//Update the directInput
	directInput->Update();
	prevMouseX = currMouseX;
	prevMouseY = currMouseY;

	directInput->GetMouseLocation(currMouseX, currMouseY);

	if(directInput->IsKeyPressed(DIK_RETURN))
	{
// 		DEBUG::printf("X Location: ");
// 		DEBUG::printf("");
// 		DEBUG::printf("Y Location: ");
// 		DEBUG::printf("");
	}

	if(directInput->IsKeyPressed(DIK_W))
	{
		camera->Walk(50.0f * _deltaTime);
	}
	if(directInput->IsKeyPressed(DIK_S))
	{
		camera->Walk(-50.0f * _deltaTime); 
	}
	if(directInput->IsKeyPressed(DIK_A))
	{
		camera->Strafe(-50.0f * _deltaTime); 
	}
	if(directInput->IsKeyPressed(DIK_D))
	{
		camera->Strafe(50.0f * _deltaTime); 
	}

	if(directInput->IsKeyPressed(DIK_B))
	{
		D3D11Renderer::BackfaceCulling(!backfaceCulling);
		backfaceCulling = !backfaceCulling;
	}

// 	if(directInput->IsKeyPressed(DIK_M))
// 	{
// 		lightPos.y += (5.0f * _deltaTime);
// 		lights[0]->SetPosition(lightPos);
// 		LightManager::GetPointLight(0)->SetLightPosition(lightPos);
// 	}
// 	if(directInput->IsKeyPressed(DIK_N))
// 	{
// 		lightPos.y += (-5.0f * _deltaTime);
// 		lights[0]->SetPosition(lightPos);
// 		LightManager::GetPointLight(0)->SetLightPosition(lightPos);
// 	}
	if(directInput->IsKeyPressed(DIK_P))
	{
		D3D11Renderer::TurnZBufferOff();
	}
	if(directInput->IsKeyPressed(DIK_O))
	{
		D3D11Renderer::TurnZBufferOn();
	}

// 	if(directInput->IsKeyPressed(DIK_DOWNARROW))
// 	{
// 		lightPos.z += (-5.0f * _deltaTime);
// 		lights[0]->SetPosition(lightPos);
// 		LightManager::GetPointLight(0)->SetLightPosition(lightPos);
// 	}
// 	if(directInput->IsKeyPressed(DIK_UPARROW))
// 	{
// 		lightPos.z += (5.0f * _deltaTime);
// 		lights[0]->SetPosition(lightPos);
// 		LightManager::GetPointLight(0)->SetLightPosition(lightPos);
// 	}
// 	if(directInput->IsKeyPressed(DIK_RIGHTARROW))
// 	{
// 		lightPos.x += (-5.0f * _deltaTime);
// 		lights[0]->SetPosition(lightPos);
// 		LightManager::GetPointLight(0)->SetLightPosition(lightPos);
// 	}
// 	if(directInput->IsKeyPressed(DIK_LEFTARROW))
// 	{
// 		lightPos.x += (5.0f * _deltaTime);
// 		lights[0]->SetPosition(lightPos);
// 		LightManager::GetPointLight(0)->SetLightPosition(lightPos);
// 	}

	//if(directInput->IsMouseButtonPressed(MOUSE_LEFT))
	{
		float rotationScale = 5.0f;
		float deltaX = (float)currMouseX - (float)prevMouseX;
		float deltaY = (float)currMouseY - (float)prevMouseY;

		if(deltaX != 0)
		{
			camera->Yaw((deltaX * rotationScale) * _deltaTime);
		}

		if(deltaY != 0)
		{
			camera->Pitch((deltaY * rotationScale) * _deltaTime);
		}
	}

	if(directInput->IsEscapePressed())
	{
// 		ID3D11Resource* resource;
// 		D3D11Renderer::shaderResourceView[1]->GetResource(&resource);
// 		SaveDDSTextureToFile(D3D11Renderer::d3dImmediateContext, resource, L"PositionTexture.dds");
		isRunning = false;
	}

	camera->UpdateViewMatrix();
}

void Game::Exit()
{
	D3D11Renderer::Shutdown();
	if(mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if(terrain)
	{
		delete terrain;
		terrain = 0;
	}

	ReleaseCOM(boxVB);
	ReleaseCOM(boxIB);
	ReleaseCOM(inputLayout);

	if(directInput)
	{
		directInput->Shutdown();
		delete directInput;
		directInput = NULL;
	}

	LightManager::Shutdown();
}

void Game::CalculateFrameStats()
{
	static int frameCount = 0;
	static float elapsedTime = 0.0f;

	elapsedTime += (timer.GetDeltaTimeFloat() / 1000.0f);

	frameCount ++;

	if(elapsedTime >= 1.0f)
	{
		float framesPerSec = frameCount / elapsedTime;

		string FPS;
		char MyInfo[16];
		sprintf_s(MyInfo, "%f", framesPerSec);
		FPS += MyInfo;
		SetWindowText(D3D11Renderer::hwnd, FPS.c_str());

		elapsedTime = 0.0f;
		frameCount = 0;
	}
}

void Game::LoadCompiledShaders()
{
	//Object Shaders
	ShaderManager::AddVertexShader((void*)GBufferVertexShader, sizeof(GBufferVertexShader), GBUFFER_VERTEX_SHADER);
	ShaderManager::AddPixelShader((void*)GBufferPixelShader, sizeof(GBufferPixelShader), GBUFFER_PIXEL_SHADER);

	//Screen Space Deferred Shaders
	ShaderManager::AddComputeShader((void*)DeferredLightingComputeShader, sizeof(DeferredLightingComputeShader), DEFERRED_LIGHTING_COMPUTE_SHADER);
	ShaderManager::AddVertexShader((void*)DeferredLightingVertexShader, sizeof(DeferredLightingVertexShader), DEFERRED_LIGHTING_VERTEX_SHADER);
	ShaderManager::AddGeometryShader((void*)DeferredLightingGeometryShader, sizeof(DeferredLightingGeometryShader), DEFERRED_LIGHTING_GEOMETRY_SHADER);
	ShaderManager::AddPixelShader((void*)DeferredLightingPixelShader, sizeof(DeferredLightingPixelShader), DEFERRED_LIGHTING_PIXEL_SHADER);
}

void Game::InitializeObjects()
{
	FBXModel* tempModel = new FBXModel("Res/Models/BlueMinion.fbx");
	XMFLOAT4 positions[4];
	XMFLOAT3 normals[4];
	XMFLOAT2 UVs[4];
	unsigned long indices[6];

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	positions[0] = XMFLOAT4(-200.0f, -50.0f,  200.0f, 0.0f);
	positions[1] = XMFLOAT4( 200.0f, -50.0f,  200.0f, 0.0f);
	positions[2] = XMFLOAT4( 200.0f, -50.0f, -200.0f, 0.0f);
	positions[3] = XMFLOAT4(-200.0f, -50.0f, -200.0f, 0.0f);

	normals[0] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	normals[1] = XMFLOAT3(0.0f, 1.0f, 0.0f); 
	normals[2] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	normals[3] = XMFLOAT3(0.0f, 1.0f, 0.0f);

	UVs[0] = XMFLOAT2(0.0f, 0.0f);
	UVs[1] = XMFLOAT2(0.0f, 1.0f);
	UVs[2] = XMFLOAT2(1.0f, 1.0f);
	UVs[3] = XMFLOAT2(1.0f, 0.0f);



	XMMATRIX worldIdentityM;
	XMFLOAT4X4 worldIdentity;
	worldIdentityM = XMMatrixIdentity();
	XMStoreFloat4x4(&worldIdentity, worldIdentityM);
	ModelData modelData;
	baseObject = new WorldObject();
	//baseObject->LoadModel("Res/Models/Nunu_Bot.fbx", modelData);
	//baseObject->AddTexture(TextureManager::GetTexture(tempModel->GetTextureIndices()[0]));
	baseObject->AddTexture(L"Res/Textures/GrassDiffuse.dds");
	baseObject->AddBaseComponent(RENDER_COMPONENT);
	baseObject->AddRenderComponent(VERTEX_BUFFER_RENDER_COMPONENT);
	baseObject->AddRenderComponent(INDEX_BUFFER_RENDER_COMPONENT);
	baseObject->AddRenderComponent(CONSTANT_BUFFER_RENDER_COMPONENT);
	baseObject->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &positions, sizeof(XMFLOAT4), sizeof(XMFLOAT4) * 4);
	baseObject->AddVertexBufferComponent(VERTEX_NORMAL_COMPONENT, &normals, sizeof(XMFLOAT3), sizeof(XMFLOAT3) * 4);
	baseObject->AddVertexBufferComponent(VERTEX_TEXCOORD_COMPONENT, &UVs, sizeof(XMFLOAT2), sizeof(XMFLOAT2) * 4);
	baseObject->AddIndexBufferComponent(INDICIES_COMPONENT, &indices, sizeof(unsigned long), sizeof(unsigned long) * 6);
// 	baseObject->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &tempModel->meshData->positions[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * tempModel->meshData->positions.size());
// 	baseObject->AddVertexBufferComponent(VERTEX_NORMAL_COMPONENT, &tempModel->meshData->normals[0], sizeof(XMFLOAT3), sizeof(XMFLOAT3) * tempModel->meshData->normals.size());
// 	baseObject->AddVertexBufferComponent(VERTEX_TEXCOORD_COMPONENT, &tempModel->meshData->UVs[0], sizeof(XMFLOAT2), sizeof(XMFLOAT2) * tempModel->meshData->UVs.size());
// 	baseObject->AddIndexBufferComponent(INDICIES_COMPONENT, &tempModel->meshData->indices[0], sizeof(unsigned long), sizeof(unsigned long) * tempModel->meshData->indices.size());
	((WorldObject*)baseObject)->SetWorldMatrix(worldIdentity);
	baseObject->AddConstantBufferComponent(WORLD_MATRIX_COMPONENT, &worldIdentity, sizeof(XMFLOAT4X4), ((WorldObject*)baseObject)->GetWorldMatrixP());
	baseObject->AddConstantBufferComponent(VIEW_MATRIX_COMPONENT, &camera->GetViewMatrixF(), sizeof(XMFLOAT4X4), camera->GetInvViewMatrixP());
	baseObject->AddConstantBufferComponent(PROJECTION_MATRIX_COMPONENT, &camera->GetProjectionMatrixF(), sizeof(XMFLOAT4X4), camera->GetProjectionMatrixP());
	baseObject->SetShaders(0, -1, 0, -1);
	baseObject->FinalizeObject();


	secondObject = new WorldObject();
	secondObject->AddTexture(TextureManager::GetTexture(tempModel->GetTextureIndices()[0]));
	secondObject->AddBaseComponent(RENDER_COMPONENT);
	secondObject->AddRenderComponent(VERTEX_BUFFER_RENDER_COMPONENT);
	secondObject->AddRenderComponent(INDEX_BUFFER_RENDER_COMPONENT);
	secondObject->AddRenderComponent(CONSTANT_BUFFER_RENDER_COMPONENT);
 	secondObject->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &tempModel->meshData->positions[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * tempModel->meshData->positions.size());
 	secondObject->AddVertexBufferComponent(VERTEX_NORMAL_COMPONENT, &tempModel->meshData->normals[0], sizeof(XMFLOAT3), sizeof(XMFLOAT3) * tempModel->meshData->normals.size());
 	secondObject->AddVertexBufferComponent(VERTEX_TEXCOORD_COMPONENT, &tempModel->meshData->UVs[0], sizeof(XMFLOAT2), sizeof(XMFLOAT2) * tempModel->meshData->UVs.size());
 	secondObject->AddIndexBufferComponent(INDICIES_COMPONENT, &tempModel->meshData->indices[0], sizeof(unsigned long), sizeof(unsigned long) * tempModel->meshData->indices.size());
	worldIdentity._42 = -50.0f;
	((WorldObject*)secondObject)->SetWorldMatrix(worldIdentity);
	secondObject->AddConstantBufferComponent(WORLD_MATRIX_COMPONENT, &worldIdentity, sizeof(XMFLOAT4X4), ((WorldObject*)secondObject)->GetWorldMatrixP());
	secondObject->AddConstantBufferComponent(VIEW_MATRIX_COMPONENT, &camera->GetViewMatrixF(), sizeof(XMFLOAT4X4), camera->GetInvViewMatrixP());
	secondObject->AddConstantBufferComponent(PROJECTION_MATRIX_COMPONENT, &camera->GetProjectionMatrixF(), sizeof(XMFLOAT4X4), camera->GetProjectionMatrixP());
	secondObject->SetShaders(0, -1, 0, -1);
	secondObject->FinalizeObject();


	XMFLOAT4 pos = XMFLOAT4(0.0f, 0.0f, 5.0f, 1.0f);
	computeObject = new ScreenSpaceObject();
	computeObject->AddBaseComponent(RENDER_COMPONENT);
	for(int i = 1; i < 6; ++i)
	{
		computeObject->AddTexture(D3D11Renderer::shaderResourceView[i]);
	}
	//TODO:: WORK ON THE COMPUTE LIGHTS!
	//computeObject->AddComputeShaderBuffer(LightManager::GetPointLightsMemory(), sizeof(PointLight), sizeof(PointLight) * LightManager::GetNumberPointLights());
	computeObject->AddRenderComponent(VERTEX_BUFFER_RENDER_COMPONENT);
	computeObject->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &pos, sizeof(XMFLOAT4), sizeof(XMFLOAT4));
	computeObject->AddRenderComponent(CONSTANT_BUFFER_RENDER_COMPONENT);
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &LightManager::numLights, sizeof(XMFLOAT4), LightManager::GetNumberOfLightsMemory());
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &widthHeightNearFar, sizeof(XMFLOAT4), &widthHeightNearFar);
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &camera->GetViewMatrixF(), sizeof(XMFLOAT4X4), camera->GetViewMatrixP());
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &frustumExtentsXY, sizeof(XMFLOAT4), &frustumExtentsXY);
	computeObject->AddComputeShaderBuffer(LightManager::GetPointLightsMemory(), sizeof(PointLightCompressed), sizeof(PointLightCompressed) * LightManager::GetNumberPointLights());
	computeObject->SetShaders(1, 0, 1, 0);
	computeObject->FinalizeObject();

	//terrain = new Terrain();
	//
	//TerrainDescription terrainDescription;
	//terrainDescription.height = 1000.0f;
	//terrainDescription.width = 1000.0f;
	//terrainDescription.numberOfSegments = 512;
	//terrainDescription.smoothingFactor = 150.0f;
	//terrainDescription.centerPoint = XMFLOAT3(0.0f, -20.0f, 0.0f);
	//
	//terrain->Initialize(D3D11Renderer::renderTargetView[RENDER_BACKBUFFER], NULL, terrainDescription);

	mesh = new ParentMeshObject();
	mesh->Initialize("Res/Models/BlueMinion.fbx", XMFLOAT3(0.0f, -75.0f, 200.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 180.0f, 0.0f), DIFFUSE_SHADER, true, L"Res/Textures/BlueMinion.dds");

	//skyBox = new ScreenSpaceObject();
	//skyBox->Initialize(D3D11Renderer::renderTargetView[RENDER_BACKBUFFER], D3D11Renderer::shaderResourceView[1], DEFERRED_COMBINE_VERTEX_SHADER, SKYBOX_PIXEL_SHADER, SKYBOX_GEOMETRY_SHADER);

	//lightPass = new ScreenSpaceObject();
	//lightPass->Initialize(D3D11Renderer::renderTargetView[RENDER_BACKBUFFER], D3D11Renderer::shaderResourceView[1], DEFERRED_COMBINE_VERTEX_SHADER, DEFERRED_COMBINE_PIXEL_SHADER, DEFERRED_COMBINE_GEOMETRY_SHADER);
}

void* Game::InitializeLights()
{
	//ParentMeshObject* newLight = new ParentMeshObject();
	//newLight->Initialize("Res/Models/UnitSphere.fbx", lightPos, XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), DIFFUSE_SHADER, true, NULL);

	//lights.push_back(newLight);

	//pointLight = new ParentMeshObject();
	//pointLight->Initialize("Res/Models/UnitSphere.fbx", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), DIFFUSE_SHADER, true, NULL);


	LightManager::SetAmbientLight("Ambient Light", XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), true);
	LightManager::AddDirectionalLight("Directional Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), true);

	//for(int i = -100; i < 100; i += 20)
	//{
		//for(int j = -100; j <= 100; j += 20)
		//{
			LightManager::AddPointLight("Point Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -45.0f, 0.0f), 80, true);
			LightManager::AddPointLight("Point Light", XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -45.0f, 50.0f), 80, true);
			LightManager::AddPointLight("Point Light", XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -45.0f, -50.0f), 80, true);
			LightManager::AddPointLight("Point Light", XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(50.0f, -45.0f, 0.0f), 80, true);
			LightManager::AddPointLight("Point Light", XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(-50.0f, -45.0f, 0.0f), 80, true);
			//LightManager::AddPointLight("Point Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 100.0f),1000, true);
			//LightManager::AddPointLight("Point Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -100.0f),1000, true);
// 		}
// 	}
	
	

	LightObjects::Initialize(D3D11Renderer::renderTargetView[7], D3D11Renderer::shaderResourceView[5]);
	return NULL;
}