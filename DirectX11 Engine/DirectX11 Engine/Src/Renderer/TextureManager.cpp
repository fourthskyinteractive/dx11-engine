#include "TextureManager.h"
#include "D3D11Renderer.h"
#include "../Utility/DDS Texture Loader/DDSTextureLoader.h"
using namespace DirectX;

vector<CComPtr<ID3D11ShaderResourceView>> TextureManager::textures;


TextureManager::TextureManager()
{

}

TextureManager::TextureManager(const TextureManager& _textureManager)
{

}

TextureManager::~TextureManager()
{
	Shutdown();
}

int TextureManager::AddTexture(WCHAR* _filename)
{
	HRESULT hr;
	ID3D11ShaderResourceView* srv;
	//Load in the texture
	hr = DirectX::CreateDDSTextureFromFile(D3D11Renderer::d3dDevice, _filename, nullptr, &srv);
	if(FAILED(hr))
	{
		return -1;
	}

	textures.push_back(srv);

	return textures.size() - 1;
}

int TextureManager::AddTexture(CComPtr<ID3D11ShaderResourceView> _shaderResourceView)
{
	textures.push_back(_shaderResourceView);
	
	return textures.size() - 1;
}

CComPtr<ID3D11ShaderResourceView> TextureManager::GetTexture(int _index)
{
	return textures[_index];
}

CComPtr<ID3D11ShaderResourceView> TextureManager::GetTextureArrayPointer()
{
	if(textures.size() > 0)
	{
		return textures[0];
	}
	return NULL;
}

int TextureManager::NumberOfTextures()
{
	return textures.size();
}

void TextureManager::Shutdown()
{
	textures.clear();

	return;
}