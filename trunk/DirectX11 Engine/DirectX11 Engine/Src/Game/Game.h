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


private:
	static Timer timer;
	static Camera* camera;

	static ID3D11Buffer* boxVB;
	static ID3D11Buffer* boxIB;
	
	static ID3DX11Effect* fx;
	static ID3DX11EffectTechnique* tech;
	static ID3DX11EffectMatrixVariable* worldViewProj;

	static ID3D11InputLayout* inputLayout;

	static XMFLOAT4X4 world;
	static XMFLOAT4X4 view;
	static XMFLOAT4X4 proj;	
};


#endif