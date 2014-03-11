#ifndef FBXMODEL_H_
#define FBXMODEL_H_

#include <unordered_map>
using namespace std;

#include <DirectXMath.h>
using namespace DirectX;

#include <fbxsdk.h>
#include <vector>
using namespace std;

class MeshData;
class MaterialData;

class FBXModel
{
public:
	enum Status {UNLOADED, MUST_BE_LOADED, MUST_BE_REFRESHED, REFRESHED};

	FBXModel(const char* _fileName);
	~FBXModel();

	Status GetStatus() const {return status;}
	const FbxScene* GetScene(){return scene;}
	bool LoadFile();

 	const FbxTime GetFrameTime() const {return frameTime;}
 
 	const FbxArray<FbxNode*>& GetCameraArray()const {return cameraArray;}
 	const FbxArray<FbxString*>& GetAnimStackNameArray() const {return animStackNameArray;}
 	const FbxArray<FbxPose*>& GetPoseArray() const {return poseArray;}
	vector<int> GetTextureIndices(){return textureIndices;}

	bool SetCurrentAnimStack(int _index);
	bool SetCurrentPoseIndex(int _poseIndex);
	bool SetSelectedNode(FbxNode* _selectedNode);
	void SetPause(bool _pause){pauseAnimation = _pause;}
	MeshData* meshData;

private:
	enum CameraStatus{CAMERA_NOTHING, CAMERA_ORBIT, CAMERA_ZOOM, CAMERA_PAN};

	const char* fileName;
	Status status;

	FbxManager* fbxManager;
	FbxScene* scene;
	FbxImporter* importer;
	FbxAnimLayer* currentAnimLayer;
	FbxNode* selectedNode;

	bool pauseAnimation;
	int poseIndex;
	vector<int> textureIndices;

 	FbxArray<FbxString*> animStackNameArray;
 	FbxArray<FbxNode*> cameraArray;
 	FbxArray<FbxPose*> poseArray;
 
 	FbxTime frameTime, startTime, stopTime, currentTime;
 	FbxTime cacheStartTime, cacheStopTime;
};

class MaterialData
{
public:
	MaterialData();
	~MaterialData();

	bool Initialize(const FbxSurfaceMaterial* _material);

	//Set material colors and binding diffuse texture if it exists
	void SetCurrentMaterial() const;

	bool HasTexture() const{if(diffuse >= 0) return true; else return false;}

	static void SetDefaultMaterial();

private:
	int emissive;
	int ambient;
	int diffuse;
	int specular;
	float shinness;
};

class MeshData
{
public:
	MeshData();
	~MeshData();

	bool Initialize(const FbxMesh* _mesh, const FbxScene* _scene);
	unsigned int FindJointIndexUsingName(string _name);
	void ProcessSkeletonHeirarchy(FbxNode* _rootNode);
	void ProcessSkeletonHeirarchyRecursively(FbxNode* _node, int _depth, int _index, int _parentIndex);
	void ProcessJointsAndAnimations(FbxNode* _inNode);
	void ProcessGeometry(FbxNode* _inNode);
	void ProcessMesh(FbxNode* _inNode);
	void ProcessControlPoints(FbxNode* _inNode);
	void AssociateMaterialToMesh(FbxNode* _inNode);
	void ReadUV(FbxMesh* _inMesh, int _inCtrlPointIndex, int _inTextureUVIndex, int _inUVLayer, XMFLOAT2& _outUV);
	void ReadNormal(FbxMesh* _inMesh, int _inCtrlPointIndex, int _inVertexCounter, XMFLOAT3& _outNormal);

//private:
	enum
	{
		VERTEX_VBO,
		NORMAL_VBO,
		UV_VBO,
		INDEX_VBO,
		VBO_COUNT,
	};

	struct BlendingIndexWeightPair
	{
		unsigned int blendingIndex;
		float blendingWeight;

		BlendingIndexWeightPair() : blendingIndex(0), blendingWeight(0){}
	};

	struct CtrlPoint
	{
		XMFLOAT3 position;
		vector<BlendingIndexWeightPair> blendingInfo;

		CtrlPoint()
		{
			blendingInfo.reserve(4);
		}
	};

	struct Keyframe
	{
		long long frameNumber;
		FbxAMatrix globalTransform;
		Keyframe* next;

		Keyframe() : next(nullptr){}
	};

	struct Joint
	{
		string name;
		int parentIndex;
		FbxAMatrix globalBindposeInverseMatrix;
		FbxNode* node;
		Keyframe* animation;

		Joint() : node(nullptr), animation(nullptr)
		{
			globalBindposeInverseMatrix.SetIdentity();
			parentIndex = -1;
		}

		~Joint()
		{
			while(animation)
			{
				Keyframe* temp = animation->next;
				delete animation;
				animation = temp;
			}
		}
	};

	struct Skeleton
	{
		vector<Joint> joints;
	};

	struct Triangle
	{
		vector<unsigned int> indices;
		string materialName;
		unsigned int materialIndex;

		bool operator<(const Triangle& _tri)
		{
			return materialIndex < _tri.materialIndex;
		}
	};

	struct VertexBlendingInfo
	{
		unsigned int blendingIndex;
		float blendingWeight;

		VertexBlendingInfo():
			blendingIndex(0),
			blendingWeight(0.0)
		{}

		bool operator < (const VertexBlendingInfo& rhs)
		{
			return (blendingWeight > rhs.blendingWeight);
		}
	};

	struct PNTIWVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 UV;
		std::vector<VertexBlendingInfo> vertexBlendingInfo;

		void SortBlendingInfoByWeight()
		{
			for(unsigned int i = 0; i < vertexBlendingInfo.size() - 1; ++i)
			{
				if(i > 1000000)
				{
					break;
				}
				for(unsigned int j = 0; j < vertexBlendingInfo.size(); ++j)
				{
					if(vertexBlendingInfo[j].blendingWeight < vertexBlendingInfo[i].blendingWeight)
					{
						float tempWeight = vertexBlendingInfo[i].blendingWeight;
						vertexBlendingInfo[i].blendingWeight = vertexBlendingInfo[j].blendingWeight;
						vertexBlendingInfo[j].blendingWeight = tempWeight;
					}
				}
			}
		}

// 		bool operator==(const PNTIWVertex& rhs) const
// 		{
// 			bool sameBlendingInfo = true;
// 
// 			// We only compare the blending info when there is blending info
// 			if(!(mVertexBlendingInfos.empty() && rhs.mVertexBlendingInfos.empty()))
// 			{
// 				// Each vertex should only have 4 index-weight blending info pairs
// 				for (unsigned int i = 0; i < 4; ++i)
// 				{
// 					if (mVertexBlendingInfos[i].mBlendingIndex != rhs.mVertexBlendingInfos[i].mBlendingIndex ||
// 						abs(mVertexBlendingInfos[i].mBlendingWeight - rhs.mVertexBlendingInfos[i].mBlendingWeight) > 0.001)
// 					{
// 						sameBlendingInfo = false;
// 						break;
// 					}
// 				}
// 			}
// 
// 			bool result1 = MathHelper::CompareVector3WithEpsilon(mPosition, rhs.mPosition);
// 			bool result2 = MathHelper::CompareVector3WithEpsilon(mNormal, rhs.mNormal);
// 			bool result3 = MathHelper::CompareVector2WithEpsilon(mUV, rhs.mUV);
// 
// 			return result1 && result2 && result3 && sameBlendingInfo;
// 		}
	};

	struct SubMesh
	{
		SubMesh() : indexOffset(0), triangleCount(0){}

		int indexOffset;
		int triangleCount;
	};
	
	bool isAnimated;

	FbxScene* scene;
	unsigned int vboNames[VBO_COUNT];
	vector<unsigned long> indices;
	std::unordered_map<unsigned int, CtrlPoint*> controlPoints;
	unsigned int triangleCount;
	vector<Triangle> triangles;
	Skeleton skeleton;
	std::vector<PNTIWVertex> vertices;

	FbxLongLong animationLength;
	std::string animationName;

	//vector<XMFLOAT3> tangents;
	//vector<XMFLOAT3> binormals;
	FbxArray<SubMesh*> subMeshes;
	bool hasNormal;
	bool hasUV;
	bool saveByControlPoint;
};
#endif