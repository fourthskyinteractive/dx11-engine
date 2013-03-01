#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>


#include "Definitions.h"
#include "../EventSystem/Event.h"
#include "../Utility/Misc/Timer.h"
#include "../Renderer/Effects/d3dx11effect.h"
#include "../Camera/Camera.h"

struct SimpleCubeVertex
{
    XMFLOAT3 pos;   // position
    XMFLOAT3 color; // color
};

struct ConstantBuffer
{
    XMFLOAT4X4 model;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

class Game
{
public:
	Game();
	~Game();
	Game(Game& game);

	static bool isRunning;
	static bool Initialize(HWND _hWnd, bool _bFullscreen, bool _bVsync, int _nScreenWidth, int _nScreenHeight);
	static void Run();
	static void Render();
	static void Update();
	static void Exit();
	static void CalculateFrameStats();
	static Timer& GetTimer(){return timer;}

	static void LoadCompiledShaders();
	static void MakeIndexAndVertexBuffers();

private:
	static Timer timer;
	static Camera* camera;
	static float degrees;

	static ID3D11Buffer* boxVB;
	static ID3D11Buffer* boxIB;
	static ID3D11Buffer* constantBuffer;
	static ConstantBuffer constantBufferData;

	static ID3D11InputLayout* inputLayout;
	
	static ID3DX11EffectMatrixVariable* worldViewProj;
};


#endif