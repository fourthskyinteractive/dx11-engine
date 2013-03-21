#include "FBXLoader.h"

#include "../../Game/Definitions.h"
#include "../../Game/Game.h"

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

bool FBXLoader::LoadFBX(char* _filePath, Mesh::VertexType** _verticesPtr, unsigned long** _indicesPtr, int& _numVertices, int& _numIndices)
{
	FbxManager* fbxManager = FbxManager::Create();
	if(!fbxManager)
	{
		printf( "ERROR %s : %d failed creating FBX Manager!\n", __FILE__, __LINE__ );
	}

	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);

	FbxString filePath = FbxGetApplicationDirectory();
	fbxManager->LoadPluginsDirectory(filePath.Buffer());

	FbxScene* scene = FbxScene::Create(fbxManager, "");

	int fileMinor, fileRevision;
	int sdkMajor, sdkMinor, sdkRevision;
	int fileFormat;

	FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);
	FbxImporter* importer = FbxImporter::Create(fbxManager, "");

	if(!fbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(_filePath, fileFormat))
	{
		//Unrecognizable file format. Try to fall back on FbxImorter::eFBX_BINARY
		fileFormat = fbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
	}

	bool importStatus = importer->Initialize(_filePath, fileFormat, fbxManager->GetIOSettings());
	importer->GetFileVersion(fileMinor, fileMinor, fileRevision);

	if(!importStatus)
	{
		printf( "ERROR %s : %d FbxImporter Initialize failed!\n", __FILE__, __LINE__ );
		return false;
	}

	importStatus = importer->Import(scene);

	if(!importStatus)
	{
		printf( "ERROR %s : %d FbxImporter failed to import the file to the scene!\n", __FILE__, __LINE__ );
		return false;
	}

	FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem axisSystem = FbxAxisSystem::DirectX;

	if(sceneAxisSystem != axisSystem)
	{
		axisSystem.ConvertScene(scene);
	}

	TriangulateRecursive(scene->GetRootNode());

	FbxArray<FbxMesh*> meshes;
	FillMeshArray(scene, meshes);

	unsigned short vertexCount = 0;
	unsigned short triangleCount = 0;
	unsigned short faceCount = 0;
	unsigned short materialCount = 0;

	Mesh::VertexType* vertices;
	int numberOfVertices = 0;
	for(int i = 0; i < meshes.GetCount(); ++i)
	{
		numberOfVertices += meshes[i]->GetPolygonVertexCount();		
	}
	
	vertices = new Mesh::VertexType[numberOfVertices];

	Face face;
	vector<Face> faces;
	numberOfVertices = 0;
	int indicesCount = 0;

	for(int i = 0; i < meshes.GetCount(); ++i)
	{
		int vertexCount = 0;
		vertexCount = meshes[i]->GetControlPointsCount();
		if(vertexCount == 0)
			continue;

		int triangleCount = meshes[i]->GetPolygonVertexCount() / 3;
		indicesCount = meshes[i]->GetPolygonVertexCount();

		FbxVector4* fbxVerts = new FbxVector4[vertexCount];
		int arrayIndex = 0;
		memcpy(fbxVerts, meshes[i]->GetControlPoints(), vertexCount * sizeof(FbxVector4));

		for(int j = 0; j < triangleCount; ++j)
		{
			int index = 0;
			FbxVector4 fbxNorm(0, 0, 0, 0);
			FbxVector2 fbxUV(0, 0);

			face.indices[0] = index = meshes[i]->GetPolygonVertex(j, 0);
			vertices[index].position.x = (float)fbxVerts[index - numberOfVertices][0];
			vertices[index].position.y = (float)fbxVerts[index - numberOfVertices][1];
			vertices[index].position.z = (float)fbxVerts[index - numberOfVertices][2];
			vertices[index].position.w = 0;
			meshes[i]->GetPolygonVertexNormal(j, 0, fbxNorm);
			vertices[index].normal.x = (float)fbxNorm[0];
			vertices[index].normal.y = (float)fbxNorm[1];
			vertices[index].normal.z = (float)fbxNorm[2];
			meshes[i]->GetPolygonVertexUV(j, 0, "map1", fbxUV);
			vertices[index].texture.x = (float)fbxUV[0];
			vertices[index].texture.y = (float)fbxUV[1];

			face.indices[1] = index = meshes[i]->GetPolygonVertex(j, 1);
			vertices[index].position.x = (float)fbxVerts[index - numberOfVertices][0];
			vertices[index].position.y = (float)fbxVerts[index - numberOfVertices][1];
			vertices[index].position.z = (float)fbxVerts[index - numberOfVertices][2];
			vertices[index].position.w = 0;
			meshes[i]->GetPolygonVertexNormal(j, 1, fbxNorm);
			vertices[index].normal.x = (float)fbxNorm[0];
			vertices[index].normal.y = (float)fbxNorm[1];
			vertices[index].normal.z = (float)fbxNorm[2];
			meshes[i]->GetPolygonVertexUV(j, 1, "map1", fbxUV);
			vertices[index].texture.x = (float)fbxUV[0];
			vertices[index].texture.y = (float)fbxUV[1];

			face.indices[2] = index = meshes[i]->GetPolygonVertex(j, 2);
			vertices[index].position.x = (float)fbxVerts[index - numberOfVertices][0];
			vertices[index].position.y = (float)fbxVerts[index - numberOfVertices][1];
			vertices[index].position.z = (float)fbxVerts[index - numberOfVertices][2];
			vertices[index].position.w = 0;
			meshes[i]->GetPolygonVertexNormal(j, 2, fbxNorm);
			vertices[index].normal.x = (float)fbxNorm[0];
			vertices[index].normal.y = (float)fbxNorm[1];
			vertices[index].normal.z = (float)fbxNorm[2];
			meshes[i]->GetPolygonVertexUV(j, 2, "map1", fbxUV);
			vertices[index].texture.x = (float)fbxUV[0];
			vertices[index].texture.y = (float)fbxUV[1];

			faces.push_back(face);
		}

		meshes[i]->Destroy();
		meshes[i] = NULL;

		numberOfVertices += triangleCount * 3;
	}

	unsigned long* indices = new unsigned long[faces.size() * 3]; 
	int indicie = 0;
	for(unsigned int i = 0; i < faces.size(); ++i)
	{
		indices[indicie++] = faces[i].indices[0];
		indices[indicie++] = faces[i].indices[1];
		indices[indicie++] = faces[i].indices[2];

	}

	_numVertices = numberOfVertices;
	_numIndices = indicesCount;

	*_verticesPtr = vertices;
	*_indicesPtr = indices;

// 	for(int i = 0; i < faces.size(); ++i)
// 	{
// 		XMVECTOR v0 = XMLoadFloat4(&vertices[faces[i].indices[2]].position);
// 		XMVECTOR v1 = XMLoadFloat4(&vertices[faces[i].indices[1]].position);
// 		XMVECTOR v2 = XMLoadFloat4(&vertices[faces[i].indices[0]].position);
// 
// 		
// 
// 		XMVECTOR v10 = v0 - v1;
// 		XMVECTOR v12 = v2 - v1;
// 
// 		XMVECTOR f3Normal;
// 		XMVector4Cross(f3Normal, v12, v10);
// 		XMVector4Normalize(f3Normal);
// 
// 		XMFLOAT4 norm;
// 		XMStoreFloat4(&norm, f3Normal);
// 	}

// 	//Open the output file
// 	ofstream fout;
// 
// 	char* modelName = GetModelName(_filePath);
// 	string newFilename = "Res/Objects/";
// 	newFilename += modelName;
// 	newFilename[newFilename.size() - 4] = '.';
// 	newFilename[newFilename.size() - 3] = 't';
// 	newFilename[newFilename.size() - 2] = 'x';
// 	newFilename[newFilename.size() - 1] = 't';
// 	newFilename[newFilename.size()] = '\0';
// 	fout.open(newFilename);
// 	int numFaces = faces.size();
// 	//Write out the file header that our model format uses.
// 	fout << "Vertex Count:" << (numFaces * 3) << endl;
// 	fout << endl;
// 	fout << "Uses BumpMap ";
// 	fout << true;
// 	fout << endl;
// 	fout << "Data:" << endl;
// 	fout << endl;
// 	int vIndex = 0;
// 	int tIndex = 0;
// 	int nIndex = 0;
// 
// 	// Now loop through all the faces and output the three vertices for each face.
// 	for(int i=0; i<numFaces; i++)
// 	{
// 		fout << vertices[faces[i].indices[0]].position.x;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].position.y;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].position.z;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].position.w;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].texture.x;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].texture.y;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].normal.x;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].normal.y;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[0]].normal.z;
// 
// 		fout << endl;		
// 
// 		fout << vertices[faces[i].indices[1]].position.x;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].position.y;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].position.z;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].position.w;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].texture.x;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].texture.y;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].normal.x;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].normal.y;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[1]].normal.z;
// 
// 		fout << endl;
// 
// 		fout << vertices[faces[i].indices[2]].position.x;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[2]].position.y;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[2]].position.z;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[2]].position.w;
// 		fout << ' ';
// 		fout << vertices[faces[i].indices[2]].texture.x;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[2]].texture.y;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[2]].normal.x;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[2]].normal.y;
// 		fout << ' ';						  
// 		fout << vertices[faces[i].indices[2]].normal.z;
// 
// 		fout << endl;
// 	}
// 
// 	// Close the output file.
// 	fout.close();
// 
// 	importer->Destroy();
// 	importer = NULL;
// 
// 	scene->Destroy();
// 	scene = NULL;
// 
// 	ioSettings->Destroy();
// 	ioSettings = NULL;
// 
// 	fbxManager->Destroy();
// 	fbxManager = NULL;

	return true;
}

void FBXLoader::FillMeshArray(FbxScene* _scene, FbxArray<FbxMesh*>& _meshArray)
{
	_meshArray.Clear();
	FillMeshArrayRecursive(_scene->GetRootNode(), _meshArray);
}

void FBXLoader::TriangulateRecursive(FbxNode* _node)
{
	FbxNodeAttribute* nodeAttr = _node->GetNodeAttribute();

	if(nodeAttr)
	{
		if( nodeAttr->GetAttributeType() == FbxNodeAttribute::eMesh ||
			nodeAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			nodeAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
			nodeAttr->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometryConverter converter(_node->GetFbxManager());
			converter.TriangulateInPlace(_node);
		}
	}

	const int chileCould = _node->GetChildCount();
	for(int i = 0; i < chileCould; ++i)
	{
		TriangulateRecursive(_node->GetChild(i));
	}
}

void FBXLoader::FillMeshArrayRecursive(FbxNode* _node, FbxArray<FbxMesh*>& _meshArray)
{
	if(_node)
	{
		if(_node->GetNodeAttribute())
		{
			if(_node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				_meshArray.Add(_node->GetMesh());
			}
		}

		const int count = _node->GetChildCount();
		for(int i = 0; i < count; ++i)
		{
			FillMeshArrayRecursive(_node->GetChild(i), _meshArray);
		}
	}
}

char* FBXLoader::GetModelName(char* _filename)
{
	int currentIndex = 0;
	int lastSlash = 0;

	char letter = _filename[currentIndex];
	while(letter != '.')
	{
		if(letter == '/')
		{
			lastSlash = currentIndex;
		}

		currentIndex ++;
		letter = _filename[currentIndex];
	}

	_filename += lastSlash + 1;
	return _filename;
}