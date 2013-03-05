#include "D3D11Renderer.h"
#include "ShaderManager.h"
#include <windows.h>
#include <d3d11.h>
#include "../Game/Game.h"

#include <assert.h>

HWND					D3D11Renderer::hwnd;
ID3D11Device*			D3D11Renderer::d3dDevice = NULL;
ID3D11DeviceContext*	D3D11Renderer::d3dImmediateContext = NULL;
IDXGISwapChain*			D3D11Renderer::swapChain = NULL;
ID3D11RenderTargetView* D3D11Renderer::renderTargetView = NULL;
ID3D11Texture2D*		D3D11Renderer::depthStencilBuffer = NULL;
ID3D11DepthStencilView* D3D11Renderer::depthStencilView = NULL;
ID3D11RasterizerState*	D3D11Renderer::rasterStateBackfaceCulling = NULL;
ID3D11RasterizerState*	D3D11Renderer::rasterStateNoCulling = NULL;

D3D_FEATURE_LEVEL		D3D11Renderer::supportedFeatureLevel;

bool D3D11Renderer::Initialize(HWND _hwnd, bool _fullscreen, bool _vsync, int _horizontalRes, int _verticalRes, bool _msaa)
{
	hwnd = _hwnd;
	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[6] = 
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	HRESULT hr = D3D11CreateDevice(
		NULL,							//default adaptor
		D3D_DRIVER_TYPE_HARDWARE,		//D3D_DRIVER_TYPE_HARDWARE if computer supports directX11
		NULL,							//no software device
		createDeviceFlags,				
		featureLevels,					//default feature level array
		6,							
		D3D11_SDK_VERSION,
		&d3dDevice, 
		&supportedFeatureLevel, 
		&d3dImmediateContext);

	if(FAILED(hr))
	{
		MessageBox(0, "D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if(supportedFeatureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	UINT m4xMsaaQuality;
	d3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);

	assert(m4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	//ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferDesc.Width						= _horizontalRes;
	sd.BufferDesc.Height					= _verticalRes;
	sd.BufferDesc.RefreshRate.Numerator		= 60;
	sd.BufferDesc.RefreshRate.Denominator	= 1;
	sd.BufferDesc.Format					= DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

	if(_msaa)
	{
		sd.SampleDesc.Count		= 4;
		sd.SampleDesc.Quality	= m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count		= 1;
		sd.SampleDesc.Quality	= 0;
	}

	sd.BufferUsage	= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount	= 1;
	sd.OutputWindow = hwnd;
	sd.Windowed		= _fullscreen;
	sd.SwapEffect	= DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags		= 0;

	IDXGIDevice* dxgiDevice = NULL;
	d3dDevice->QueryInterface(_uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = NULL;
	dxgiDevice->GetParent(_uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = NULL;
	dxgiAdapter->GetParent(_uuidof(IDXGIFactory), (void**)&dxgiFactory);

	//CREATE THE CHAIN SWAP
	hr = dxgiFactory->CreateSwapChain(d3dDevice, &sd, &swapChain);

	if(hr == DXGI_ERROR_INVALID_CALL)
	{
		MessageBox(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}
	
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	ID3D11Texture2D* backBuffer;
	swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	hr = d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
	ReleaseCOM(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width		= _horizontalRes;
	depthStencilDesc.Height		= _verticalRes;
	depthStencilDesc.MipLevels	= 1;
	depthStencilDesc.ArraySize	= 1;
	depthStencilDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	if(_msaa)
	{
		depthStencilDesc.SampleDesc.Count	= 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count	= 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage			= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags		= 0;


	d3dDevice->CreateTexture2D(
		&depthStencilDesc,			//Description of texture to create
		0,
		&depthStencilBuffer);		//Return pointer to depth/stencil buffer

	d3dDevice->CreateDepthStencilView(
		depthStencilBuffer,			//Resource we want to create a view to
		0,
		&depthStencilView);			//Return depth/stencil view

	d3dImmediateContext->OMSetRenderTargets(
		1,
		&renderTargetView,
		depthStencilView);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the no culling rasterizer state.
	hr = d3dDevice->CreateRasterizerState(&rasterDesc, &rasterStateNoCulling);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the backface culling rasterizer state.
	hr = d3dDevice->CreateRasterizerState(&rasterDesc, &rasterStateBackfaceCulling);

	d3dImmediateContext->RSSetState(rasterStateBackfaceCulling);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)_horizontalRes;
	vp.Height = (float)_verticalRes;
	vp.MinDepth = D3D11_MIN_DEPTH;
	vp.MaxDepth = D3D11_MAX_DEPTH;

	d3dImmediateContext->RSSetViewports(1, &vp);

	return true;
}

void D3D11Renderer::ClearScene(const float* _color /* = reinterpret_cast<const float*> */)
{
	assert(d3dImmediateContext);
	assert(swapChain);

	d3dImmediateContext->ClearRenderTargetView(renderTargetView, _color);
	d3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11Renderer::Present(int _vBlankWait, int _presentationFlags)
{
	assert(d3dImmediateContext);
	assert(swapChain);

	HRESULT hr = swapChain->Present(_vBlankWait, _presentationFlags);
}

void D3D11Renderer::ResetDevice()
{

}

void D3D11Renderer::BackfaceCulling(bool _backfaceCulling)
{
	if(_backfaceCulling)
	{
		d3dImmediateContext->RSSetState(rasterStateBackfaceCulling);
	}
	else
	{
		d3dImmediateContext->RSSetState(rasterStateNoCulling);
	}
}

void D3D11Renderer::Shutdown()
{
	if(depthStencilView)
	{
		ReleaseCOM(depthStencilView);
	}
	if(depthStencilBuffer)
	{
		ReleaseCOM(depthStencilBuffer);
	}
	if(renderTargetView)
	{
		ReleaseCOM(renderTargetView);
	}
	if(swapChain)
	{
		ReleaseCOM(swapChain);
	}
	if(d3dImmediateContext)
	{
		ReleaseCOM(d3dImmediateContext);
	}
	if(d3dDevice)
	{
		ReleaseCOM(d3dDevice);
	}
}