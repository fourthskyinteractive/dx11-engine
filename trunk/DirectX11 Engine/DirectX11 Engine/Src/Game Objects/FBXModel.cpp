#include "FBXModel.h"
#include "../Utility/Misc/DebugOutput.h"
#include "../Renderer/TextureManager.h"
#include "../Game/Definitions.h"

namespace
{
	bool LoadTextureFromFile(const FbxString& _fileName, vector<int>& _textureIndicies)
	{
		if(_fileName.Right(3).Upper() == "DDS")
		{
			int textureIndex = TextureManager::AddTexture(ConvertConstCharToWChar(_fileName.Buffer()));
			if(textureIndex >= 0)
			{
				_textureIndicies.push_back(textureIndex);
				return true;
			}
		}

		return false;
	}

	void FillCameraArrayRecursive(FbxNode* _node, FbxArray<FbxNode*>& _cameraArray)
	{
		if(_node)
		{
			if(_node->GetNodeAttribute())
			{
				if(_node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera)
				{
					_cameraArray.Add(_node);
				}
			}

			const int count = _node->GetChildCount();
			for(int i = 0; i < count; ++i)
			{
				FillCameraArrayRecursive(_node->GetChild(i), _cameraArray);
			}
		}
	}

	void FillCameraArray(FbxScene* _scene, FbxArray<FbxNode*>& _cameraArray)
	{
		_cameraArray.Clear();

		FillCameraArrayRecursive(_scene->GetRootNode(), _cameraArray);
	}

	void LoadCacheRecursive(FbxNode* _node, FbxAnimLayer* _animationLayer, MeshData* _meshData)
	{
		//Make Material and hook as user data
		//TODO:FINISH FBX MATERIAL STUFF
		const int materialCount = _node->GetMaterialCount();
		for(int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			FbxSurfaceMaterial* material = _node->GetMaterial(materialIndex);
			if(material && !material->GetUserDataPtr())
			{
				FbxAutoPtr<MaterialData> materialCache(new MaterialData);
				if(materialCache->Initialize(material))
				{
					material->SetUserDataPtr(materialCache.Release());
				}
			}
		}

		FbxNodeAttribute* nodeAttribute = _node->GetNodeAttribute();
		if(nodeAttribute)
		{
			if(nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				FbxMesh* mesh = _node->GetMesh();
				if(mesh && !mesh->GetUserDataPtr())
				{
					_meshData->Initialize(mesh);
					FbxAutoPtr<MeshData> meshData(new MeshData);
					if(meshData->Initialize(mesh))
					{
						mesh->SetUserDataPtr(meshData.Release());
					}
				}
			}
			else if(nodeAttribute->GetAttributeType() == FbxNodeAttribute::eLight)
			{
				FbxLight* light = _node->GetLight();
				if(light && !light->GetUserDataPtr())
				{
					//TODO:FINISH FBX LIGHT STUFF
				}
			}
		}

		const int childCount = _node->GetChildCount();
		for(int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			LoadCacheRecursive(_node->GetChild(childIndex), _animationLayer, _meshData);
		}
	}

	void LoadCacheRecursive(FbxScene * _scene, FbxAnimLayer * _animationLayer, const char * _fbxFileName, vector<int>& _textureIndicies, MeshData* _meshData)
	{
		const int textureCount = _scene->GetTextureCount();
		for(int i = 0; i < textureCount; ++i)
		{
			FbxTexture* texture = _scene->GetTexture(i);
			FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);
			if(fileTexture && !fileTexture->GetUserDataPtr())
			{
				const FbxString textureFilePath = fileTexture->GetFileName();

				if(textureFilePath.Right(3).Upper() != "DDS")
				{
					DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_GENERIC, "Only DDS textures are supported!");
					continue;
				}

				bool loadingStatus = LoadTextureFromFile(textureFilePath, _textureIndicies);

				const FbxString absFbxFileName = FbxPathUtils::Resolve(_fbxFileName);
				const FbxString absFolderName = FbxPathUtils::GetFolderName(absFbxFileName);
				if(!loadingStatus)
				{
					//Load texture from relativ efile name (relative to FBX file)
					const FbxString resolvedFileName = FbxPathUtils::Bind(absFolderName, fileTexture->GetRelativeFileName());
					loadingStatus = LoadTextureFromFile(resolvedFileName, _textureIndicies);
				}

				if(!loadingStatus)
				{
					//Load texture from file name only (relative to FBX file)
					const FbxString textureFileName = FbxPathUtils::GetFileName(textureFilePath);
					const FbxString resolvedFileName = FbxPathUtils::Bind(absFolderName, textureFileName);
					loadingStatus = LoadTextureFromFile(resolvedFileName, _textureIndicies);
				}

				if(!loadingStatus)
				{
					string failedMessage = "Failed to load texture ";
					failedMessage += textureFilePath.Buffer();
					DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_GENERIC, failedMessage.c_str());
				}

				if(loadingStatus)
				{
					texture->SetUserDataPtr(&_textureIndicies[_textureIndicies.size() - 1]);
				}
			}
		}

		LoadCacheRecursive(_scene->GetRootNode(), _animationLayer, _meshData);
	}
}

FBXModel::FBXModel(const char* _fileName)
	: fileName(_fileName), status(UNLOADED),
	fbxManager(NULL), scene(NULL), importer(NULL), currentAnimLayer(NULL),
	selectedNode(NULL), poseIndex(-1), pauseAnimation(false)
{

	if(fileName == NULL)
	{
		return;
	}

	cacheStartTime = FBXSDK_TIME_INFINITE;
	cacheStopTime = FBXSDK_TIME_MINUS_INFINITE;

	fbxManager = FbxManager::Create();
	if(!fbxManager)
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_CREATE, "FBX Manager");
	}
	else
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::SUCCESS_CREATE, "FBX Manager");
	}

	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	if(!ios)
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_CREATE, "FBXIOS Settings");
	}
	else
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::SUCCESS_CREATE, "FBXIOS Settings");
	}

	FbxString path = FbxGetApplicationDirectory();
	fbxManager->LoadPluginsDirectory(path.Buffer());
	if(!path)
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_GENERIC, "Load FBX Plugins");
	}
	else
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::SUCCESS_GENERIC, "Load FBX Plugins");
	}

	scene = FbxScene::Create(fbxManager, "FBX Scene");
	if(!scene)
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_CREATE, "FBX Scene");
	}
	else
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::SUCCESS_CREATE, "FBX Scene");
	}

	if(fbxManager)
	{
		int lFileFormat = -1;
		importer = FbxImporter::Create(fbxManager, "");

		if(!fbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(fileName, lFileFormat))
		{
			DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_GENERIC, "File format not recognized.  Falling back on FBX binary(*.fbx)");
		}

		if(!importer->Initialize(fileName, lFileFormat))
		{
			DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_CREATE, "FBX Importer");
		}
		else
		{
			DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::SUCCESS_CREATE, "FBX Importer");

			status = MUST_BE_LOADED;
		}
	}

	LoadFile();
}

FBXModel::~FBXModel()
{

}

bool FBXModel::LoadFile()
{
	bool result = false;

	if(status == MUST_BE_LOADED)
	{
		if(importer->Import(scene))
		{
			status = MUST_BE_REFRESHED;

			FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem axisSystem( FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded );

			if(sceneAxisSystem != axisSystem)
			{
				axisSystem.ConvertScene(scene);
			}

			FbxSystemUnit sceneSystemUnit = scene->GetGlobalSettings().GetSystemUnit();
			if(sceneSystemUnit.GetScaleFactor() != 1.0f)
			{
				FbxSystemUnit::cm.ConvertScene(scene);
			}

			scene->FillAnimStackNameArray(animStackNameArray);

			FillCameraArray(scene, cameraArray);

			FbxGeometryConverter geometryConverter(fbxManager);
			geometryConverter.Triangulate(scene, true);

			geometryConverter.SplitMeshesPerMaterial(scene, true);

			meshData = new MeshData;
			LoadCacheRecursive(scene, currentAnimLayer, fileName, textureIndices, meshData);
		}
	}

	return result;
}

//bool FBXModel::SetCurrentAnimStack(int _index)
//{
//
//}
//
//bool FBXModel::SetCurrentPoseIndex(int _poseIndex)
//{
//
//}
//
//bool FBXModel::SetSelectedNode(FbxNode* _selectedNode)
//{
//
//}

MaterialData::MaterialData()
{
	//TODO:FINISH FBX MATERIAL STUFF
}

MaterialData::~MaterialData()
{
	//TODO:FINISH FBX MATERIAL STUFF
}

bool MaterialData::Initialize(const FbxSurfaceMaterial* _material)
{
	//TODO:FINISH FBX MATERIAL STUFF
	return true;
}

void MaterialData::SetCurrentMaterial() const
{
	//TODO:FINISH FBX MATERIAL STUFF
}

void MaterialData::SetDefaultMaterial()
{
	//TODO:FINISH FBX MATERIAL STUFF
}


MeshData::MeshData() : hasNormal(false), hasUV(false), saveByControlPoint(true)
{
	for(int i = 0; i < VBO_COUNT; ++i)
	{
		vboNames[i] = 0;
	}
}

MeshData::~MeshData()
{
	for(int i = 0; i < subMeshes.GetCount(); ++i)
	{
		delete subMeshes[i];
	}

	subMeshes.Clear();
}

bool MeshData::Initialize(const FbxMesh* _mesh)
{
	if(!_mesh->GetNode())
	{
		return false;
	}

	const int polygonCount = _mesh->GetPolygonCount();

	//Count the polygon count for each material
	FbxLayerElementArrayTemplate<int>* materialIndicie = NULL;
	FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;
	if(_mesh->GetElementMaterial())
	{
		materialIndicie = &_mesh->GetElementMaterial()->GetIndexArray();
		materialMappingMode = _mesh->GetElementMaterial()->GetMappingMode();
		if(materialIndicie && materialMappingMode == FbxGeometryElement::eByPolygon)
		{
			FBX_ASSERT(materialIndicie->GetCount() == polygonCount)
			if(materialIndicie->GetCount() == polygonCount)
			{
				//Count the faces of each material
				for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
				{
					const int materialIndex = materialIndicie->GetAt(polygonIndex);
					if(subMeshes.GetCount() < materialIndex + 1)
					{
						subMeshes.Resize(materialIndex + 1);
					}
					if(subMeshes[materialIndex] == NULL)
					{
						subMeshes[materialIndex] = new SubMesh();
					}
					subMeshes[materialIndex]->triangleCount += 1;
				}

				//Make sure we have no "holes" (NULL) in the subMeshes table.  This can happen
				//if, in the loop above, we resized the subMeshes by more than one slot
				for(int i = 0; i < subMeshes.GetCount(); ++i)
				{
					if(subMeshes[i] == NULL)
						subMeshes[i] = new SubMesh();
				}

				//Record the offset (how many verts
				const int materialCount = subMeshes.GetCount();
				int offset = 0;
				for(int i = 0; i < materialCount; ++i)
				{
					subMeshes[i]->indexOffset = offset;
					offset += subMeshes[i]->triangleCount * 3;
					subMeshes[i]->triangleCount = 0;
				}
				FBX_ASSERT(offset == polygonCount);
			}
		}
	}

	//All faces will use the same material
	if(subMeshes.GetCount() == 0)
	{
		subMeshes.Resize(1);
		subMeshes[0] = new SubMesh();
	}

	//Congregate all the data of a mesh to be cached in VBO's
	//If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex
	hasNormal = _mesh->GetElementNormalCount() > 0;
	hasUV = _mesh->GetElementUVCount() > 0;
	FbxGeometryElement::EMappingMode normalMappingMode = FbxGeometryElement::eNone;
	FbxGeometryElement::EMappingMode uvMappingMode = FbxGeometryElement::eNone;
	if(hasNormal)
	{
		normalMappingMode = _mesh->GetElementNormal(0)->GetMappingMode();
		if(normalMappingMode == FbxGeometryElement::eNone)
		{
			hasNormal = false;
		}
		if(hasNormal && normalMappingMode != FbxGeometryElement::eByControlPoint)
		{
			saveByControlPoint = false;
		}
	}
	if(hasUV)
	{
		uvMappingMode = _mesh->GetElementUV(0)->GetMappingMode();
		if(uvMappingMode == FbxGeometryElement::eNone)
		{
			hasUV = false;
		}
		if(hasUV && uvMappingMode != FbxGeometryElement::eByControlPoint)
		{
			saveByControlPoint = false;
		}
	}

	int polygonVertexCount = _mesh->GetControlPointsCount();
	if(!saveByControlPoint)
	{
		polygonVertexCount = polygonCount * 3;
	}

	positions.resize(polygonVertexCount);
	indices.resize(polygonCount * 3);
	if(hasNormal)
	{
		normals.resize(polygonVertexCount);
	}
	FbxStringList uvNames;
	_mesh->GetUVSetNames(uvNames);
	const char* uvName = NULL;
	if(hasUV)
	{
		UVs.resize(polygonVertexCount);
		uvName = uvNames[0];
	}

	//Populate the vertex attribute vectors
	const FbxVector4* controlPoints = _mesh->GetControlPoints();
	FbxVector4 currentVertex;
	FbxVector4 currentNormal;
	FbxVector2 currentUV;
	if(saveByControlPoint)
	{
		const FbxGeometryElementNormal* normalElement = NULL;
		const FbxGeometryElementUV* uvElement = NULL;
		if(hasNormal)
		{
			normalElement = _mesh->GetElementNormal(0);
		}
		if(hasUV)
		{
			uvElement = _mesh->GetElementUV(0);
		}
		for(int i = 0; i < polygonVertexCount; ++i)
		{
			currentVertex = controlPoints[i];
			XMFLOAT4 position;
			position.x = static_cast<float>(currentVertex[0]);
			position.y = static_cast<float>(currentVertex[1]);
			position.z = static_cast<float>(currentVertex[2]);
			position.w = 1.0f;

			positions[i] = position;

			//Save the Normal
			if(hasNormal)
			{
				int normalIndex = i;
				if(normalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					normalIndex = normalElement->GetIndexArray().GetAt(i);
				}
				currentNormal = normalElement->GetDirectArray().GetAt(normalIndex);
				XMFLOAT3 normal;
				normal.x = static_cast<float>(currentNormal[0]);
				normal.y = static_cast<float>(currentNormal[1]);
				normal.z = static_cast<float>(currentNormal[2]);
				normals[i] = normal;
			}

			//Save the UV
			if(hasUV)
			{
				int uvIndex = i;
				if(uvElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					uvIndex = uvElement->GetIndexArray().GetAt(i);
				}
				currentUV = uvElement->GetDirectArray().GetAt(i);
				XMFLOAT2 uv;
				uv.x = static_cast<float>(currentUV[0]);
				uv.y = static_cast<float>(currentUV[1]);
				UVs[i] = uv;
			}
		}
	}

	int vertexCount = 0;
	for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		//Material for the current face
		int materialIndex = 0;
		if(materialIndicie && materialMappingMode == FbxGeometryElement::eByPolygon)
		{
			materialIndex = materialIndicie->GetAt(polygonIndex);
		}

		//Where should I save the vertex attribute index, according to the material
		const int indexOffset = subMeshes[materialIndex]->indexOffset + subMeshes[materialIndex]->triangleCount * 3;
		for(int verticeIndex = 0; verticeIndex < 3; ++verticeIndex)
		{
			const int controlPointIndex = _mesh->GetPolygonVertex(polygonIndex, verticeIndex);

			if(saveByControlPoint)
			{
				indices[indexOffset + verticeIndex] = static_cast<unsigned long>(controlPointIndex);
			}
			//Populate the vertex attribute vectors, if by polygon vertex
			else
			{
				indices[indexOffset + verticeIndex] = static_cast<unsigned long>(vertexCount);

				currentVertex = controlPoints[controlPointIndex];
				XMFLOAT4 position;
				position.x = static_cast<float>(currentVertex[0]);
				position.y = static_cast<float>(currentVertex[1]);
				position.z = static_cast<float>(currentVertex[2]);
				position.w = 1.0f;
				positions[vertexCount] = position;

				if(hasNormal)
				{
					_mesh->GetPolygonVertexNormal(polygonIndex, verticeIndex, currentNormal);
					XMFLOAT3 normal;
					normal.x = static_cast<float>(currentNormal[0]);
					normal.y = static_cast<float>(currentNormal[1]);
					normal.z = static_cast<float>(currentNormal[2]);
					normals[vertexCount] = normal;
				}

				if(hasUV)
				{
					bool unmappedUV;
					_mesh->GetPolygonVertexUV(polygonIndex, verticeIndex, uvName, currentUV, unmappedUV);
					XMFLOAT2 uv;
					uv.x = static_cast<float>(currentUV[0]);
					uv.y = 1.0f - static_cast<float>(currentUV[1]);
					UVs[vertexCount] = uv;
				}
			}
			vertexCount++;
		}
		subMeshes[materialIndex]->triangleCount += 1;
	}

	return true;
}