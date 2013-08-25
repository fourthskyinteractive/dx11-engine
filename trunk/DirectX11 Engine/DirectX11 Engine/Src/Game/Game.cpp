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

XMFLOAT3						Game::lightPos;
ParentMeshObject*				Game::pointLight;
vector<XMFLOAT3>				Game::pointLightPos;
bool							Game::isRunning;
Timer							Game::timer;

ParentMeshObject*				Game::mesh = NULL;
ScreenSpaceObject*				Game::lightPass = NULL;
ScreenSpaceObject*				Game::edgeDetectionPass = NULL;
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

bool Game::Initialize(HINSTANCE _hInstance, HWND _hWnd, bool _fullscreen, bool _bVsync, int _screenWidth, int _screenHeight)
{
	hwnd = _hWnd;
	ShowCursor(false);
	bool result;
	backfaceCulling = true;

	directInput = new DirectInput;
	result = directInput->Initialize(_hInstance, _hWnd, _screenWidth, _screenHeight);

	cameraRotation.x = 0;
	cameraRotation.y = 0;

	camera = new Camera(XMFLOAT3(0.0f, 0.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	camera->SetLens(XMConvertToRadians(50), (1366.0f / 768.0f), 0.1f, 10000.0f);
	camera->UpdateViewMatrix();

	timer.Init();

	lightPos = XMFLOAT3(0.0f, 10.0f, 0.0f);
	
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, true, 1366, 768, false);
// 
// 	XMFLOAT3* testArray = new XMFLOAT3[10];
// 	for(int i = 0; i < 10; ++i)
// 	{
// 		testArray[i] = XMFLOAT3(i, i, i);
// 	}
// 
// 	unsigned int* indicies = new unsigned int[10];
// 	for(int i = 0; i < 10; ++i)
// 	{
// 		indicies[i] = i;
// 	}

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

	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);
	mesh->UpdateWorldMatrix();
	mesh->Render();
	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);

// 	for(unsigned int i = 0; i < pointLightPos.size(); ++i)
// 	{
// 		D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);
// 		pointLight->SetPosition(pointLightPos[i]);
// 		pointLight->UpdateWorldMatrix();
// 		pointLight->Update(0.0f);
// 		pointLight->Render();
// 	}
	//terrain->Render();

	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);
	D3D11Renderer::d3dImmediateContext->OMSetBlendState(D3D11Renderer::blendState, 0, 0xffffffff);
	lightPass->Render();
	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);
	//edgeDetectionPass->Render();
	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);

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
		ID3D11Resource* resource;
		D3D11Renderer::shaderResourceView[4]->GetResource(&resource);
		SaveDDSTextureToFile(D3D11Renderer::d3dImmediateContext, resource, L"PositionTexture.dds");
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
	ShaderManager::AddShader("Res/Compiled Shaders/VertexShader.cso", VERTEX_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/PixelShader.cso", PIXEL_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/TextureVertexShader.cso", VERTEX_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/TexturePixelShader.cso", PIXEL_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/MultipleTextureVertexShader.cso", VERTEX_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/MultipleTexturePixelShader.cso", PIXEL_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/DepthVertexShader.cso", VERTEX_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/DepthPixelShader.cso", PIXEL_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/DeferredCombineVertexShader.cso", VERTEX_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/DeferredCombinePixelShader.cso", PIXEL_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/DeferredCombineGeometryShader.cso", GEOMETRY_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/DeferredGeometryVertexShader.cso", VERTEX_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/DeferredGeometryPixelShader.cso", PIXEL_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/BillboardGeometryShader.cso", GEOMETRY_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/SkyboxGeometryShader.cso", GEOMETRY_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/SkyboxPixelShader.cso", PIXEL_SHADER);
}

void Game::InitializeObjects()
{
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

	skyBox = new ScreenSpaceObject();
	skyBox->Initialize(D3D11Renderer::renderTargetView[RENDER_BACKBUFFER], D3D11Renderer::shaderResourceView[1], DEFERRED_COMBINE_VERTEX_SHADER, SKYBOX_PIXEL_SHADER, SKYBOX_GEOMETRY_SHADER);

	lightPass = new ScreenSpaceObject();
	lightPass->Initialize(D3D11Renderer::renderTargetView[RENDER_BACKBUFFER], D3D11Renderer::shaderResourceView[1], DEFERRED_COMBINE_VERTEX_SHADER, DEFERRED_COMBINE_PIXEL_SHADER, DEFERRED_COMBINE_GEOMETRY_SHADER);

	//edgeDetectionPass = new ScreenSpaceObject();
	//edgeDetectionPass->Initialize(D3D11Renderer::renderTargetView[RENDER_BACKBUFFER], D3D11Renderer::shaderResourceView[1], DEFERRED_COMBINE_VERTEX_SHADER, EDGE_DETECTION_PIXEL_SHADER, DEFERRED_COMBINE_GEOMETRY_SHADER);
}

void Game::InitializeLights()
{
	//ParentMeshObject* newLight = new ParentMeshObject();
	//newLight->Initialize("Res/Models/UnitSphere.fbx", lightPos, XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), DIFFUSE_SHADER, true, NULL);

	//lights.push_back(newLight);

	//pointLight = new ParentMeshObject();
	//pointLight->Initialize("Res/Models/UnitSphere.fbx", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), DIFFUSE_SHADER, true, NULL);


	LightManager::SetAmbientLight("Ambient Light", XMFLOAT4(.15f, .15f, .15f, 1.0f), true);
	LightManager::AddDirectionalLight("Directional Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), true);
	//LightManager::AddDirectionalLight("Directional Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), true);
	//LightManager::AddDirectionalLight("Directional Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), true);
	//LightManager::AddDirectionalLight("Directional Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), true);


 	//float posX = -70.0f;
 	//float posZ = -70.0f;
	//for(int i = 0; i < 10; ++i)
	//{
	//	for(int j = 0; j < 10; ++j)
	//	{
	//		float randR = ((float) rand()) / (float) RAND_MAX;
	//		float randG = ((float) rand()) / (float) RAND_MAX;
	//		float randB = ((float) rand()) / (float) RAND_MAX;
	//
	//		LightManager::AddPointLight("Point Light", XMFLOAT4(randR, randG, randB, 1.0f), XMFLOAT3(posX, 10.0f, posZ), 20, true);
	//
	//		pointLightPos.push_back(XMFLOAT3(posX, 10.0f, posZ));
	//
	//		posZ += 14;
	//	}
	//	posZ = -70;
	//	posX += 14;
	//}

	//LightManager::AddPointLight("Point Light", XMFLOAT4(1.0f, .50f, 1.0f, 1.0f), lightPos, 20, true);
	//LightManager::AddPointLight("Point Light", XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), lightPos, 20, true);

	LightObjects::Initialize(D3D11Renderer::renderTargetView[7], D3D11Renderer::shaderResourceView[5]);
}