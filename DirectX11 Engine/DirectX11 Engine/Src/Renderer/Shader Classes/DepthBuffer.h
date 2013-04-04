#ifndef DEPTHBUFFER_H
#define DEPTHBUFFER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"

class DepthBuffer
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 viewProjection;
	};

public:
	DepthBuffer();
	DepthBuffer(const DepthBuffer&);
	~DepthBuffer();

	bool Initialize();
	void Shutdown();
	bool Render(int _indexCount);
	bool UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix);

private:
	bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex);
	void ShutdownShader();
	void RenderShader(int _indexCount);

private:
	unsigned int vertexShaderIndex;
	unsigned int pixelShaderIndex;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantBuffer;
};

#endif
