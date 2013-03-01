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

ID3D11Buffer*					Game::boxVB;
ID3D11Buffer*					Game::boxIB;

ID3DX11Effect*					Game::fx;
ID3DX11EffectTechnique*			Game::tech;
ID3DX11EffectMatrixVariable*	Game:: worldViewProj;

ID3D11InputLayout*				Game::inputLayout;

XMFLOAT4X4						Game::world;
XMFLOAT4X4						Game::view;
XMFLOAT4X4						Game::proj;

Camera*							Game::camera;

bool Game::Initialize(HWND _hWnd, bool _bFullscreen, bool _bVsync, int _nScreenWidth, int _nScreenHeight)
{
	camera = new Camera(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	XMMATRIX I = XMMatrixIdentity();

	XMStoreFloat4x4(&world, I);
	XMStoreFloat4x4(&view, I);
	XMStoreFloat4x4(&proj, I);

	timer.Init();
	
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, true, 800, 600, true); 
	LoadCompiledShaders();

	if(bResult)
	{
		isRunning = true;
	}
	else
	{
		isRunning = false;
	}

	//BuildGeometryBuffers();
	//BuildFX();
	//BuildVertexLayout();

	return true;
}

void Game::Run()
{
	Update();
	Render();
}

void Game::Render()
{
	D3D11Renderer::ClearScene(reinterpret_cast<const float*>(&XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
		
	D3D11Renderer::Present(0, 0);
}

void Game::Update()
{
	CalculateFrameStats();

	XMVECTOR pos = XMVectorSet(10.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&view, V);

	timer.TimeStep();
}

void Game::Exit()
{
	D3D11Renderer::Shutdown();

	ReleaseCOM(boxVB);
	ReleaseCOM(boxIB);
	ReleaseCOM(fx);
	ReleaseCOM(inputLayout);
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
	ShaderManager::AddShader("Res/Compiled Shaders/VertexShader.cso");
}
