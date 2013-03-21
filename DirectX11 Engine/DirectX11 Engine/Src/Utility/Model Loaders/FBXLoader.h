#ifndef FBXLOADER_H
#define FBXLOADER_H

#include <fbxsdk.h>

#include "../../Game Objects/Mesh.h"

class FBXLoader
{
public:
	FBXLoader();
	FBXLoader(const FBXLoader&);
	~FBXLoader();

	static bool LoadFBX(char* _filePath, Mesh::VertexType** _verticesPtr, unsigned long** _indicesPtr, int& _numVertices, int& _numIndices);
	static void FillMeshArray(FbxScene* _scene, FbxArray<FbxMesh*>& _meshArray);
	static void FillMeshArrayRecursive(FbxNode* _node, FbxArray<FbxMesh*>& _meshArray);
	static char* GetModelName(char* _filePath);
	static void TriangulateRecursive(FbxNode* _node);
	//static void CalculateNormals(Mesh::VertexType* _vertices);

	struct Face
	{
		int indices[3];
	};

private:
	

};
#endif