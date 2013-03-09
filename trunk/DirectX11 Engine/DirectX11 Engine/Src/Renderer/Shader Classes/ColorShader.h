#ifndef COLORSHADER_H
#define COLORSHADER_H

#include <Windows.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"

class ColorShader
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 viewProjection;
	};

public:
	ColorShader();
	ColorShader(const ColorShader&);
	~ColorShader();

	bool Initialize(ID3D11Device* _device);
	void Shutdown();
	bool Render(ID3D11DeviceContext* _deviceContext, int _indexCount, XMFLOAT4X4 _worldMatrix,
		XMFLOAT4X4 _viewProjMatrix);
	bool UpdateShaderConstants(ID3D11DeviceContext* _deviceContext, XMFLOAT4X4 _worldMatrix, 
		XMFLOAT4X4 _viewProjMatrix);

private:
	bool InitializeShader(ID3D11Device* _device, int _vertexShaderIndex, int _pixelShaderIndex);
	void ShutdownShader();	
	void RenderShader(ID3D11DeviceContext* _deviceContext, int _indexCount);

private:
	unsigned int vertexShaderIndex;
	unsigned int pixelShaderIndex;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantBuffer;

};

#endif
