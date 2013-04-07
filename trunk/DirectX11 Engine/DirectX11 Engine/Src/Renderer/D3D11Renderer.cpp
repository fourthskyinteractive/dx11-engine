#include "D3D11Renderer.h"
#include "ShaderManager.h"
#include <d3d11.h>
#include "../Game/Game.h"

#include <assert.h>

HWND						D3D11Renderer::hwnd;
ID3D11Device*				D3D11Renderer::d3dDevice = NULL;
ID3D11DeviceContext*		D3D11Renderer::d3dImmediateContext = NULL;
IDXGISwapChain*				D3D11Renderer::swapChain = NULL;
ID3D11RenderTargetView*		D3D11Renderer::renderTargetView[8];
ID3D11Texture2D*			D3D11Renderer::depthStencilBuffer = NULL;
ID3D11DepthStencilView*		D3D11Renderer::depthStencilView = NULL;
ID3D11RasterizerState*		D3D11Renderer::rasterStateBackfaceCulling = NULL;
ID3D11RasterizerState*		D3D11Renderer::rasterStateNoCulling = NULL;

bool						D3D11Renderer::vsyncEnabled = false;
int							D3D11Renderer::videoCardMemory = 0;
char						D3D11Renderer::videoCardDescription[128];
ID3D11DepthStencilState*	D3D11Renderer::depthStencilState = NULL;

D3D_FEATURE_LEVEL			D3D11Renderer::supportedFeatureLevel;

bool D3D11Renderer::Initialize(HWND _hwnd, bool _fullscreen, bool _vsync, int _horizontalRes, int _verticalRes, bool _msaa)
{
	HRESULT hr;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	ID3D11Texture2D* backBufferPtr;
	ID3D11Texture2D* renderTextures[7];
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;

	// Store the vsync setting.
	vsyncEnabled = _vsync;

	//Before we can initialize Direct3D we have to get the refresh rate from the video card/monitor. Each computer may be slightly different so we will need to query for that information. We query for the numerator and denominator values and then pass them to DirectX during the setup and it will calculate the proper refresh rate. If we don't do this and just set the refresh rate to a default value which may not exist on all computers then DirectX will respond by performing a blit instead of a buffer flip which will degrade performance and give us annoying errors in the debug output.

	// Create a DirectX graphics interface factory.
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(hr))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	hr = factory->EnumAdapters(0, &adapter);
	if(FAILED(hr))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	hr = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(hr))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(hr))
	{
		return false;
	}
 
	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(hr))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)_horizontalRes)
		{
			if(displayModeList[i].Height == (unsigned int)_verticalRes)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//We now have the numerator and denominator for the refresh rate. The last thing we will retrieve using the adapter is the name of the video card and the amount of memory on the video card.
	// Get the adapter (video card) description.
	hr = adapter->GetDesc(&adapterDesc);
	if(FAILED(hr))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	//Now that we have stored the numerator and denominator for the refresh rate and the video card information we can release the structures and interfaces used to get that information.
	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

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

	hr = D3D11CreateDevice(
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
		printf("D3D11CreateDevice Failed.");
		return false;
	}

	if(supportedFeatureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		printf("Direct3D Feature Level 11 unsupported.");
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
		printf("DX 11 Unsupported");
		return false;
	}

// 	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, 6, 
// 		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &d3dDevice, NULL, &d3dImmediateContext);

	if(FAILED(hr))
	{
		return false;
	}

	//Sometimes this call to create the device will fail if the primary video card is not compatible with DirectX 11. Some machines may have the primary card as a DirectX 10 video card and the secondary card as a DirectX 11 video card. Also some hybrid graphics cards work that way with the primary being the low power Intel card and the secondary being the high power Nvidia card. To get around this you will need to not use the default device and instead enumerate all the video cards in the machine and have the user choose which one to use and then specify that card when creating the device.
	//Now that we have a swap chain we need to get a pointer to the back buffer and then attach it to the swap chain. We'll use the CreateRenderTargetView function to attach the back buffer to our swap chain.

	// Get the pointer to the back buffer.
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(hr))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	hr = d3dDevice->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView[0]);
	if(FAILED(hr))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	//backBufferPtr->Release();
	//backBufferPtr = 0;

	//We will also need to set up a depth buffer description. We'll use this to create a depth buffer so that our polygons can be rendered properly in 3D space. At the same time we will attach a stencil buffer to our depth buffer. The stencil buffer can be used to achieve effects such as motion blur, volumetric shadows, and other things.
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = _horizontalRes;
	depthBufferDesc.Height = _verticalRes;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Make all of the other render target views
	for(int i = 0; i < 7; ++i)
	{
		hr = d3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &renderTextures[i]);
		hr = d3dDevice->CreateRenderTargetView(renderTextures[i], NULL, &renderTargetView[i + 1]);
		renderTextures[i]->Release();
		renderTextures[i] = 0;
	}

	//Now we create the depth/stencil buffer using that description. You will notice we use the CreateTexture2D function to make the buffers, hence the buffer is just a 2D texture. The reason for this is that once your polygons are sorted and then rasterized they just end up being colored pixels in this 2D buffer. Then this 2D buffer is drawn to the screen.
	// Create the texture for the depth buffer using the filled out description.
	hr = d3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if(FAILED(hr))
	{
		return false;
	}

	//Now we need to setup the depth stencil description. This allows us to control what type of depth test Direct3D will do for each pixel.
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//With the description filled out we can now create a depth stencil state.
	// Create the depth stencil state.
	hr = d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if(FAILED(hr))
	{
		return false;
	}

	//With the created depth stencil state we can now set it so that it takes effect. Notice we use the device context to set it.
	// Set the depth stencil state.
	d3dImmediateContext->OMSetDepthStencilState(depthStencilState, 1);

	//The next thing we need to create is the description of the view of the depth stencil buffer. We do this so that Direct3D knows to use the depth buffer as a depth stencil texture. After filling out the description we then call the function CreateDepthStencilView to create it.
	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hr = d3dDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if(FAILED(hr))
	{
		return false;
	}

	//With that created we can now call OMSetRenderTargets. This will bind the render target view and the depth stencil buffer to the output render pipeline. This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created. With the graphics written to the back buffer we can then swap it to the front and display our graphics on the user's screen.
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	d3dImmediateContext->OMSetRenderTargets(8, renderTargetView, depthStencilView);

	//Now that the render targets are setup we can continue on to some extra functions that will give us more control over our scenes for future tutorials. First thing is we'll create is a rasterizer state. This will give us control over how polygons are rendered. We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons. By default DirectX already has a rasterizer state set up and working the exact same as the one below but you have no control to change it unless you set up one yourself
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = d3dDevice->CreateRasterizerState(&rasterDesc, &rasterStateBackfaceCulling);
	if(FAILED(hr))
	{
		return false;
	}

	// Now set the rasterizer state.
	d3dImmediateContext->RSSetState(rasterStateBackfaceCulling);


	//The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. Set this to be the entire size of the window.
	// Setup the viewport for rendering.
	viewport.Width = (float)_horizontalRes;
	viewport.Height = (float)_verticalRes;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	d3dImmediateContext->RSSetViewports(1, &viewport);

 	return true;
}

void D3D11Renderer::ClearScene(const float* _color /* = reinterpret_cast<const float*> */)
{
	assert(d3dImmediateContext);
	assert(swapChain);

	for(int i = 0; i < 8; ++i)
	{
		if(renderTargetView[i])
		{
			d3dImmediateContext->ClearRenderTargetView(renderTargetView[i], _color);
		}
	}
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
	for(int i = 0; i < 8; ++i)
	{
		if(renderTargetView[i])
		{
			ReleaseCOM(renderTargetView[i]);
		}
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