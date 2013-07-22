#include "FBXLoader.h"

#include "../../Game/Definitions.h"
#include "../../Game/Game.h"

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
using namespace std;

bool FBXLoader::LoadFBX(ParentMeshObject* _parentMesh, char* _filePath, bool _hasTexture)
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
	FbxAxisSystem axisSystem( FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded );

	if(sceneAxisSystem != axisSystem)
	{
		axisSystem.ConvertScene(scene);
	}

	TriangulateRecursive(scene->GetRootNode());

	FbxAnimEvaluator* mySceneEvaluator = scene->GetEvaluator();

	FbxTime myTime;

	myTime.SetSecondDouble(1.0);

	FbxNode* myMeshNode = FbxNode::Create(scene, "");

	
	static int numNodes = 0;
	//GETTING ANIMAION DATA
	for(int i = 0; i < scene->GetSrcObjectCount<FbxAnimStack>(); ++i)
	{
		FbxAnimStack* lAnimStack = scene->GetSrcObject<FbxAnimStack>(i);

		FbxString stackName = "Animation Stack Name: ";
		stackName += lAnimStack->GetName();
		string sStackName = stackName;

		int numLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();

		for(int j = 0; j < numLayers; ++j)
		{
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);

			FbxString layerName = "Animation Stack Name: ";
			layerName += lAnimLayer->GetName();
			string sLayerName = layerName;

			double weight = lAnimLayer->Weight;

			queue<FbxNode*> nodes;

			FbxNode* tempNode = scene->GetRootNode();
			FbxString nodeName = "Animation Stack Name: ";
			layerName += tempNode->GetName();
			string sNodeName = layerName;

			numNodes ++;

			while(tempNode != NULL)
			{

				//THIS SHIT RIGHT HERE IS CLOSE NIGGEH
				FbxString nodeName = "";
				nodeName += tempNode->GetName();
				string sNodeName = nodeName;

				FbxNodeAttribute* nodeAttribute = tempNode->GetNodeAttribute();
				
				FbxNodeAttribute::EType attributeType;

				if(nodeAttribute)
					attributeType = nodeAttribute->GetAttributeType();

				FbxMatrix globalMatrix = mySceneEvaluator->GetNodeLocalTransform(tempNode, myTime);
				
				FbxGeometry* nodeGeometry = tempNode->GetGeometry();

				FbxGeometryWeightedMap* weightMap;
				if(nodeGeometry)
					 weightMap = nodeGeometry->GetSourceGeometryWeightedMap();

				FbxAnimCurve* lAnimCurve = tempNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);

				if(lAnimCurve != NULL)
				{
					
				}

				for(int i = 0; i < tempNode->GetChildCount(false); ++i)
				{
					nodes.push(tempNode->GetChild(i));
					numNodes++;

				}

				if(nodes.size() > 0)
				{
					tempNode = nodes.front();
					nodes.pop();
				}
				else
				{
					tempNode = NULL;
				}
			}               
		}       
	}

	FbxArray<FbxMesh*> meshes;
	FillMeshArray(scene, meshes);

	unsigned short vertexCount = 0;
	unsigned short triangleCount = 0;
	unsigned short faceCount = 0;
	unsigned short materialCount = 0;

	int numberOfVertices = 0;
	for(int i = 0; i < meshes.GetCount(); ++i)
	{
		numberOfVertices += meshes[i]->GetPolygonVertexCount();
	}	

	Face face;
	vector<Face> faces;
	int indicesCount = 0;

	int ptrMove = 0;

	float wValue = 0.0f;

	if(!_hasTexture)
	{
		wValue = 1.0f;
	}

	for(int i = 0; i < meshes.GetCount(); ++i)
	{
		int vertexCount = 0;
		vertexCount = meshes[i]->GetControlPointsCount();
		if(vertexCount == 0)
			continue;

		VertexType* vertices;
		vertices = new VertexType[vertexCount];

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
			bool texCoordFound = false;
			face.indices[0] = index = meshes[i]->GetPolygonVertex(j, 0);
			vertices[index].position.x = (float)fbxVerts[index][0];
			vertices[index].position.y = (float)fbxVerts[index][1];
			vertices[index].position.z = (float)fbxVerts[index][2];
			vertices[index].position.w = wValue;
			meshes[i]->GetPolygonVertexNormal(j, 0, fbxNorm);
			vertices[index].normal.x = (float)fbxNorm[0];
			vertices[index].normal.y = (float)fbxNorm[1];
			vertices[index].normal.z = (float)fbxNorm[2];
			texCoordFound = meshes[i]->GetPolygonVertexUV(j, 0, "map1", fbxUV);
			vertices[index].texture.x = (float)fbxUV[0];
			vertices[index].texture.y = 1.0f - (float)fbxUV[1];

			face.indices[1] = index = meshes[i]->GetPolygonVertex(j, 1);
			vertices[index].position.x = (float)fbxVerts[index][0];
			vertices[index].position.y = (float)fbxVerts[index][1];
			vertices[index].position.z = (float)fbxVerts[index][2];
			vertices[index].position.w = wValue;
			meshes[i]->GetPolygonVertexNormal(j, 1, fbxNorm);
			vertices[index].normal.x = (float)fbxNorm[0];
			vertices[index].normal.y = (float)fbxNorm[1];
			vertices[index].normal.z = (float)fbxNorm[2];
			texCoordFound = meshes[i]->GetPolygonVertexUV(j, 1, "map1", fbxUV);
			vertices[index].texture.x = (float)fbxUV[0];
			vertices[index].texture.y = 1.0f - (float)fbxUV[1];

			face.indices[2] = index = meshes[i]->GetPolygonVertex(j, 2);
			vertices[index].position.x = (float)fbxVerts[index][0];
			vertices[index].position.y = (float)fbxVerts[index][1];
			vertices[index].position.z = (float)fbxVerts[index][2];
			vertices[index].position.w = wValue;
			meshes[i]->GetPolygonVertexNormal(j, 2, fbxNorm);
			vertices[index].normal.x = (float)fbxNorm[0];
			vertices[index].normal.y = (float)fbxNorm[1];
			vertices[index].normal.z = (float)fbxNorm[2];
			texCoordFound = meshes[i]->GetPolygonVertexUV(j, 2, "map1", fbxUV);
			vertices[index].texture.x = (float)fbxUV[0];
			vertices[index].texture.y = 1.0f - (float)fbxUV[1];

			faces.push_back(face);
		}

		meshes[i]->Destroy();
		meshes[i] = NULL;

		int indexCount = faces.size() * 3;
		unsigned long* indices = new unsigned long[faces.size() * 3]; 
		int indicie = 0;
		for(unsigned int i = 0; i < faces.size(); ++i)
		{
			indices[indicie++] = faces[i].indices[0];
			indices[indicie++] = faces[i].indices[1];
			indices[indicie++] = faces[i].indices[2];
		}
		faces.clear();
		_parentMesh->AddChild(vertices, indices, vertexCount, indexCount);
	}

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