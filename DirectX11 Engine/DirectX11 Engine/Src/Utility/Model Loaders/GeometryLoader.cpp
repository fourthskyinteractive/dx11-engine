void FBXFillCameraArrayRecursive( FbxNode* pNode, FbxArray<FbxNode*>& pCameraArray)
{
	if( pNode )
	{
		if( pNode->GetNodeAttribute() )
		{
			if( pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera )
			{
				pCameraArray.Add( pNode );
			}
		}

		const int lCount = pNode->GetChildCount();
		for( int i = 0; i < lCount; i++ )
			FBXFillCameraArrayRecursive( pNode->GetChild(i), pCameraArray );
	}
}

void FBXFillCameraArray( FbxScene* pScene, FbxArray<FbxNode*>& pCameraArray )
{
	pCameraArray.Clear();
	FBXFillCameraArrayRecursive( pScene->GetRootNode(), pCameraArray );
}

void FBXFillPoseArray( FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray )
{
	const int nPoseCount = pScene->GetPoseCount();

	for( int i = 0; i < nPoseCount; i++ )
	{
		pPoseArray.Add( pScene->GetPose( i ) );
	}
}

void FBXFillMeshArrayRecursive( FbxNode* pNode, FbxArray<FbxMesh*>& pMeshArray )
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

void FBXFillMeshArray( FbxScene* pScene, FbxArray<FbxMesh*>& pMeshArray )
{
	pMeshArray.Clear();
	FBXFillMeshArrayRecursive( pScene->GetRootNode(), pMeshArray );
}

void FBXTriangulateRecursive( FbxNode* pNode )
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

void GeometryLoaderDX11::loadFBXFile( std::string szFilename, std::vector<GeometryPtr>& vGeomVector, std::vector<std::string>& vNames )
{
	FileSystem fs;
	szFilename = fs.GetModelsFolderS() + szFilename;
	pFBXManager = FbxManager::Create();
	if( !pFBXManager )
		Log::Get().Write( L"CGeometryLoader11.cpp: Error creating FBX Manager!" );

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

	if (!pFBXManager->GetIOPluginRegistry()->DetectReaderFileFormat(szFilename.c_str(), lFileFormat) )
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		lFileFormat = pFBXManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
	}

	bool ImportStatus = pImporter->Initialize( szFilename.c_str(), lFileFormat, pFBXManager->GetIOSettings() );
	pImporter->GetFileVersion( nFileMinor, nFileMinor, nFileRevision );

	if( !ImportStatus )
	{
		Log::Get().Write( L"CGeometryLoader11.cpp: FbxImporter Initialize failed!" );
		return;
	}

	ImportStatus = pImporter->Import( pScene );

	if( !ImportStatus )
	{
		Log::Get().Write( L"CGeometryLoader11.cpp: FbxImporter failed to import the file to the scene!" );
		return;
	}

	FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem AxisSystem( FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded );
	if( SceneAxisSystem != AxisSystem )
	{
		AxisSystem.ConvertScene( pScene );
	}

	//FbxSystemUnit SceneSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();
	//if( SceneSystemUnit.GetScaleFactor() != 1.0f )
	//	FbxSystemUnit::cm.ConvertScene( pScene );

	FBXTriangulateRecursive( pScene->GetRootNode() );

	FbxArray<FbxMesh*> vMeshs;
	FBXFillMeshArray( pScene, vMeshs );

	unsigned short usVertexCount = 0;
	unsigned short usTriangleCount = 0;
	unsigned short usGroupCount = 0;
	unsigned short usMaterialCount = 0;
	unsigned short usIndicesCount = 0;

	for( i = 0; i < vMeshs.GetCount(); i++ )
	{
		Log::Get().Write( L"CGeometryLoader11.cpp: Loading File!" );
		std::string name = vMeshs[i]->GetNode()->GetName();
		vNames.push_back( name );
		usVertexCount = vMeshs[i]->GetControlPointsCount();
		if( usVertexCount == 0 )
			continue;

		usTriangleCount = vMeshs[i]->GetPolygonVertexCount() / 3;
		usIndicesCount = vMeshs[i]->GetPolygonVertexCount();

		VertexElementDX11* pPositions = new VertexElementDX11( 3, usTriangleCount * 3 );
		pPositions->m_SemanticName = VertexElementDX11::PositionSemantic;
		pPositions->m_uiSemanticIndex = 0;
		pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pPositions->m_uiInputSlot = 0;
		pPositions->m_uiAlignedByteOffset = 0;
		pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pPositions->m_uiInstanceDataStepRate = 0;

		VertexElementDX11* pTexCoords = new VertexElementDX11( 2, usTriangleCount * 3 );
		pTexCoords->m_SemanticName = VertexElementDX11::TexCoordSemantic;
		pTexCoords->m_uiSemanticIndex = 0;
		pTexCoords->m_Format = DXGI_FORMAT_R32G32_FLOAT;
		pTexCoords->m_uiInputSlot = 0;
		pTexCoords->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		pTexCoords->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pTexCoords->m_uiInstanceDataStepRate = 0;

		VertexElementDX11* pNormals = new VertexElementDX11( 3, usTriangleCount * 3 );
		pNormals->m_SemanticName = VertexElementDX11::NormalSemantic;
		pNormals->m_uiSemanticIndex = 0;
		pNormals->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pNormals->m_uiInputSlot = 0;
		pNormals->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		pNormals->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pNormals->m_uiInstanceDataStepRate = 0;

		Vector3f* pPos = pPositions->Get3f(0);
		Vector3f* pNorm = pNormals->Get3f(0);
		Vector2f* pTex = pTexCoords->Get2f(0);

		FbxVector4* pFBXVerts = new FbxVector4[usVertexCount];
		memcpy( pFBXVerts, vMeshs[i]->GetControlPoints(), usVertexCount * sizeof(FbxVector4));

		TriangleIndices face;

		GeometryPtr pGeomPointer( new GeometryDX11() );

		for( int j = 0; j < usTriangleCount; j++ )
		{	
			int nIndex = 0;
			FbxVector4 FBXNorm( 0, 0, 0, 0 );
			FbxVector2 FBXUV( 0, 0 );

			face.P1() = nIndex = vMeshs[i]->GetPolygonVertex( j, 0 );
			pPos[nIndex].x = (float)pFBXVerts[ nIndex ][0];
			pPos[nIndex].y = (float)pFBXVerts[ nIndex ][1];
			pPos[nIndex].z = (float)pFBXVerts[ nIndex ][2];
			vMeshs[i]->GetPolygonVertexNormal( j, 0, FBXNorm );
			pNorm[nIndex].x = (float)FBXNorm[0];
			pNorm[nIndex].y = (float)FBXNorm[1];
			pNorm[nIndex].z = (float)FBXNorm[2];
			vMeshs[i]->GetPolygonVertexUV( j, 0, "map1", FBXUV );
			pTex[nIndex].x = (float)FBXUV[0];
			pTex[nIndex].y = (float)FBXUV[1];

			face.P2() = nIndex = vMeshs[i]->GetPolygonVertex( j, 1 );
			pPos[nIndex].x = (float)pFBXVerts[ nIndex ][0];
			pPos[nIndex].y = (float)pFBXVerts[ nIndex ][1];
			pPos[nIndex].z = (float)pFBXVerts[ nIndex ][2];
			vMeshs[i]->GetPolygonVertexNormal( j, 1, FBXNorm );
			pNorm[nIndex].x = (float)FBXNorm[0];
			pNorm[nIndex].y = (float)FBXNorm[1];
			pNorm[nIndex].z = (float)FBXNorm[2];
			vMeshs[i]->GetPolygonVertexUV( j, 1, "map1", FBXUV );
			pTex[nIndex].x = (float)FBXUV[0];
			pTex[nIndex].y = (float)FBXUV[1];

			face.P3() = nIndex = vMeshs[i]->GetPolygonVertex( j, 2 );
			pPos[nIndex].x = (float)pFBXVerts[ nIndex ][0];
			pPos[nIndex].y = (float)pFBXVerts[ nIndex ][1];	
			pPos[nIndex].z = (float)pFBXVerts[ nIndex ][2];
			vMeshs[i]->GetPolygonVertexNormal( j, 2, FBXNorm );
			pNorm[nIndex].x = (float)FBXNorm[0];
			pNorm[nIndex].y = (float)FBXNorm[1];
			pNorm[nIndex].z = (float)FBXNorm[2];
			vMeshs[i]->GetPolygonVertexUV( j, 2, "map1", FBXUV );
			pTex[nIndex].x = (float)FBXUV[0];
			pTex[nIndex].y = (float)FBXUV[1];

			pGeomPointer->AddFace( face );
		}

		for( int j = 0; j < usVertexCount; j++ )
		{
			pNorm[j].Normalize();
		}

		pGeomPointer->AddElement( pPositions );
		pGeomPointer->AddElement( pNormals );
		pGeomPointer->AddElement( pTexCoords );

		delete[] pFBXVerts;
		vGeomVector.push_back( pGeomPointer );

		vMeshs[i]->Destroy();
		vMeshs[i] = NULL;
	}


	pImporter->Destroy();
	pImporter = NULL;

	pScene->Destroy();
	pScene = NULL;

	pIOS->Destroy();
	pIOS = NULL;

	pFBXManager->Destroy();
	pFBXManager = NULL;
}