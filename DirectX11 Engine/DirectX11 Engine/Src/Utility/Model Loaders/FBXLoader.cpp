#include "FBXLoader.h"

#include "../../Game/Definitions.h"
#include "../../Game/Game.h"
#include "../../Game Objects/Mesh.h"

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

bool FBXLoader::LoadFBX(char* _filePath)
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
	int i, fileFormat;

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

	FbxArray<FbxMesh*> meshes;
	FillMeshArray(scene, meshes);

	unsigned short vertexCount = 0;
	unsigned short triangleCount = 0;
	unsigned short faceCount = 0;
	unsigned short materialCount = 0;
	unsigned short indicesCount = 0;

	

	return true;
}

void FBXLoader::FillMeshArray(FbxScene* _scene, FbxArray<FbxMesh*>& _meshArray)
{
	_meshArray.Clear();
	FillMeshArrayRecursive(_scene->GetRootNode(), _meshArray);
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