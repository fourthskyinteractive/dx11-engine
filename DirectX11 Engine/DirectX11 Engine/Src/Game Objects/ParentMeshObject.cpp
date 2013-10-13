#include "ParentMeshObject.h"
#include "ChildMeshObject.h"
#include "../Game/Game.h"
#include "../Renderer/D3D11Renderer.h"
#include "../Utility/Model Loaders/FBXLoader.h"
#include "Lights/LightManager.h"


ParentMeshObject::ParentMeshObject()
{
	textures = NULL;
}

ParentMeshObject::ParentMeshObject(const ParentMeshObject& _parentMeshObject)
{

}

ParentMeshObject::~ParentMeshObject()
{

}

void ParentMeshObject::Initialize(char* _filePath, XMFLOAT3 _position, XMFLOAT3 _scale, XMFLOAT3 _rotation, SHADER_TO_USE _shaderToUse, bool _hasOneTexture /* = false */, WCHAR* _textureFilePath /* = L"" */)
{
	position = _position;
	scale = _scale;
	rotation = _rotation;
	rotation.x = XMConvertToRadians(rotation.x);
	rotation.y = XMConvertToRadians(rotation.y);
	rotation.z = XMConvertToRadians(rotation.z);

	textures = new TextureManager();
	if( _hasOneTexture)
	{
		if(_textureFilePath)
			AddTexture(_textureFilePath);
	}

	//TODO: Rewrite this whole file
	//objectShader.Initialize(DEFERRED_GEOMETRY_VERTEX_SHADER, DEFERRED_GEOMETRY_PIXEL_SHADER, NO_GEOMETRY_SHADER);
	objectShader.UpdatePixelShaderTextureConstants(textures->GetTextureArrayPointer());
	//lightShader.Initialize();
	switch(_shaderToUse)
	{
	case DIFFUSE_SHADER:
		{
			SetShaderUsed(&objectShader);
			break;
		}
	case DEPTH_SHADER:
		{

			break;
		}
	case NORMAL_SHADER:
		{

			break;
		}
	case LIGHT_SHADER:
		{
			//SetShaderUsed(&lightShader);
			break;
		}
	default:
		{

		}
	}

	UpdateWorldMatrix();
	bool hasTexture = true;
	if(textures->NumberOfTextures() == 0)
	{
		hasTexture = false;
	}
	//FBXLoader::LoadFBX(this, _filePath, hasTexture);
}

void ParentMeshObject::SetShaderUsed(BaseShader* _shaderUsed)
{
	shaderUsed = _shaderUsed;
	shaderUsed->SetShader();
}

void ParentMeshObject::Render()
{
	for(unsigned int i = 0; i < children.size(); ++i)
	{
		objectShader.Update(children[i], textures->GetTextureArrayPointer());
		children[i]->Render(&objectShader, D3D11Renderer::renderTargetView[RENDER_GEOMETRYBUFFER]);
	}
}

void ParentMeshObject::Update(float _dt)
{
	float degrees = 0;
	degrees = 45.0f * _dt;

	//Rotate(degrees, XMFLOAT3(0.0f, 1.0f, 0.0f));
	UpdateWorldMatrix();

	unsigned int childCount = children.size();
	for(unsigned int i = 0; i < childCount; ++i)
	{
		children[i]->Update(_dt);
	}
}

void ParentMeshObject::SwitchRenderMode(int _renderMode)
{
	if(_renderMode == DEPTH_SHADER)
	{
		SetShaderUsed(&depthShader);
	}
	else if(_renderMode == DIFFUSE_SHADER)
	{
		SetShaderUsed(&objectShader);
	}
}

void ParentMeshObject::Rotate(float _degrees, XMFLOAT3 _axis)
{
	_degrees = XMConvertToRadians(_degrees);

	rotation.x += _axis.x * _degrees;
	rotation.y += _axis.y * _degrees;
	rotation.z += _axis.z * _degrees;
}

void ParentMeshObject::AddChild(VertexType* _vertices, unsigned long* _indices, int _numVertices, int _numIdices)
{
	ChildMeshObject* child = new ChildMeshObject();
	child->SetParent(this);
	child->SetIndexCount(_numIdices);
	child->SetVertexCount(_numVertices);
	child->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	child->InitializeBuffers(_vertices, _indices);
	children.push_back(child);
}

void ParentMeshObject::AddTexture(WCHAR* _filePath)
{
	textures->AddTexture(D3D11Renderer::d3dDevice, _filePath);
}

void ParentMeshObject::AddChildTexture(WCHAR* _filePath, int _child)
{
	children[_child]->AddTexture(_filePath);
}

void ParentMeshObject::UpdateWorldMatrix()
{
	XMMATRIX mScale;
	XMMATRIX mTranslation;
	XMMATRIX mRotation;

	mScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	mTranslation = XMMatrixTranslation(position.x, position.y, position.z);

	mRotation = XMMatrixIdentity();
	if(rotation.x != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(1.0f, 0.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), rotation.x);
	}
	if(rotation.y != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), rotation.y);
	}
	if(rotation.z != 0)
	{
		XMFLOAT3 axis = XMFLOAT3(0.0f, 0.0f, 1.0f);
		mRotation *= XMMatrixRotationAxis(XMLoadFloat3(&axis), rotation.z);
	}

	XMMATRIX mWorldMat = XMMatrixIdentity();
	mWorldMat *= mScale;
	mWorldMat *= mRotation;
	mWorldMat *= mTranslation;

	XMStoreFloat4x4(&worldMatrix, mWorldMat);
}