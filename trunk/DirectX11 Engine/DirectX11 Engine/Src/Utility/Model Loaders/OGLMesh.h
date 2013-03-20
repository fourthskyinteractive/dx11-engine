#ifndef _OGLMESH_H_
#define _OGLMESH_H_

#include "stdafx.h"
#include <fbxsdk.h>

namespace OGL
{
	class OGLVertexBufferObject;
	class OGLMesh
	{
	public:
		OGLMesh();
		virtual ~OGLMesh();

		bool Load( const std::string& szName );
		void Destroy( void );
		void Draw( void );
		void Draw( GLuint unGroup );
		void DrawDebug( void );
		GLuint GetGroupCount( void );
		//OGLBoundingBox& GetBoundingBox( void );
		
		static void EnableComputeNormals( bool bEnable );

	private:
		bool LoadObj( const std::string& szFile );
		bool LoadFBX( const std::string& szFile );

		void FBXTriangulateRecursive( FbxNode* pNode );
		void FBXFillMeshArrayRecursive( FbxNode* pNode, FbxArray<FbxMesh*>& pMeshArray );
		void FBXFillMeshArray( FbxScene* pScene, FbxArray<FbxMesh*>& pMeshArray );

		void ComputeVBO( void );
		void ComputeNormals( void );
		void ComputeTangents( void );
		//void ComputeBoundingBox( void );

		struct sFace
		{
			GLuint ind[3];
		};

		struct sGroup
		{
			std::string szName;
			long lMaterial;
			std::vector<sFace> vFaces;
		};

		std::vector< sGroup > m_vGroups;
		//OGLBoundingBox m_cBoundingBox;
		OGLVertexBufferObject* m_pVBO;
		GLBatch m_glDebugBatch;
		static bool m_sbComputeNormals;
	};
}

#endif