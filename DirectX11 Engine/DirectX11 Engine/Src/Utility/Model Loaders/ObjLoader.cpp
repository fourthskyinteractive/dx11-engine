#include "ObjLoader.h"

#include "../../Game/Definitions.h"
#include "../../Game/Game.h"

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <iostream>
#include <fstream>
using namespace std;

bool ObjLoader::LoadObjFile(char* _filename, bool _hasVertexNormals, int& _vertexCount, int& _textureCount, int& _normalCount, int& _faceCount)
{
	//Get the filecounts for the OBJ file
	ifstream fin;
	char input;

	_vertexCount = 0;
	_textureCount = 0;
	_normalCount = 0;
	_faceCount = 0;

	fin.open(_filename);

	if(fin.fail() == true)
	{
		return false;
	}

	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ')
			{
				_vertexCount ++;
			}
			else if(input == 't')
			{
				_textureCount ++;
			}
			else if(input == 'n')
			{
				_normalCount ++;
			}
		}
		else if(input == 'f')
		{
			fin.get(input);
			if(input == ' ')
			{
				_faceCount ++;
			}
		}

		while(input != '\n')
		{
			fin.get(input);
		}

		//Start reading the beginning of the next line
		fin.get(input);
	}

	fin.close();

	LoadDataStructures(_filename, _hasVertexNormals, _vertexCount, _textureCount, _normalCount, _faceCount);

	return true;
}

bool ObjLoader::LoadDataStructures(char* _filename, bool _hasVertexNormals, int _vertexCount, int _textureCount, int _normalCount, int _faceCount)
{
	Game::GetTimer().TimeStep();
	float timeStart = (Game::GetTimer().GetDeltaTimeFloat() / 1000.0f);
	float timeEnd;
	XMFLOAT3* vertices;
	XMFLOAT3* texCoords;
	XMFLOAT3* normals;

	FaceType* faces;
	ifstream fin;
	int vertexIndex, texCoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;

	//Initialize the four data structures
	vertices = new XMFLOAT3[_vertexCount];
	if(!vertices)
	{
		return false;
	}

	texCoords = new XMFLOAT3[_textureCount];
	if(!texCoords)
	{
		return false;
	}

	normals = new XMFLOAT3[_normalCount];
	if(!normals)
	{
		return false;
	}

	faces = new FaceType[_faceCount];
	if(!faces)
	{
		return false;
	}

	//Initialize the indexes
	vertexIndex = 0;
	texCoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	//Open the file
	fin.open(_filename);

	//Check if it was successful in opening the file
	if(fin.fail() == true)
	{
		return false;
	}

	int linesRead = 0;
	//Read in the vertices, texture coordinates, and normals into the data structures.
	//Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				//Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1;
				vertexIndex ++;
			}

			//Read in the texture uv coordinates
			if(input == 't')
			{
				fin >> texCoords[texCoordIndex].x >> texCoords[texCoordIndex].y;

				//Invert the V texture coordinate to the left hand system.
				texCoords[texCoordIndex].y = 1.0f - texCoords[texCoordIndex].y;
				texCoordIndex ++;
			}

			//Read in the normals
			if(input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				//Invert the Z normal to change to left hand system
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex ++;
			}
		}

		//Read in the faces
		if(input == 'f')
		{
			fin.get(input);
			if(input == ' ')
			{
				//Read the face data in backwards to convert it to a left hand system from right hand system
				fin >> faces[faceIndex].vIndex3;
				fin >> input2;
				fin >> faces[faceIndex].tIndex3;
				if(_hasVertexNormals)
				{
					fin >> input2;
					fin >> faces[faceIndex].nIndex3;
				}

				fin	>> faces[faceIndex].vIndex2;
				fin >> input2;
				fin >> faces[faceIndex].tIndex2;
				if(_hasVertexNormals)
				{
					fin >> input2;
					fin >> faces[faceIndex].nIndex2;
				}

				fin	>> faces[faceIndex].vIndex1;
				fin >> input2;
				fin >> faces[faceIndex].tIndex1;
				if(_hasVertexNormals)
				{
					fin >> input2;
					fin >> faces[faceIndex].nIndex1;
				}

				faceIndex ++;
			}
		}

		//Read in the remainder of the line
		while(input != '\n')
		{
			fin.get(input);
		}
		linesRead ++;
		if(linesRead == 30)
		{
			bool stopHere = true;
		}
		//Start reading the beginning of the next line
		fin.get(input);
	}

	//Close the file
	fin.close();

	Game::GetTimer().TimeStep();
	timeEnd = (Game::GetTimer().GetDeltaTimeFloat() / 1000.0f);
	float actualTime = timeEnd - timeStart;

	//Open the output file
	char* modelName = GetModelName(_filename);
	string newFilename = "Res/Objects/";
	newFilename += modelName;
	newFilename[newFilename.size() - 4] = '.';
	newFilename[newFilename.size() - 3] = 't';
	newFilename[newFilename.size() - 2] = 'x';
	newFilename[newFilename.size() - 1] = 't';
	newFilename[newFilename.size()] = '\0';
	fout.open(newFilename);

	//Write out the file header that our model format uses.
	fout << "Vertex Count:" << (_faceCount * 3) << endl;
	fout << endl;
	fout << "Uses BumpMap ";
	fout << _hasVertexNormals;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	// Now loop through all the faces and output the three vertices for each face.
	for(int i=0; i<faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x;
		fout << ' ';
		fout << vertices[vIndex].y;
		fout << ' ';
		fout << vertices[vIndex].z;
		fout << ' ';
		fout << texCoords[tIndex].x;
		fout << ' ';
		fout << texCoords[tIndex].y;
// 		if(_hasVertexNormals)
// 		{
// 			fout << ' ';
// 			fout << normals[nIndex].x;
// 			fout << ' ';
// 			fout << normals[nIndex].y;
// 			fout << ' ';
// 			fout << normals[nIndex].z;
// 		}
		fout << ' ';
		fout << 0;
		fout << ' ';
		fout << 0;
		fout << ' ';
		fout << 0;
 		fout << endl;		

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x;
		fout << ' ';
		fout << vertices[vIndex].y;
		fout << ' ';
		fout << vertices[vIndex].z;
		fout << ' ';
		fout << texCoords[tIndex].x;
		fout << ' ';
		fout << texCoords[tIndex].y;		
// 		if(_hasVertexNormals)
// 		{
// 			fout << ' ';
// 			fout << normals[nIndex].x;
// 			fout << ' ';
// 			fout << normals[nIndex].y;
// 			fout << ' ';
// 			fout << normals[nIndex].z;
// 		}
		fout << ' ';
		fout << 0;
		fout << ' ';
		fout << 0;
		fout << ' ';
		fout << 0;
 		fout << endl;
		

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x;
		fout << ' ';
		fout << vertices[vIndex].y;
		fout << ' ';
		fout << vertices[vIndex].z;
		fout << ' ';
		fout << texCoords[tIndex].x;
		fout << ' ';
		fout << texCoords[tIndex].y;
// 		if(_hasVertexNormals)
// 		{
// 			fout << ' ';
// 			fout << normals[nIndex].x;
// 			fout << ' ';
// 			fout << normals[nIndex].y;
// 			fout << ' ';
// 			fout << normals[nIndex].z;
// 		}
		fout << ' ';
		fout << 0;
		fout << ' ';
		fout << 0;
		fout << ' ';
		fout << 0;
		fout << endl;
	}

	// Close the output file.
	fout.close();

	// Release the four data structures.
	if(vertices)
	{
		delete [] vertices;
		vertices = 0;
	}
	if(texCoords)
	{
		delete [] texCoords;
		texCoords = 0;
	}
	if(normals)
	{
		delete [] normals;
		normals = 0;
	}
	if(faces)
	{
		delete [] faces;
		faces = 0;
	}

	return true;
}

char* ObjLoader::GetModelName(char* _filename)
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