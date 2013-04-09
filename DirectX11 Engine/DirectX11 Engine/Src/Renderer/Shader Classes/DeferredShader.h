#ifndef DEFERREDSHADER_H
#define DEFERREDSHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
#include "BaseShader.h"

class DeferredShader : public BaseShader
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 viewProjection;
	};

public:
	DeferredShader();
	DeferredShader(const DeferredShader&);
	~DeferredShader();

	bool Initialize();
	void Shutdown();
	bool Render(int _indexCount);
	bool UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix);
	void Update(ChildMeshObject* _obj);

private:
	bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex);
	void ShutdownShader();
	void RenderShader(int _indexCount);
	void SetShader();

private:
	unsigned int vertexShaderIndex;
	unsigned int pixelShaderIndex;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantBuffer;
};

#endif
