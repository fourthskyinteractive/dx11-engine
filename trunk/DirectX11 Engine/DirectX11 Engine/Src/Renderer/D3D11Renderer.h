#ifndef D3D11RENDERER_H
#define D3D11RENDERER_H

#include <Windows.h>
#include "../Game/Definitions.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class D3D11Renderer
{
public:
	D3D11Renderer();
	D3D11Renderer(const D3D11Renderer&){};
	~D3D11Renderer();

	static bool Initialize(HWND _hwnd, bool _fullscreen, bool _vsync, int _horizontalRes, int _verticalRes, bool _msaa);
	static void ClearScene(const float* _color = reinterpret_cast<const float*>(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	static void ResetDevice();
	static void Present(int _vBlankWait, int _presentationFlags);
	static void Shutdown();

	static HWND hwnd;
	static ID3D11Device* d3dDevice;
	static ID3D11DeviceContext* d3dImmediateContext;
	static IDXGISwapChain* swapChain;
	static ID3D11RenderTargetView* renderTargetView;
	static ID3D11Texture2D* depthStencilBuffer;
	static ID3D11DepthStencilView* depthStencilView;

	static D3D_FEATURE_LEVEL supportedFeatureLevel;
};
#endif