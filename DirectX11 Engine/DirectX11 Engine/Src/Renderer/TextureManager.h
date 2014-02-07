#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <d3d11.h>
#include <atlbase.h>
#include <vector>
using namespace std;
//#include <d3dx11tex.h>

class TextureManager
{
public:
	TextureManager();
	TextureManager(const TextureManager&);
	~TextureManager();

	static int AddTexture(WCHAR* _filename);
	static int AddTexture(CComPtr<ID3D11ShaderResourceView> _shaderResourceView);
	static int NumberOfTextures();
	static CComPtr<ID3D11ShaderResourceView> GetTextureArrayPointer();
	static void Shutdown();


	static CComPtr<ID3D11ShaderResourceView> GetTexture(int _index);

private:
	static vector<CComPtr<ID3D11ShaderResourceView>> textures;
};

#endif