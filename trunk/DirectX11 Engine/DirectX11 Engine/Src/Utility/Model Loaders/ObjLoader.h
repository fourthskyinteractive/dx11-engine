#ifndef OBJLOADER_H
#define OBJLOADER_H

class ObjLoader
{
public:
	ObjLoader();
	ObjLoader(const ObjLoader&){};
	~ObjLoader();

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	static bool LoadObjFile(char* _filename, bool _hasVertexNormals, int& _vertexCount, int& _textureCount, int& _normalCount, int& _faceCount);
	static bool LoadDataStructures(char* _filename, bool _hasVertexNormals, int _vertexCount, int _textureCount, int _normalCount, int _faceCount);
	static char* GetModelName(char* _filename);
};


#endif
