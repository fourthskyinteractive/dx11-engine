#include "TextureManager.h"
#include <DDSTextureLoader.h>
using namespace DirectX;

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

int TextureManager::AddTexture(ID3D11Device* _device, WCHAR* _filename)
{
	HRESULT hr;
	ID3D11ShaderResourceView* srv;
	//Load in the texture
	hr = CreateDDSTextureFromFile(_device, _filename, nullptr, &srv);
	if(FAILED(hr))
	{
		return -1;
	}

	textures.push_back(srv);

	return textures.size() - 1;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(int _index)
{
	return textures[_index];
}

void* TextureManager::GetTextureArrayPointer()
{
	return &textures[0];
}

int TextureManager::NumberOfTextures()
{
	return textures.size();
}

void TextureManager::Shutdown()
{
	for(unsigned int i =  0; i < textures.size(); ++i)
	{
		if(textures[i])
		{
			textures[i]->Release();
			textures[i] = 0;
		}
	}

	textures.clear();

	return;
}