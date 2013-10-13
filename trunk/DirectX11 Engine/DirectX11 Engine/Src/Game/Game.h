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
#include "../Renderer/Shader Classes/BaseShader.h"
#include "../Game Objects/BaseObject.h"
#include "../Game Objects/CubeObjectColor.h"
#include "../Game Objects/Mesh.h"
#include "../Game Objects/ParentMeshObject.h"
#include "../Game Objects/ScreenSpaceObject.h"
#include "../Game Objects/LightObjects.h"
#include "../Game Objects/Terrain.h"
#include "../Game Objects/DynamicObject.h"

#include "../Game Objects/BaseObject.h"
#include "../Game Objects/WorldObject.h"

struct vertStruct
{
	XMFLOAT4 position;
	XMFLOAT3 normal;
	XMFLOAT2 texCoord;
};

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
	static HWND hwnd;

private:
	static BaseObject* baseObject;
	static BaseObject* computeObject;

	static XMFLOAT3 lightPos;
	static ParentMeshObject* pointLight;
	static vector<XMFLOAT3> pointLightPos;
	static ParentMeshObject* mesh;
	static ScreenSpaceObject* lightPass;
	static ScreenSpaceObject* skyBox;

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
	static bool backFaceSwap;

	static Terrain* terrain;
};


#endif