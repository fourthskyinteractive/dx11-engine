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
float							Game::degrees;
ID3D11Buffer*					Game::constantBuffer;
ConstantBuffer					Game::constantBufferData;

ID3DX11EffectMatrixVariable*	Game:: worldViewProj;

ID3D11InputLayout*				Game::inputLayout;

XMFLOAT4X4						Game::world;
XMFLOAT4X4						Game::view;
XMFLOAT4X4						Game::proj;

Camera*							Game::camera;

bool Game::Initialize(HWND _hWnd, bool _bFullscreen, bool _bVsync, int _nScreenWidth, int _nScreenHeight)
{
	degrees = 0.0f;

	camera = new Camera(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

	XMMATRIX I = XMMatrixIdentity();

	XMStoreFloat4x4(&world, I);
	XMStoreFloat4x4(&view, I);
	XMStoreFloat4x4(&proj, I);

	timer.Init();
	
	bool bResult = D3D11Renderer::Initialize(_hWnd, true, true, 800, 600, true); 
	LoadCompiledShaders();
	MakeIndexAndVertexBuffers();

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
	D3D11Renderer::ClearScene(reinterpret_cast<const float*>(&XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
	
	D3D11Renderer::d3dImmediateContext->IASetInputLayout(Game::inputLayout);

	UINT stride = sizeof(SimpleCubeVertex);
	UINT offset = 0;

	D3D11Renderer::d3dImmediateContext->IASetVertexBuffers(
		0,
		1,
		&boxVB,
		&stride,
		&offset);

	D3D11Renderer::d3dImmediateContext->IASetIndexBuffer(
		boxIB,
		DXGI_FORMAT_R16_UINT,
		0);

	D3D11Renderer::d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D11Renderer::d3dImmediateContext->VSSetShader(
		ShaderManager::vertexShaders[BASIC_VERTEX_SHADER].shader,
		nullptr,
		0);
	D3D11Renderer::d3dImmediateContext->VSSetConstantBuffers(
		0,
		1,
		&constantBuffer);
	D3D11Renderer::d3dImmediateContext->PSSetShader(
		ShaderManager::pixelShaders[BASIC_PIXEL_SHADER].shader,
		nullptr,
		0);
	D3D11Renderer::d3dImmediateContext->DrawIndexed(
		38,
		0,
		0);

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

	degrees += (90.0f * timer.GetDeltaTimeFloat());
	XMStoreFloat4x4(&constantBufferData.model, 
		XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), degrees));

	timer.TimeStep();
}

void Game::Exit()
{
	D3D11Renderer::Shutdown();

	ReleaseCOM(boxVB);
	ReleaseCOM(boxIB);
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
	ShaderManager::AddShader("Res/Compiled Shaders/VertexShader.cso", VERTEX_SHADER);
	ShaderManager::AddShader("Res/Compiled Shaders/PixelShader.cso", PIXEL_SHADER);
}

void Game::MakeIndexAndVertexBuffers()
{
	const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11Renderer::d3dDevice->CreateInputLayout(
		basicVertexLayoutDesc,
		ARRAYSIZE(basicVertexLayoutDesc),
		ShaderManager::vertexShaders[BASIC_VERTEX_SHADER].buffer->GetBufferPointer(),
		ShaderManager::vertexShaders[BASIC_VERTEX_SHADER].buffer->GetBufferSize(),
		&inputLayout);

	SimpleCubeVertex cubeVertices[] =
        {
            { XMFLOAT3(-0.5f, 0.5f, -1.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // +Y (top face)
            { XMFLOAT3( 0.5f, 0.5f, -1.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
            { XMFLOAT3( 0.5f, 0.5f,  -.5f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            { XMFLOAT3(-0.5f, 0.5f,  -.5f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },

            { XMFLOAT3(-0.5f, -0.5f,  -0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // -Y (bottom face)
            { XMFLOAT3( 0.5f, -0.5f,  -0.5f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
            { XMFLOAT3( 0.5f, -0.5f, -1.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(-0.5f, -0.5f, -1.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
        };

        unsigned short cubeIndices[] =
        {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            3, 2, 5,
            3, 5, 4,

            2, 1, 6,
            2, 6, 5,

            1, 7, 6,
            1, 0, 7,

            0, 3, 4,
            0, 4, 7
        };

		D3D11_BUFFER_DESC vertexBufferDesc = {0};
		vertexBufferDesc.ByteWidth = sizeof(SimpleCubeVertex) * ARRAYSIZE(cubeVertices);
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		D3D11Renderer::d3dDevice->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&boxVB);

		D3D11_BUFFER_DESC indexBufferDesc;
        indexBufferDesc.ByteWidth = sizeof(unsigned short) * ARRAYSIZE(cubeIndices);
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA indexBufferData;
        indexBufferData.pSysMem = cubeIndices;
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

		D3D11Renderer::d3dDevice->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&boxIB);

		 D3D11_BUFFER_DESC constantBufferDesc = {0};
        constantBufferDesc.ByteWidth = sizeof(constantBufferData);
        constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = 0;
        constantBufferDesc.MiscFlags = 0;
        constantBufferDesc.StructureByteStride = 0;

		D3D11Renderer::d3dDevice->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&constantBuffer);

		XMStoreFloat4x4(&constantBufferData.view, camera->GetViewMatrix());
		XMStoreFloat4x4(&constantBufferData.projection, camera->GetProjectionMatrix());
}