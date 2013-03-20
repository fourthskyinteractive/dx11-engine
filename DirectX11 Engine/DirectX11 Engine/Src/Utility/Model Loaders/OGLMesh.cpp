#include "stdafx.h"
#include "OGLMesh.h"
#include <iostream>
#include <fstream>
#include "OGLVertexBufferObject.h"

using namespace OGL;

#define NEXT_INDICE do{i++;}while((buff[i]<'0')&&(buff[i]>'9'));

bool OGLMesh::m_sbComputeNormals = false;

OGLMesh::OGLMesh()
{
	m_pVBO = NULL;
}

OGLMesh::~OGLMesh()
{
	Destroy();
}

bool OGLMesh::Load( const std::string& szName )
{
	bool ErrorCheck = false;

	m_pVBO = new OGLVertexBufferObject();
	if( szName.find(".obj") != std::string::npos )
		ErrorCheck = LoadObj( szName );
	else if( szName.find(".fbx") != std::string::npos )
		ErrorCheck = LoadFBX( szName );
	else
		printf( "ERROR %s : %d - file type not recognized!\n", __FILE__, __LINE__ );

	if( !ErrorCheck )
		printf( "ERROR %s : %d - could not load file!\n", __FILE__, __LINE__ );

	if( m_sbComputeNormals )
		ComputeNormals();
	ComputeTangents();
	//ComputeBoundingBox();

	m_glDebugBatch.Begin( GL_LINES, m_pVBO->GetPositions().size() * 2 );
		for( int i = 0; i < m_pVBO->GetPositions().size(); i++ )
		{
			m_glDebugBatch.Vertex3f( m_pVBO->GetPositions()[i].x, m_pVBO->GetPositions()[i].y, m_pVBO->GetPositions()[i].z );
			m_glDebugBatch.Vertex3f( m_pVBO->GetPositions()[i].x + m_pVBO->GetNormals()[i].x * 2,
											 m_pVBO->GetPositions()[i].y + m_pVBO->GetNormals()[i].y * 2,
											 m_pVBO->GetPositions()[i].z + m_pVBO->GetNormals()[i].z * 2 );
		}

	return m_pVBO->Create( GL_STATIC_DRAW );
}

void OGLMesh::Destroy( void )
{
	if( m_pVBO != NULL )
	{
		m_pVBO->Destroy();
		delete m_pVBO;
		m_pVBO = NULL;
	}

	std::vector<sGroup>::iterator iter = m_vGroups.begin();

	for( ; iter != m_vGroups.end(); ++iter )
	{
		iter->vFaces.clear();
	}

	m_vGroups.clear();
}

void OGLMesh::Draw( void )
{
	if( m_pVBO )
	{
		m_pVBO->Enable();

		std::vector<sGroup>::iterator iter = m_vGroups.begin();
		for( ; iter != m_vGroups.end(); ++iter )
			glDrawElements( GL_TRIANGLES, (GLsizei)iter->vFaces.size() * 3, GL_UNSIGNED_INT, &(iter->vFaces[0].ind[0]));

		m_pVBO->Disable();
	}
	else
		printf( "ERROR %s : %d - Draw called on an empty OGLMesh!\n", __FILE__, __LINE__ );
}

void OGLMesh::Draw( GLuint unGroup )
{
	if( m_pVBO )
	{
		if( unGroup < GetGroupCount() )
		{
			m_pVBO->Enable();
			glDrawElements( GL_TRIANGLES, (GLsizei)m_vGroups[unGroup].vFaces.size() * 3, GL_UNSIGNED_INT, &(m_vGroups[unGroup].vFaces[0].ind[0]) );
			m_pVBO->Disable();
		}
		else
			printf( "ERROR %s : %d - unGroup is out of range!\n", __FILE__, __LINE__ );
	}
	else
		printf( "ERROR %s : %d - Draw called on an empty OGLMesh!\n", __FILE__, __LINE__ );
}

void OGLMesh::DrawDebug( void )
{
	m_glDebugBatch.Draw();
}

GLuint OGLMesh::GetGroupCount( void )
{
	return (GLuint)m_vGroups.size();
}

//OGLBoundingBox& GetBoundingBox( void )
//{
//
//}

void OGLMesh::EnableComputeNormals( bool bEnable )
{
	m_sbComputeNormals = bEnable;
}

bool OGLMesh::LoadObj( const std::string& szFile )
{
	std::ifstream fs( szFile.c_str(), std::ios_base::binary);
	if( !fs.is_open() )
		return false;

	sFace face;
	sGroup group;
	Float3 pos;
	Float2 tx;

	long i, v, g;

	GLuint FacePosIndex;
	GLuint FaceNormIndex;
	GLuint FaceTexIndex;

	std::vector<Float3> vTempPositions;
	std::vector<Float3> vTempNormals;
	std::vector<Float2> vTempTexCoords;

	std::vector<Float3>& vPositions = m_pVBO->GetPositions();
	std::vector<Float3>& vNormals = m_pVBO->GetNormals();
	std::vector<Float2>& vTexCoors = m_pVBO->GetTexcoords();

	std::string strBuffer;
	while( std::getline(fs, strBuffer) )
	{
		std::stringstream strStream( strBuffer );
		const char* buff = strBuffer.c_str();

		if( strBuffer.size() > 0 )
		{
			std::string bufftmp;
			strStream >> bufftmp;

			if( bufftmp == "#" )
			{

			}
			else if( bufftmp == "mtllib" )
			{

			}
			else if( bufftmp == "v" )
			{
				sscanf( &strBuffer[2], "%f%f%f", &pos.x, pos.y, pos.z );
				vTempPositions.push_back( pos );
			}
			else if( bufftmp == "vt" )
			{
				sscanf( &strBuffer[2], "%f%f", &tx.x, &tx.y );
				tx.t = 1.0f - tx.t;
				vTempTexCoords.push_back( tx );
			}
			else if( bufftmp == "vn" )
			{
				sscanf( &strBuffer[2], "%f%f%f", &pos.x, &pos.y, &pos.z );
				pos.Normalize();
				vTempNormals.push_back( pos );
			}
			else if( bufftmp == "g" )
			{
				if( strBuffer.size() > 1 )
				{
					group.szName = &buff[2];
					group.lMaterial = 0;
					m_vGroups.push_back( group );
					g = 0;
				}
				else
				{
					g = (long)m_vGroups.size() - 1;
				}
			}
			else if( bufftmp == "f" )
			{
				int max;
				if( vTempPositions.size() >= vTempNormals.size() )
					max = 0;
				else				
					max = 1;

				if(max==0 && vTempTexCoords.size() > vTempPositions.size() )
					max = 2;
				if(max==1 && vTempTexCoords.size() > vTempNormals.size() )	
					max = 2;

				switch(max) 
				{
				case 0: 
					{
					vPositions.resize( vTempPositions.size() );
					vNormals.resize( vTempPositions.size() );
					vTexCoors.resize( vTempPositions.size() );
					break;
					}
				case 1: 
					{
					vPositions.resize( vTempNormals.size() );
					vNormals.resize( vTempNormals.size() );
					vTexCoors.resize( vTempNormals.size() );
					break;
					}
				case 2: 
					{
					vPositions.resize( vTempTexCoords.size() );
					vNormals.resize( vTempTexCoords.size() );
					vTexCoors.resize( vTempTexCoords.size() );
					break;
					}
				}

				if(m_vGroups.size() <= 0) 
				{
					group.szName = "No Name";
					group.lMaterial = 0;
					m_vGroups.push_back(group);
					g = 0;
				} 
				else 
				{
					g = (long)m_vGroups.size() - 1;
				}
				for( i = 0; (buff[i] < '0') || (buff[i] > '9') ; i++ );
				{
					for(v=0; v < 3 ;v++) 
					{
						FacePosIndex = 0;
						for( ;(buff[i] >= '0') && (buff[i] <= '9'); i++ ) 
						{
							FacePosIndex *= 10;
							FacePosIndex += buff[i]-0x30;
						}

						FacePosIndex--;
						NEXT_INDICE;
						FaceTexIndex = 0;

						for( ;(buff[i] >= '0') && (buff[i] <= '9'); i++ ) 
						{
							FaceTexIndex *= 10;
							FaceTexIndex += buff[i]-0x30;
						}

						FaceTexIndex--;
						NEXT_INDICE;
						FaceNormIndex = 0;

						for( ; (buff[i] >= '0') && (buff[i] <= '9'); i++ ) 
						{
							FaceNormIndex *= 10;
							FaceNormIndex += buff[i]-0x30;
						}

						FaceNormIndex--;
						if( v < 2 ) 
							NEXT_INDICE;


						int idx = 0;
						switch( max ) 
						{
						case 0: 
							{
								idx = FacePosIndex;	
								break;
							}
						case 1: 
							{
								idx = FaceNormIndex;
								break;
							}
						case 2: 
							{
								idx = FaceTexIndex;	
								break;
							}
						}

						vPositions[idx] = vTempPositions[FacePosIndex];
						vNormals[idx] = vTempNormals[FaceNormIndex];
						vTexCoors[idx] = vTempTexCoords[FaceTexIndex];
						face.ind[v] = idx;
					}
				}


				m_vGroups[g].vFaces.push_back(face);
			}
		}
	}

	fs.close();

	vTempPositions.clear();
	vTempNormals.clear();
	vTempTexCoords.clear();

	//GLuint nbFaces = 0;
	//std::vector<sGroup>::iterator iter = m_vGroups.begin();
	//for( ; iter != m_vGroups.end(); ++iter )
	//{
	//	nbFaces += (GLuint)(iter->vFaces.size() );
	//}
	//	cout << "nbFaces : " << nbFaces << endl;

	return false;
}

void OGLMesh::FBXTriangulateRecursive( FbxNode* pNode )
{
	FbxNodeAttribute* pNodeAttr = pNode->GetNodeAttribute();

	if( pNodeAttr )
	{
		if( pNodeAttr->GetAttributeType() == FbxNodeAttribute::eMesh || 
			pNodeAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			pNodeAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
			pNodeAttr->GetAttributeType() == FbxNodeAttribute::ePatch )
		{
			FbxGeometryConverter lConverter( pNode->GetFbxManager() );
			lConverter.TriangulateInPlace( pNode );
		}
	}

	const int nChildCount = pNode->GetChildCount();
	for( int i = 0; i < nChildCount; i++ )
	{
		FBXTriangulateRecursive( pNode->GetChild( i ) );
	}
}

void OGLMesh::FBXFillMeshArrayRecursive( FbxNode* pNode, FbxArray<FbxMesh*>& pMeshArray )
{
	if( pNode )
	{
		if( pNode->GetNodeAttribute() )
		{
			if( pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh )
			{
				pMeshArray.Add( pNode->GetMesh() );
			}
		}

		const int lCount = pNode->GetChildCount();
		for( int i = 0; i < lCount; i++ )
			FBXFillMeshArrayRecursive( pNode->GetChild(i), pMeshArray );
	}
}

void OGLMesh::FBXFillMeshArray( FbxScene* pScene, FbxArray<FbxMesh*>& pMeshArray )
{
	pMeshArray.Clear();
	FBXFillMeshArrayRecursive( pScene->GetRootNode(), pMeshArray );
}

bool OGLMesh::LoadFBX( const std::string& szFile )
{
	FbxManager* pFBXManager = FbxManager::Create();
	if( !pFBXManager )
	{
		printf( "ERROR %s : %d failed creating FBX Manager!\n", __FILE__, __LINE__ );
		return false;
	}

	FbxIOSettings* pIOS = FbxIOSettings::Create( pFBXManager, IOSROOT );
	pFBXManager->SetIOSettings( pIOS );

	FbxString lPath = FbxGetApplicationDirectory();
	pFBXManager->LoadPluginsDirectory( lPath.Buffer() );

	FbxScene* pScene = FbxScene::Create( pFBXManager, "" );

	int /*nFileMajor,*/ nFileMinor, nFileRevision;
	int nSDKMajor, nSDKMinor, nSDKRevision;

	int i, /*nAnimationStack,*/ lFileFormat;
	//	bool bStatus;
	//	char szPassword[1024];

	FbxManager::GetFileFormatVersion( nSDKMajor, nSDKMinor, nSDKRevision );

	FbxImporter* pImporter = FbxImporter::Create( pFBXManager, "" );

	if (!pFBXManager->GetIOPluginRegistry()->DetectReaderFileFormat(szFile.c_str(), lFileFormat) )
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		lFileFormat = pFBXManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
	}

	bool ImportStatus = pImporter->Initialize( szFile.c_str(), lFileFormat, pFBXManager->GetIOSettings() );
	pImporter->GetFileVersion( nFileMinor, nFileMinor, nFileRevision );

	if( !ImportStatus )
	{
		printf( "ERROR %s : %d FbxImporter Initialize failed!\n", __FILE__, __LINE__ );
		return false;
	}

	ImportStatus = pImporter->Import( pScene );

	if( !ImportStatus )
	{
		printf( "ERROR %s : %d FbxImporter failed to import the file to the scene!\n", __FILE__, __LINE__ );
		return false;
	}

	FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem AxisSystem( FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded );
	if( SceneAxisSystem != AxisSystem )
	{
		AxisSystem.ConvertScene( pScene );
	}

	//FBXTriangulateRecursive( pScene->GetRootNode() );

	FbxArray<FbxMesh*> vMeshs;
	FBXFillMeshArray( pScene, vMeshs );

	unsigned short usVertexCount = 0;
	unsigned short usTriangleCount = 0;
	unsigned short usGroupCount = 0;
	unsigned short usMaterialCount = 0;
	unsigned short usIndicesCount = 0;

	std::vector<Float3>& vPositions = m_pVBO->GetPositions();
	std::vector<Float2>& vTexcoords = m_pVBO->GetTexcoords();
	std::vector<Float3>& vNormals = m_pVBO->GetNormals();
	//m_vGroups.resize( vMeshs.GetCount() );

	sGroup group;
	sFace face;
	Float3 point;
	Float2 tex;

	unsigned int currentSize = 0;

	for( i = 0; i < vMeshs.GetCount(); i++ )
	{
		currentSize += vMeshs[i]->GetPolygonVertexCount();
		std::string name = vMeshs[i]->GetNode()->GetName();
		group.szName = name;
		group.lMaterial = 0;
		m_vGroups.push_back(group);
	}

	vPositions.resize( currentSize );
	vTexcoords.resize( currentSize );
	vNormals.resize( currentSize );
	currentSize = 0;

	for( i = 0; i < vMeshs.GetCount(); i++ )
	{
		printf( "Loading model %s...\n", m_vGroups[i].szName.c_str() );
		usVertexCount = vMeshs[i]->GetControlPointsCount();
		if( usVertexCount == 0 )
			continue;

		usTriangleCount = vMeshs[i]->GetPolygonVertexCount() / 3;
		usIndicesCount = vMeshs[i]->GetPolygonVertexCount();

		FbxVector4* pFBXVerts = new FbxVector4[usVertexCount];
		int ArrayIndex = 0;
		memcpy( pFBXVerts, vMeshs[i]->GetControlPoints(), usVertexCount * sizeof(FbxVector4));

		for( int j = 0; j < usTriangleCount; j++ )
		{	
			int nIndex = 0;
			FbxVector4 FBXNorm( 0, 0, 0, 0 );
			FbxVector2 FBXUV( 0, 0 );

			face.ind[0] = nIndex = vMeshs[i]->GetPolygonVertex( j, 0 ) + currentSize;
			vPositions[nIndex].x = (float)pFBXVerts[ nIndex - currentSize ][0];
			vPositions[nIndex].y = (float)pFBXVerts[ nIndex - currentSize ][1];
			vPositions[nIndex].z = (float)pFBXVerts[ nIndex - currentSize ][2];
			vMeshs[i]->GetPolygonVertexNormal( j, 0, FBXNorm );
			vNormals[nIndex].x = (float)FBXNorm[0];
			vNormals[nIndex].y = (float)FBXNorm[1];
			vNormals[nIndex].z = (float)FBXNorm[2];
			vMeshs[i]->GetPolygonVertexUV( j, 0, "map1", FBXUV );
			vTexcoords[nIndex].x = (float)FBXUV[0];
			vTexcoords[nIndex].y = (float)FBXUV[1];

			face.ind[1] = nIndex = vMeshs[i]->GetPolygonVertex( j, 1 ) + currentSize;
			vPositions[nIndex].x = (float)pFBXVerts[ nIndex - currentSize ][0];
			vPositions[nIndex].y = (float)pFBXVerts[ nIndex - currentSize ][1];
			vPositions[nIndex].z = (float)pFBXVerts[ nIndex - currentSize ][2];
			vMeshs[i]->GetPolygonVertexNormal( j, 1, FBXNorm );
			vNormals[nIndex].x = (float)FBXNorm[0];
			vNormals[nIndex].y = (float)FBXNorm[1];
			vNormals[nIndex].z = (float)FBXNorm[2];
			vMeshs[i]->GetPolygonVertexUV( j, 1, "map1", FBXUV );
			vTexcoords[nIndex].x = (float)FBXUV[0];
			vTexcoords[nIndex].y = (float)FBXUV[1];

			face.ind[2] = nIndex = vMeshs[i]->GetPolygonVertex( j, 2 ) + currentSize;
			vPositions[nIndex].x = (float)pFBXVerts[ nIndex - currentSize ][0];
			vPositions[nIndex].y = (float)pFBXVerts[ nIndex - currentSize ][1];	
			vPositions[nIndex].z = (float)pFBXVerts[ nIndex - currentSize ][2];
			vMeshs[i]->GetPolygonVertexNormal( j, 2, FBXNorm );
			vNormals[nIndex].x = (float)FBXNorm[0];
			vNormals[nIndex].y = (float)FBXNorm[1];
			vNormals[nIndex].z = (float)FBXNorm[2];
			vMeshs[i]->GetPolygonVertexUV( j, 2, "map1", FBXUV );
			vTexcoords[nIndex].x = (float)FBXUV[0];
			vTexcoords[nIndex].y = (float)FBXUV[1];

			m_vGroups[i].vFaces.push_back( face );
		}

		delete[] pFBXVerts;

		vMeshs[i]->Destroy();
		vMeshs[i] = NULL;

		currentSize += usTriangleCount * 3;
	}


	pImporter->Destroy();
	pImporter = NULL;

	pScene->Destroy();
	pScene = NULL;

	pIOS->Destroy();
	pIOS = NULL;

	pFBXManager->Destroy();
	pFBXManager = NULL;

	return true;
}

void OGLMesh::ComputeVBO( void )
{
	ComputeNormals();
	ComputeTangents();
}

void OGLMesh::ComputeNormals( void )
{
	std::vector<Float3>& vPositions = m_pVBO->GetPositions();
	std::vector<Float3>& vNormals = m_pVBO->GetNormals();
	std::vector<Float2>& vTexcoords = m_pVBO->GetTexcoords();

	vNormals.assign( vNormals.size(), Float3( 0.0f, 0.0f, 0.0f ) );
	std::vector<int> vNormalCount( vNormals.size(), 0 );

	std::vector<sGroup>::iterator groupIter = m_vGroups.begin();
	for( ; groupIter != m_vGroups.end(); ++groupIter )
	{
		std::vector<sFace>::iterator faceIter = groupIter->vFaces.begin();
		for( ; faceIter != groupIter->vFaces.end(); ++faceIter )
		{
			GLuint* ind = ((GLuint*)faceIter->ind);

			Float3 v0 = vPositions[ind[0]];
			Float3 v1 = vPositions[ind[1]];
			Float3 v2 = vPositions[ind[2]];

			Float3 v10 = v0 - v1;
			Float3 v12 = v2 - v1;

			Float3 f3Normal = Float3::Cross( v12, v10 );
			f3Normal.Normalize();

			vNormals[ind[0]] += f3Normal;
			vNormals[ind[1]] += f3Normal;
			vNormals[ind[2]] += f3Normal;

			vNormalCount[ind[0]]++;
			vNormalCount[ind[1]]++;
			vNormalCount[ind[2]]++;
		}
	}

	for( unsigned int i = 0; i < vNormals.size(); i++ )
	{
		if( vNormalCount[i] > 0 )
		{
			vNormals[i] /= (float)vNormalCount[i];
		}
	}

	vNormalCount.clear();
}

void OGLMesh::ComputeTangents( void )
{
	std::vector<Float3>& vPositions = m_pVBO->GetPositions();
	std::vector<Float3>& vNormals = m_pVBO->GetNormals();
	std::vector<Float2>& vTexcoords = m_pVBO->GetTexcoords();
	std::vector<Float3>& vTangents = m_pVBO->GetTangents();

	vTangents.resize( vNormals.size() );

	std::vector<sGroup>::iterator groupIter = m_vGroups.begin();
	for( ; groupIter != m_vGroups.end(); ++groupIter )
	{
		std::vector<sFace>::iterator faceIter = groupIter->vFaces.begin();
		for( ; faceIter != groupIter->vFaces.end(); ++faceIter )
		{
			GLuint* ind = ((GLuint*)faceIter->ind);
			
			Float3 f3Tangent;

			Float3 v0 = vPositions[ind[0]];
			Float3 v1 = vPositions[ind[1]];
			Float3 v2 = vPositions[ind[2]];

			Float3 v10 = v0 - v1;
			Float3 v12 = v2 - v1;

			float fDT10 = vTexcoords[ind[0]].t - vTexcoords[ind[1]].t;
			float fDT12 = vTexcoords[ind[2]].t - vTexcoords[ind[1]].t;

			f3Tangent = v10 * fDT12 - v12 * fDT10;
			f3Tangent.Normalize();
			
			vTangents[ind[0]] = vTangents[ind[1]] = vTangents[ind[2]] = f3Tangent;
		}
	}
}

//void OGLMesh::ComputeBoundingBox( void )
//{
//
//}