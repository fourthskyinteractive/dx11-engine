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

//#include <D3DX11async.h>
#include <fstream>
using std::ifstream;
using std::ios_base;
#include <iostream>
using std::cin;

#ifdef _DEBUG
#include "../Utility/Misc/DebugOutput.h"
#include<sstream>
using std::stringstream;
#endif

bool							Game::isRunning;
Timer							Game::timer;

ParentMeshObject*				Game::mesh = NULL;
ScreenSpaceObject*				Game::screenSpaceQuad = NULL;

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
	cameraRotation.x = 0;
	cameraRotation.y = 0;

	camera = new Camera(XMFLOAT3(0.0f, 0.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	camera->SetLens(XMConvertToRadians(55), (800.0f / 600.0f), 0.1f, 10000.0f);
	camera->UpdateViewMatrix();

	timer.Init();

	
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, true, 800, 600, false);

	LoadCompiledShaders();
	InitializeLights();
	InitializeObjects();

	directInput = new DirectInput;
	result = directInput->Initialize(_hInstance, _hWnd, _screenWidth, _screenHeight);

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

	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);
	mesh->UpdateWorldMatrix();
	mesh->Render();
	D3D11Renderer::ContextClearState(D3D11Renderer::d3dImmediateContext);
	screenSpaceQuad->Render();
	//LightObjects::Render();
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
		camera->Walk(5.0f * _deltaTime);
	}
	if(directInput->IsKeyPressed(DIK_S))
	{
		camera->Walk(-5.0f * _deltaTime); 
	}
	if(directInput->IsKeyPressed(DIK_A))
	{
		camera->Strafe(5.0f * _deltaTime); 
	}
	if(directInput->IsKeyPressed(DIK_D))
	{
		camera->Strafe(-5.0f * _deltaTime); 
	}

	if(directInput->IsKeyPressed(DIK_B))
	{
		D3D11Renderer::BackfaceCulling(!backfaceCulling);
		backfaceCulling = !backfaceCulling;
	}

	if(directInput->IsKeyPressed(DIK_J))
	{
		screenSpaceQuad->ChangeShaderResourceView(D3D11Renderer::shaderResourceView[1]);
	}
	if(directInput->IsKeyPressed(DIK_K))
	{
		screenSpaceQuad->ChangeShaderResourceView(D3D11Renderer::shaderResourceView[2]);
	}
	if(directInput->IsKeyPressed(DIK_L))
	{
		screenSpaceQuad->ChangeShaderResourceView(D3D11Renderer::shaderResourceView[3]);
	}

	if(directInput->IsKeyPressed(DIK_M))
	{
		backFaceSwap = true;
	}
	if(directInput->IsKeyPressed(DIK_N))
	{
		backFaceSwap = false;
	}
	if(directInput->IsKeyPressed(DIK_P))
	{
		D3D11Renderer::TurnZBufferOff();
	}
	if(directInput->IsKeyPressed(DIK_O))
	{
		D3D11Renderer::TurnZBufferOn();
	}

	//if(directInput->IsMouseButtonPressed(MOUSE_LEFT))
	{
		float rotationScale = 5.0f;
		float deltaX = (float)currMouseX - (float)prevMouseX;
		float deltaY = (float)currMouseY - (float)prevMouseY;

		if(deltaX != 0)
		{
			camera->Yaw((-deltaX * rotationScale) * _deltaTime);
		}

		if(deltaY != 0)
		{
			camera->Pitch((deltaY * rotationScale) * _deltaTime);
		}
	}

	if(directInput->IsEscapePressed())
	{

		isRunning = false;
	}

	XMStoreFloat4x4(&constantBufferData.viewProjection, XMMatrixTranspose(camera->GetViewProjectionMatrixM()));
}

void Game::Exit()
{
	D3D11Renderer::Shutdown();
	if(mesh)
	{
		delete mesh;
		mesh = 0;
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

		char MyInfo[16];
		sprintf_s(MyInfo, "%f", framesPerSec);

		string FPS;
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

}

void Game::InitializeObjects()
{
	mesh = new ParentMeshObject();
	mesh->Initialize("Res/Models/graves.fbx", XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(.005f, .005f, .005f), XMFLOAT3(0.0f, 180.0f, 0.0f), true, L"Res/Textures/graves.dds");

	screenSpaceQuad = new ScreenSpaceObject();
	screenSpaceQuad->Initialize(D3D11Renderer::renderTargetView[RENDER_BACKBUFFER], D3D11Renderer::shaderResourceView[1]);
}

void Game::InitializeLights()
{
	LightManager::SetAmbientLight("Ambient Light", XMFLOAT4(.15f, .15f, .15f, 1.0f), true);
	LightManager::AddDirectionalLight("Directional Light", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), true);
	LightObjects::Initialize(D3D11Renderer::renderTargetView[7], D3D11Renderer::shaderResourceView[5]);
}