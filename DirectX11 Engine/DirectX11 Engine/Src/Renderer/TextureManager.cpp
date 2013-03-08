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

	return textures.size() - 1;
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