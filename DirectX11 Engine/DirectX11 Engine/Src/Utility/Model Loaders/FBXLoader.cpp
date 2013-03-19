#include "FBXLoader.h"

#include "../../Game/Definitions.h"
#include "../../Game/Game.h"

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

FbxManager*		FBXLoader::fbxManager = NULL;
FbxIOSettings*	FBXLoader::fbxIOSettings = NULL;
FbxImporter*	FBXLoader::fbxImporter = NULL;
FbxScene*		FBXLoader::fbxScene = NULL;

void FBXLoader::Initialize()
{
	fbxManager = FbxManager::Create();

	const char* filename = "Res/Models/soldier.fbx";

	fbxIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(fbxIOSettings);

	fbxImporter = FbxImporter::Create(fbxManager, "FBXIMPORTER");

	if(!fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings()))
	{
		bool ErrorHERE = true;
	}

	fbxScene = FbxScene::Create(fbxManager, "MyScene");

	fbxImporter->Import(fbxScene);

	fbxImporter->Destroy();

	FbxNode* rootNode = fbxScene->GetRootNode();

	if(rootNode)
	{
		GetFbxInfo(rootNode);
	}
}

void FBXLoader::GetFbxInfo(FbxNode* _node)
{
	int numKids = _node->GetChildCount();
	FbxNode* childNode = NULL;
	XMFLOAT4* verts;
	int* indices;

	for(int i = 0; i < numKids; ++i)
	{
		childNode = _node->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();

		if(mesh != NULL)
		{
			//Get the verts
			int numVerts = mesh->GetControlPointsCount();
			verts = new XMFLOAT4[numVerts];
			for(int j = 0; j < numVerts; ++j)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				verts[j].x = vert.mData[0];
				verts[j].y = vert.mData[1];
				verts[j].z = vert.mData[2];
			}

			int numIndices = mesh->GetPolygonVertexCount();
			indices = new int[numIndices];
			indices = mesh->GetPolygonVertices();

			int polyCount = mesh->GetPolygonCount();
			for(int j = 0; j < polyCount; ++j)
			{
				FbxLayerElementArrayTemplate<FbxVector2>* uvVertices = 0;
				mesh->GetTextureUV(&uvVertices, FbxLayerElement::eTextureDiffuse);

				for(int k = 0; k < mesh->GetPolygonSize(j); ++k)
				{
					//FbxVector2 uv = uvVertices[mesh->GetTextureUVIndex(j, k)];
				}
			}
		}
	}

	bool win = true;
}