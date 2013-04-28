#ifndef OBJECTSHADER_H
#define OBJECTSHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
#include "BaseShader.h"

class ObjectShader : public BaseShader
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 viewProjection;
	};

public:
	ObjectShader();
	ObjectShader(const ObjectShader&);
	~ObjectShader();

	bool Initialize();
	void Shutdown();
	bool Render(int _indexCount, ID3D11RenderTargetView* _renderTarget);
	bool UpdatePixelShaderTextureConstants(ID3D11ShaderResourceView* _textureArray);
	bool UpdateVertexShaderConstants(XMFLOAT4X4 _worldMatrix, XMFLOAT4X4 _viewProjMatrix);
	void Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture = NULL);
	void SetShader();

private:
	bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex);
	void ShutdownShader();
	void RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget);

private:
	unsigned int vertexShaderIndex;
	unsigned int pixelShaderIndex;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* constantBuffer;
	ID3D11SamplerState* sampleState;
};

#endif