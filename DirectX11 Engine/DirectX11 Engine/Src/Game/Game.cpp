#include "Game.h"

#include "../Utility/Math/MathHelper.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Renderer/Effects/d3dx11effect.h"
#include "../Renderer/ShaderManager.h"

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

BaseObject						Game::cubeObj;
CubeObject						Game::cubeObject;

ID3D11Buffer*					Game::boxVB;
ID3D11Buffer*					Game::boxIB;
float							Game::degrees;
ID3D11Buffer*					Game::constantBuffer;
ConstantBuffer					Game::constantBufferData;

XMFLOAT4X4						Game::viewMatrix;

ID3D11InputLayout*				Game::inputLayout;

Camera*							Game::camera;
DirectInput*					Game::directInput;
int								Game::currMouseX;
int								Game::currMouseY;
int								Game::prevMouseX;
int								Game::prevMouseY;

bool							Game::backfaceCulling;


XMFLOAT2						Game::cameraRotation;

XMFLOAT3						Game::positions[10000];
XMFLOAT3						Game::scales[10000];
XMFLOAT3						Game::rotations[10000];

bool Game::Initialize(HINSTANCE _hInstance, HWND _hWnd, bool _fullscreen, bool _bVsync, int _screenWidth, int _screenHeight)
{
	ShowCursor(false);
	bool result;
	backfaceCulling = true;
	degrees = 0.0f;
	cameraRotation.x = 0;
	cameraRotation.y = 0;

	camera = new Camera(XMFLOAT3(0.0f, 0.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
	camera->SetLens(XMConvertToRadians(55), (800.0f / 600.0f), .01f, 10000.0f);
	camera->UpdateViewMatrix();

	timer.Init();

	directInput = new DirectInput;
	result = directInput->Initialize(_hInstance, _hWnd, _screenWidth, _screenHeight);
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, true, 800, 600, true); 
	LoadCompiledShaders();
	InitializeObjects();

	for(int i = 0; i < 10000; ++i)
	{
		positions[i] = XMFLOAT3((float)((rand()% 100) - 50), (float)((rand()% 100) - 50), (float)((rand()% 100) - 50));
		float scale = (float)(rand() % 5);
		scales[i] = XMFLOAT3(scale, scale, scale);
		rotations[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

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

	for(int i = 0; i < 10000; ++i)
	{
		cubeObject.UpdateWorldMatrix(positions[i], scales[i], rotations[i]);
		cubeObject.Render(D3D11Renderer::d3dImmediateContext);
	}	

	D3D11Renderer::Present(1, 0);
}

void Game::Update()
{
	//Get Input
	Input();
	timer.TimeStep();

	degrees += (100.0f * (timer.GetDeltaTimeFloat() / 1000.0f));

	for(int i = 0; i < 10000; ++i)
	{
		rotations[i] = XMFLOAT3(0.0f, degrees, 0.0f);
	}

	CalculateFrameStats();
	camera->UpdateViewMatrix();
}

void Game::Input()
{
	//Update the directInput
	directInput->Update();
	prevMouseX = currMouseX;
	prevMouseY = currMouseY;

	directInput->GetMouseLocation(currMouseX, currMouseY);

	if(directInput->IsKeyPressed(DIK_W))
	{
		camera->Walk(5.0f * (timer.GetDeltaTimeFloat() / 1000.0f));
	}
	if(directInput->IsKeyPressed(DIK_S))
	{
		camera->Walk(-5.0f * (timer.GetDeltaTimeFloat() / 1000.0f)); 
	}
	if(directInput->IsKeyPressed(DIK_A))
	{
		camera->Strafe(-5.0f * (timer.GetDeltaTimeFloat() / 1000.0f)); 
	}
	if(directInput->IsKeyPressed(DIK_D))
	{
		camera->Strafe(5.0f * (timer.GetDeltaTimeFloat() / 1000.0f)); 
	}

	if(directInput->IsKeyPressed(DIK_B))
	{
		D3D11Renderer::BackfaceCulling(!backfaceCulling);
		backfaceCulling = !backfaceCulling;
	}

	if(directInput->IsMouseButtonPressed(MOUSE_LEFT))
	{
		float rotationScale = 5.0f;
		float deltaX = (float)currMouseX - (float)prevMouseX;
		float deltaY = (float)currMouseY - (float)prevMouseY;

		if(deltaX != 0)
		{	
			camera->Yaw((deltaX * rotationScale) * (timer.GetDeltaTimeFloat() / 1000.0f));
		}

		if(deltaY != 0)
		{
			camera->Pitch((deltaY * rotationScale) * (timer.GetDeltaTimeFloat() / 1000.0f));
		}
	}

	if(directInput->IsEscapePressed())
	{

		isRunning = false;
	}

	XMStoreFloat4x4(&constantBufferData.viewProjection, XMMatrixTranspose(camera->GetViewProjectionMatrix()));
}

void Game::Exit()
{
	D3D11Renderer::Shutdown();

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
}

void Game::InitializeObjects()
{
	cubeObject.Initialize(D3D11Renderer::d3dDevice, XMFLOAT3(0.0f, 0.0f, 2.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
}