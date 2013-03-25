#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <d3d11.h>
#include <vector>
using namespace std;
//#include <d3dx11tex.h>

class TextureManager
{
public:
	TextureManager();
	TextureManager(const TextureManager&);
	~TextureManager();

	int AddTexture(ID3D11Device* _device, WCHAR* _filename);
	int NumberOfTextures();
	ID3D11ShaderResourceView* GetTextureArrayPointer();
	void Shutdown();


	ID3D11ShaderResourceView* GetTexture(int _index);

private:
	vector<ID3D11ShaderResourceView*> textures;
};

#endif