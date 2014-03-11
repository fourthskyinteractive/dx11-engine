#include "FBXModel.h"
#include "../Utility/Misc/DebugOutput.h"
#include "../Renderer/TextureManager.h"
#include "../Game/Definitions.h"

#include <queue>
using namespace std;

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

	void LoadCacheRecursive(FbxNode* _node, FbxAnimLayer* _animationLayer, MeshData* _meshData, FbxScene* _scene)
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
					FbxAutoPtr<MeshData> meshData(_meshData);
					if(_meshData->Initialize(mesh, _scene))
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
			LoadCacheRecursive(_node->GetChild(childIndex), _animationLayer, _meshData, _scene);
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

		LoadCacheRecursive(_scene->GetRootNode(), _animationLayer, _meshData, _scene);
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

	//meshData->GetAnimationInformation(scene);

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


MeshData::MeshData() : hasNormal(false), hasUV(false), saveByControlPoint(true), isAnimated(true)
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

bool MeshData::Initialize(const FbxMesh* _mesh, const FbxScene* _scene)
{
	scene = (FbxScene*)_scene;

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

	FbxNode* rootNode = _scene->GetRootNode();
	ProcessSkeletonHeirarchy(rootNode);

	if(skeleton.joints.empty())
	{
		isAnimated = false;
	}

	ProcessGeometry(rootNode);

	//Congregate all the data of a mesh to be cached in VBO's
	//If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex

	//isAnimated = GetAnimationInformation(_mesh, _scene);

	return true;
}

void MeshData::ProcessGeometry(FbxNode* _inNode)
{
	if(_inNode->GetNodeAttribute())
	{
		switch(_inNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			{
				ProcessControlPoints(_inNode);
				if(isAnimated)
				{
					ProcessJointsAndAnimations(_inNode);
				}
				ProcessMesh(_inNode);
				AssociateMaterialToMesh(_inNode);
				break;
			}
		}
	}

	for(int i = 0; i < _inNode->GetChildCount(); ++i)
	{
		ProcessGeometry(_inNode->GetChild(i));
	}
}

void MeshData::ProcessControlPoints(FbxNode* _inNode)
{
	FbxMesh* currentMesh = _inNode->GetMesh();
	unsigned int ctrlPointCount = currentMesh->GetControlPointsCount();
	for(unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint* currentControlPoint = new CtrlPoint();
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(currentMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currentMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currentMesh->GetControlPointAt(i).mData[2]);
		currentControlPoint->position = currPosition;
		controlPoints[i] = currentControlPoint;
	}
}

void MeshData::ProcessMesh(FbxNode* _inNode)
{
	FbxMesh* currentMesh = _inNode->GetMesh();

	triangleCount = currentMesh->GetPolygonCount();
	int vertexCounter = 0;
	triangles.reserve(triangleCount);
	vertices.clear();

	for(unsigned int i = 0; i < triangleCount; ++i)
	{
		XMFLOAT3 normal[3];
		XMFLOAT3 tangent[3];
		XMFLOAT3 binormal[3];
		XMFLOAT2 UV[3][2];
		Triangle currTriangle;
		triangles.push_back(currTriangle);

		for(unsigned int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = currentMesh->GetPolygonVertex(i, j);
			CtrlPoint* currCtrlPoint = controlPoints[ctrlPointIndex];

			ReadNormal(currentMesh, ctrlPointIndex, vertexCounter, normal[j]);

			ReadUV(currentMesh, ctrlPointIndex, currentMesh->GetTextureUVIndex(i, j), 0, UV[j][0]);
			PNTIWVertex temp;
			temp.position = currCtrlPoint->position;
			temp.normal = normal[j];
			temp.UV = UV[j][0];

			for(unsigned int i = 0; i < currCtrlPoint->blendingInfo.size(); ++i)
			{
				VertexBlendingInfo currentBlendingInfo;
				currentBlendingInfo.blendingIndex = currCtrlPoint->blendingInfo[i].blendingIndex;
				currentBlendingInfo.blendingWeight = currCtrlPoint->blendingInfo[i].blendingWeight;
				temp.vertexBlendingInfo.push_back(currentBlendingInfo);
			}

			//SORT THAT SHIT
			temp.SortBlendingInfoByWeight();

			vertices.push_back(temp);
			triangles.back().indices.push_back(vertexCounter);
			indices.push_back(vertexCounter);
			++vertexCounter;
		}
	}
}

void MeshData::ReadUV(FbxMesh* _inMesh, int _inCtrlPointIndex, int _inTextureUVIndex, int _inUVLayer, XMFLOAT2& _outUV)
{
	if(_inUVLayer >= 2 || _inMesh->GetElementUVCount() <= _inUVLayer)
	{
		throw std::exception("Invalid UV Layer Number");
	}
	FbxGeometryElementUV* vertexUV = _inMesh->GetElementUV(_inUVLayer);

	switch(vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			{
				_outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(_inCtrlPointIndex).mData[0]);
				_outUV.y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(_inCtrlPointIndex).mData[1]);
			}
			break;

		case FbxGeometryElement::eIndexToDirect:
			{
				int index = vertexUV->GetIndexArray().GetAt(_inCtrlPointIndex);
				_outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
				_outUV.y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
			}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
			{
				_outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(_inTextureUVIndex).mData[0]);
				_outUV.y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(_inTextureUVIndex).mData[1]);
			}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void MeshData::ReadNormal(FbxMesh* _inMesh, int _inCtrlPointIndex, int _inVertexCounter, XMFLOAT3& _outNormal)
{
	if(_inMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = _inMesh->GetElementNormal(0);
	switch(vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch(vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			{
				_outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(_inCtrlPointIndex).mData[0]);
				_outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(_inCtrlPointIndex).mData[1]);
				_outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(_inCtrlPointIndex).mData[2]);
			}
			break;

		case FbxGeometryElement::eIndexToDirect:
			{
				int index = vertexNormal->GetIndexArray().GetAt(_inCtrlPointIndex);
				_outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
				_outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
				_outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
			}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch(vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			{
				_outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(_inVertexCounter).mData[0]);
				_outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(_inVertexCounter).mData[1]);
				_outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(_inVertexCounter).mData[2]);
			}
			break;

		case FbxGeometryElement::eIndexToDirect:
			{
				int index = vertexNormal->GetIndexArray().GetAt(_inVertexCounter);
				_outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
				_outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
				_outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
			}
			break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void MeshData::ProcessSkeletonHeirarchy(FbxNode* _rootNode)
{
	for(int childIndex = 0; childIndex < _rootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currentNode = _rootNode->GetChild(childIndex);
		ProcessSkeletonHeirarchyRecursively(currentNode, 0, 0, -1);
	}
}

void MeshData::ProcessSkeletonHeirarchyRecursively(FbxNode* _node, int _depth, int _index, int _parentIndex)
{
	if(_node->GetNodeAttribute() && _node->GetNodeAttribute()->GetAttributeType() && _node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currentJoint;
		currentJoint.parentIndex = _parentIndex;
		currentJoint.name = _node->GetName();
		skeleton.joints.push_back(currentJoint);
	}
	for(int childNode = 0; childNode < _node->GetChildCount(); ++childNode)
	{
		ProcessSkeletonHeirarchyRecursively(_node->GetChild(childNode), _depth + 1, skeleton.joints.size(), _index);
	}
}

void MeshData::AssociateMaterialToMesh(FbxNode* _inNode)
{
	FbxLayerElementArrayTemplate<int>* materialIndices;
	FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;
	FbxMesh* currMesh = _inNode->GetMesh();

	if(currMesh->GetElementMaterial())
	{
		materialIndices = &(currMesh->GetElementMaterial()->GetIndexArray());
		materialMappingMode = currMesh->GetElementMaterial()->GetMappingMode();

		if(materialIndices)
		{
			switch(materialMappingMode)
			{
			case FbxGeometryElement::eByPolygon:
				{
					if (materialIndices->GetCount() == triangleCount)
					{
						for (unsigned int i = 0; i < triangleCount; ++i)
						{
							unsigned int materialIndex = materialIndices->GetAt(i);
							triangles[i].materialIndex = materialIndex;
						}
					}
				}
				break;

			case FbxGeometryElement::eAllSame:
				{
					unsigned int materialIndex = materialIndices->GetAt(0);
					for (unsigned int i = 0; i < triangleCount; ++i)
					{
						triangles[i].materialIndex = materialIndex;
					}
				}
				break;

			default:
				throw std::exception("Invalid mapping mode for material\n");
			}
		}
	}
}

unsigned int MeshData::FindJointIndexUsingName(string _name)
{
	for(unsigned int i = 0; i < skeleton.joints.size(); ++i)
	{
		if(skeleton.joints[i].name == _name)
		{
			return i;
		}
	}

 	return -1;
}

void MeshData::ProcessJointsAndAnimations(FbxNode* _inNode)
{
	FbxMesh* currentMesh = _inNode->GetMesh();

	string nodeName = _inNode->GetName();

	if(!currentMesh)
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_GENERIC, "Null mesh when getting animation");
		false;
	}

	unsigned int numOfDeformers = currentMesh->GetDeformerCount();

	FbxAMatrix geometryTransform;

	if(!_inNode)
	{
		DebugOutput::Print(DebugOutput::FBXLOADER_MSG, DebugOutput::FAILURE_GENERIC, "Null node when getting animation");
		geometryTransform.SetIdentity();
	}
	else
	{
		const FbxVector4 lT = _inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = _inNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = _inNode->GetGeometricScaling(FbxNode::eSourcePivot);
		geometryTransform = FbxAMatrix(lT, lR, lS);
	}

	int numIndicies = 0;
	for(unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currentSkin = reinterpret_cast<FbxSkin*>(currentMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if(!currentSkin)
		{
			continue;
		}

		unsigned int numOfClusters = currentSkin->GetClusterCount();
		for(unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++ clusterIndex)
		{
			FbxCluster* currCluster = currentSkin->GetCluster(clusterIndex);
			string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName);

			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindPoseInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix);
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			globalBindPoseInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			skeleton.joints[currJointIndex].globalBindposeInverseMatrix = globalBindPoseInverseMatrix;
			skeleton.joints[currJointIndex].node = currCluster->GetLink();

			unsigned int numOfIndicies = currCluster->GetControlPointIndicesCount();
			numIndicies += numOfIndicies;
			for(unsigned int i =  0; i < numOfIndicies; ++i)
			{
				BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.blendingIndex = currJointIndex;
				currBlendingIndexWeightPair.blendingWeight = (float)*currCluster->GetControlPointWeights();
				controlPoints[currCluster->GetControlPointIndices()[i]]->blendingInfo.push_back(currBlendingIndexWeightPair);
			}

			FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			animationName = animStackName.Buffer();

			//TODO:FBX DEBUG STUFF
			FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			animationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			Keyframe** currAnim = &skeleton.joints[currJointIndex].animation;

			for(FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe();
				(*currAnim)->frameNumber = i;
				FbxAMatrix currentTransformOffset = _inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				(*currAnim)->globalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim = &((*currAnim)->next);
			}
		}
	}

	BlendingIndexWeightPair currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.blendingIndex = 0;
	currBlendingIndexWeightPair.blendingWeight = 0;
	for(unsigned int i = 0; i < controlPoints.size(); ++i)
	{
		for(unsigned int j = controlPoints[i]->blendingInfo.size(); j < 4; ++j)
		{
			controlPoints[i]->blendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}