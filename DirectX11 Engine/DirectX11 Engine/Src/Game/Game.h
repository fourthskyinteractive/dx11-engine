#ifndef GAME_H
#define GAME_H

#include <d3d11.h>
#include <d3d11shader.h>


#include "Definitions.h"
#include "../EventSystem/Event.h"
#include "../Utility/Misc/Timer.h"
#include "../Renderer/Effects/d3dx11effect.h"
#include "../Camera/Camera.h"
#include "../Input/DirectInput.h"
#include "../Game Objects/BaseObject.h"
#include "../Game Objects/CubeObjectColor.h"
#include "../Game Objects/Mesh.h"
#include "../Game Objects/Lights/LightClass.h"
#include "../Game Objects/ParentMeshObject.h"

class Game
{
public:
	Game();
	~Game();
	Game(Game& game);

	static bool isRunning;
	static bool Initialize(HINSTANCE _hInstance, HWND _hWnd, bool _fullscreen, bool _bVsync, int _screenWidth, int _screenHeight);
	static void Run();
	static void Render();
	static void Update();
	static void Input(float _deltaTime);
	static void Exit();
	static void CalculateFrameStats();
	static Timer& GetTimer(){return timer;}

	static void LoadCompiledShaders();
	static void InitializeObjects();
	static void InitializeLights();

	static Camera* camera;
	static LightClass* lightDiffuse;

private:

	static ParentMeshObject* mesh;
	static CubeObjectColor cubeObject;
	static BaseObject cubeObj;

	static Timer timer;
	static XMFLOAT2 cameraRotation;

	static ID3D11Buffer* boxVB;
	static ID3D11Buffer* boxIB;
	static ID3D11Buffer* constantBuffer;
	static ConstantBuffer constantBufferData;
	static ID3D11InputLayout* inputLayout;

	static XMFLOAT4X4 viewMatrix;

	static DirectInput* directInput;
	static int currMouseX, currMouseY, prevMouseX, prevMouseY;

	static bool backfaceCulling;
};


#endif