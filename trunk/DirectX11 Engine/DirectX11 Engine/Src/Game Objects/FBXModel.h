#ifndef FBXMODEL_H_
#define FBXMODEL_H_

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

	bool Initialize(const FbxMesh* _mesh);

//private:
	enum
	{
		VERTEX_VBO,
		NORMAL_VBO,
		UV_VBO,
		INDEX_VBO,
		VBO_COUNT,
	};

	struct SubMesh
	{
		SubMesh() : indexOffset(0), triangleCount(0){}

		int indexOffset;
		int triangleCount;
	};
	
	unsigned int vboNames[VBO_COUNT];
	vector<XMFLOAT4> positions;
	vector<XMFLOAT2> UVs;
	vector<XMFLOAT3> normals;
	vector<unsigned long> indices;

	//vector<XMFLOAT3> tangents;
	//vector<XMFLOAT3> binormals;
	FbxArray<SubMesh*> subMeshes;
	bool hasNormal;
	bool hasUV;
	bool saveByControlPoint;
};
#endif