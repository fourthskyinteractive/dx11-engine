#ifndef FBXLOADER_H
#define FBXLOADER_H

#include <fbxsdk.h>

class FBXLoader
{
public:
	FBXLoader();
	FBXLoader(const FBXLoader&);
	~FBXLoader();

	static bool LoadFBX(char* _filePath);
	static void FillMeshArray(FbxScene* _scene, FbxArray<FbxMesh*>& _meshArray);
	static void FillMeshArrayRecursive(FbxNode* _node, FbxArray<FbxMesh*>& _meshArray);

private:
};
#endif