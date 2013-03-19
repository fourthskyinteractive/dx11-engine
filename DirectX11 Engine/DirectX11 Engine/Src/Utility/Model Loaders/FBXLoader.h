#ifndef FBXLOADER_H
#define FBXLOADER_H

#include <fbxsdk.h>

class FBXLoader
{
public:
	FBXLoader();
	FBXLoader(const FBXLoader&);
	~FBXLoader();

	static void Initialize();
	static void GetFbxInfo(FbxNode* _node);

private:
	static FbxManager* fbxManager;
	static FbxIOSettings* fbxIOSettings;
	static FbxImporter* fbxImporter;
	static FbxScene* fbxScene;
};
#endif