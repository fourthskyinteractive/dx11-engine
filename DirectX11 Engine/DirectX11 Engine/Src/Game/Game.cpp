#include "Game.h"

#include "../Input/Input.h"
#include "../Utility/Math/MathHelper.h"
#include "../EventSystem/EventSystem.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Renderer/Effects/d3dx11effect.h"

#include <D3DX11async.h>
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

bool Game::Initialize(HWND _hWnd, bool _bFullscreen, bool _bVsync, int _nScreenWidth, int _nScreenHeight)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&world, I);
	XMStoreFloat4x4(&view, I);
	XMStoreFloat4x4(&proj, I);

	timer.Init();
	Input::Init();
	
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, true, 1680, 1050, false); 

	if(bResult)
	{
		isRunning = true;
	}
	else
	{
		isRunning = false;
	}

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();

	return true;
}

void Game::Run()
{
	Update();
	Render();
}

void Game::Render()
{
	static bool once = true;
	if(once)
	{
		D3D11Renderer::ClearScene(reinterpret_cast<const float*>(&XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
		
		D3D11Renderer::Present(0, 0);
		once = false;
	}
}

void Game::Update()
{
	CalculateFrameStats();
	Input::Update();

	XMVECTOR pos = XMVectorSet(10.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&view, V);

	timer.TimeStep();

	EventSystem::ProcessEvents();
}

void Game::Exit()
{
	D3D11Renderer::Shutdown();
	EventSystem::ShutdownEventSystem();

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

void Game::BuildGeometryBuffers()
{
	XMFLOAT3 test = XMFLOAT3(1.0f, 1.0f, 1.0f);
	
	Vertex vertices[8];

	vertices[0].color = Colors::White;
	vertices[1].color = Colors::Black;
	vertices[2].color = Colors::Red;
	vertices[3].color = Colors::Green;
	vertices[4].color = Colors::Blue;
	vertices[5].color = Colors::Yellow;
	vertices[6].color = Colors::Cyan;
	vertices[7].color = Colors::Magenta;

	vertices[0].pos = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	vertices[1].pos = XMFLOAT3(-1.0f, +1.0f, -1.0f);
	vertices[2].pos = XMFLOAT3(+1.0f, +1.0f, -1.0f);
	vertices[3].pos = XMFLOAT3(+1.0f, -1.0f, -1.0f);
	vertices[4].pos = XMFLOAT3(-1.0f, -1.0f, +1.0f);
	vertices[5].pos = XMFLOAT3(-1.0f, +1.0f, +1.0f);
	vertices[6].pos = XMFLOAT3(+1.0f, +1.0f, +1.0f);
	vertices[7].pos = XMFLOAT3(+1.0f, -1.0f, +1.0f);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	D3D11Renderer::d3dDevice->CreateBuffer(&vbd, &vinitData, &boxVB);

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	D3D11Renderer::d3dDevice->CreateBuffer(&ibd, &iinitData, &boxIB);
}

void Game::BuildFX()
{
// 	std::ifstream fin("../FX/color.fxo",)
// 
// 	D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 
// 		0, D3D11Renderer::d3dDevice, &fx);
// 
// 	// Done with compiled shader.
// 	ReleaseCOM(compiledShader);
// 
// 	tech    = fx->GetTechniqueByName("ColorTech");
// 	worldViewProj = fx->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void Game::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	tech->GetPassByIndex(0)->GetDesc(&passDesc);
	D3D11Renderer::d3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &inputLayout);
}
