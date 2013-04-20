#ifndef BASESHADER_H
#define BASESHADER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include "../../Game/Definitions.h"
//#include "../../Game Objects/ChildMeshObject.h"

enum BUFFER_TYPE{BASE_BUFFER, DEPTH_BUFFER, LIGHT_BUFFER, DIFFUSE_BUFFER};

class ChildMeshObject;

class BaseShader
{
public:
	BaseShader(){bufferType = BASE_BUFFER;}
	virtual ~BaseShader(){;}

	virtual bool Initialize(){return true;}
	virtual void Shutdown(){;}
	virtual bool Render(int _indexCount, ID3D11RenderTargetView* _renderTarget){return true;}
	virtual void Update(ChildMeshObject* _obj, ID3D11ShaderResourceView* _texture = NULL){;}
	virtual void SetShader(){;}
	void SetBufferType(int _bufferType){bufferType = _bufferType;}
	int GetBufferType(){return bufferType;}

private:
	virtual bool InitializeShader(int _vertexShaderIndex, int  _pixelShaderIndex, int geometryShaderIndex = 0){return true;}
	virtual void ShutdownShader(){;}
	virtual void RenderShader(int _indexCount, ID3D11RenderTargetView* _renderTarget){;}

	int bufferType;
};
#endif
