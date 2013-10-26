#ifndef D3D11RENDERER_H
#define D3D11RENDERER_H

#include "../Game/Definitions.h"
#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgi.h>
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
	static void BackfaceCulling(bool _backfaceCulling);
	static void TurnZBufferOn();
	static void TurnZBufferOff();
	static void ContextClearState(ID3D11DeviceContext* _context);

	static bool vsyncEnabled;
	static int videoCardMemory;
	static char videoCardDescription[128];

	static HWND hwnd;
	static CComPtr<ID3D11Device> d3dDevice;
	static CComPtr<ID3D11DeviceContext> d3dImmediateContext;
	static CComPtr<IDXGISwapChain> swapChain;
	static CComPtr<ID3D11Texture2D> renderTextures[7];
	static CComPtr<ID3D11RenderTargetView> renderTargetView[8];
	static CComPtr<ID3D11ShaderResourceView> shaderResourceView[8];
	static CComPtr<ID3D11Texture2D> depthStencilBuffer;

	static CComPtr<ID3D11Texture2D> backBufferPtr;
	static CComPtr<ID3D11UnorderedAccessView> backBufferUAV;
	static CComPtr<ID3D11DepthStencilState> depthStencilState;
	static CComPtr<ID3D11DepthStencilView> depthStencilView;
	static CComPtr<ID3D11DepthStencilView> orthoDepthStencilView;
	static CComPtr<ID3D11DepthStencilState> orthoDepthStencilState;
	static CComPtr<ID3D11RasterizerState> rasterStateNoCulling;
	static CComPtr<ID3D11RasterizerState> rasterStateBackfaceCulling;
	static CComPtr<ID3D11BlendState> blendState;
	static D3D11_VIEWPORT viewport;
	
	static D3D_FEATURE_LEVEL supportedFeatureLevel;
};
#endif