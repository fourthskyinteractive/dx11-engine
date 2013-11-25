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

//Shaders
#include "../../GBufferVertexShader.csh"
#include "../../GBufferPixelShader.csh"
#include "../../DeferredLightingComputeShader.csh"
#include "../../DeferredLightingVertexShader.csh"
#include "../../DeferredLightingGeometryShader.csh"
#include "../../DeferredLightingPixelShader.csh"


#include "ScreenGrab.h"

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

#define NEAR_PLANE 0.1f
#define FAR_PLANE 1000.0f

BaseObject*						Game::baseObject;
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

	camera = new Camera(XMFLOAT3(0.0f, 0.0f, -50.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	camera->SetLens(XMConvertToRadians(50), ((float)_screenWidth / (float)_screenHeight), NEAR_PLANE, FAR_PLANE);
	camera->UpdateViewMatrix();

	frustumExtentsXY.y = (tan(XMConvertToRadians(50) / 2.0f) * 1000.0f) / 1000.0f;
	frustumExtentsXY.x = (frustumExtentsXY.y * (float)_screenWidth / (float)_screenHeight) / 1000.0f;

	timer.Init();

	lightPos = XMFLOAT3(0.0f, 10.0f, 0.0f);
	
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, true, _screenWidth, _screenHeight, false);

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
	if(directInput->IsKeyPressed(DIK_SPACE))
	{
		camera->Raise(50.0f * _deltaTime);
	}
	if(directInput->IsKeyPressed(DIK_LCONTROL))
	{
		camera->Raise(-50.0f * _deltaTime); 
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

	XMStoreFloat4x4(&constantBufferData.viewProjection, camera->GetViewProjectionMatrixM());
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
	vertStruct tempstruct[4];
	tempstruct[0].position.x = -1;
	tempstruct[0].position.y = -1;
	tempstruct[0].position.z = 1;
	tempstruct[0].position.w = 0;

	tempstruct[1].position.x = -1;
	tempstruct[1].position.y = 1;
	tempstruct[1].position.z = 1;
	tempstruct[1].position.w = 0;

	tempstruct[2].position.x = 1;
	tempstruct[2].position.y = 1;
	tempstruct[2].position.z = 1;
	tempstruct[2].position.w = 0;

	tempstruct[3].position.x = 1;
	tempstruct[3].position.y = -1;
	tempstruct[3].position.z = 1;
	tempstruct[3].position.w = 0;

	tempstruct[0].normal.x = -1;
	tempstruct[0].normal.y = -1;
	tempstruct[0].normal.z = 1;
				  
	tempstruct[1].normal.x = -1;
	tempstruct[1].normal.y = 1;
	tempstruct[1].normal.z = 1;
				 
	tempstruct[2].normal.x = 1;
	tempstruct[2].normal.y = 1;
	tempstruct[2].normal.z = 1;
				  
	tempstruct[3].normal.x = 1;
	tempstruct[3].normal.y = -1;
	tempstruct[3].normal.z = 1;

	tempstruct[0].texCoord.x = 0;
	tempstruct[0].texCoord.y = 1;

	tempstruct[1].texCoord.x = 0;
	tempstruct[1].texCoord.y = 0;

	tempstruct[2].texCoord.x = 1;
	tempstruct[2].texCoord.y = 0;

	tempstruct[3].texCoord.x = 1;
	tempstruct[3].texCoord.y = 1;

	unsigned long indicies[6];
	indicies[0] = 0;
	indicies[1] = 1;
	indicies[2] = 2;
	indicies[3] = 2;
	indicies[4] = 3;
	indicies[5] = 0;

	XMMATRIX worldIdentityM;
	XMFLOAT4X4 worldIdentity;
	worldIdentityM = XMMatrixIdentity();
	XMStoreFloat4x4(&worldIdentity, worldIdentityM);
	ModelData modelData;
	baseObject = new WorldObject();
	DX11RenderDataMembers* renderDataMembers = baseObject->GetRenderDataMembers();
	baseObject->LoadModel("Res/Models/BlueMinion.fbx", modelData);
	baseObject->AddTexture(L"Res/Textures/BlueMinion.dds");
	baseObject->AddBaseComponent(RENDER_COMPONENT);
	baseObject->AddRenderComponent(VERTEX_BUFFER_RENDER_COMPONENT);
	baseObject->AddRenderComponent(INDEX_BUFFER_RENDER_COMPONENT);
	baseObject->AddRenderComponent(CONSTANT_BUFFER_RENDER_COMPONENT);
	baseObject->AddVertexBufferComponent(VERTEX_POSITION_COMPONENT, &modelData.positions[0], sizeof(XMFLOAT4), sizeof(XMFLOAT4) * modelData.positions.size());
	baseObject->AddVertexBufferComponent(VERTEX_NORMAL_COMPONENT, &modelData.normals[0], sizeof(XMFLOAT3), sizeof(XMFLOAT3) * modelData.normals.size());
	baseObject->AddVertexBufferComponent(VERTEX_TEXCOORD_COMPONENT, &modelData.texCoords[0], sizeof(XMFLOAT2), sizeof(XMFLOAT2) * modelData.texCoords.size());
	baseObject->AddIndexBufferComponent(INDICIES_COMPONENT, &modelData.indices[0], sizeof(unsigned long), sizeof(unsigned long) * modelData.indices.size());
	((WorldObject*)baseObject)->SetWorldMatrix(worldIdentity);
	void* (*function)();
	function = CBUpdateWorldMatrix;
	baseObject->AddConstantBufferComponent(WORLD_MATRIX_COMPONENT, &worldIdentity, sizeof(XMFLOAT4X4), function);
	function = CBUpdateViewMatrix;
	baseObject->AddConstantBufferComponent(VIEW_MATRIX_COMPONENT, &camera->GetViewMatrixF(), sizeof(XMFLOAT4X4), function);
	function = CBUpdateProjectionMatrix;
	baseObject->AddConstantBufferComponent(PROJECTION_MATRIX_COMPONENT, &camera->GetProjectionMatrixF(), sizeof(XMFLOAT4X4), function);
	baseObject->SetShaders(0, -1, 0, -1);
	baseObject->FinalizeObject();


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
	function = CBGetNumberOfLights;
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &LightManager::numLights, sizeof(XMFLOAT4), function);
	function = CBGetScreenWidthHeight;
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &widthHeightNearFar, sizeof(XMFLOAT4), function);
	function = CBUpdateViewMatrix;
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &camera->GetViewMatrixF(), sizeof(XMFLOAT4X4), function);
	function = CBGetFrustumExtents;
	computeObject->AddConstantBufferComponent(MISCELANEOUS_COMPONENT, &frustumExtentsXY, sizeof(XMFLOAT4), function);
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


	LightManager::SetAmbientLight("Ambient Light", XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f), true);
	LightManager::AddDirectionalLight("Directional Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), true);

	LightManager::AddPointLight("Point Light", XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), lightPos, 20, true);

	LightObjects::Initialize(D3D11Renderer::renderTargetView[7], D3D11Renderer::shaderResourceView[5]);
	return NULL;
}