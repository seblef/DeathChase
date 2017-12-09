
#include "TreeGenerator.h"
#include "../Core/MeshMgr.h"
#include "../Core/MaterialMgr.h"
#include "../Core/ShaderMgr.h"
#include "../Core/TextureMgr.h"
#include "../Dirs.h"
#include <math.h>

namespace Game
{

TreeGenerator::~TreeGenerator()
{
	for(int i=0;i<TREE_COUNT;i++)
		g_MeshMgr.ReleaseMesh(m_hMeshes[i]);
}

bool TreeGenerator::GetTreeMesh(const TreeType _Type,
				 Core::HMesh& hNormal,
				 Core::HMesh& hBurned)
{
	if(_Type < 0 || _Type >= TREE_COUNT) return false;

	TreeType burned=TREE_PINBURN;
	if(_Type >= TREE_PINNORMAL && _Type <= TREE_PIN4SNOW)
		burned=TREE_PINBURN;
	else if(_Type >= TREE_PALMNORMAL && _Type <= TREE_PALMB7)
		burned=TREE_PALMBURN;

	if(m_hMeshes[_Type].IsNull())
	{
		if(!Generate(_Type))
			return false;
	}

	if(m_hMeshes[burned].IsNull())
	{
		if(!Generate(burned))
			return false;
	}

	hNormal=m_hMeshes[_Type];
	hBurned=m_hMeshes[burned];

	return true;
}

bool TreeGenerator::Generate(const TreeType _Type)
{
	Core::TDynamicTab<Core::VertexPNDTT> vertices;
	Core::TDynamicTab<uint16> indices;
	Core::TDynamicTab<Core::VertexPNDTT> svertices;
	Core::TDynamicTab<uint16> sindices;

	vertices.Initialize(128);
	indices.Initialize(128*3);

	svertices.Initialize(128);
	sindices.Initialize(128*3);

	bool bNoLOD=false;

	if(_Type==TREE_PINNORMAL)
	{
		//			Pin
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.625f,0.875f,0.7f,0.3f,0.1f);
		GeneratePinLevel(&vertices,&indices,0.375f,0.875f,0.5f,0.7f,0.2f);
		GeneratePinLevel(&vertices,&indices,0.125f,0.875f,0.3f,1.1f,0.5f);

		//			Pin small
		GeneratePinLevel(&svertices,&sindices,0.625f,0.875f,0.7f,1.1f,0.1f);
	}
	else if(_Type==TREE_PINSNOW)
	{
		//			Pine snow
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.625f,0.625f,0.7f,0.3f,0.1f);
		GeneratePinLevel(&vertices,&indices,0.375f,0.625f,0.5f,0.7f,0.2f);
		GeneratePinLevel(&vertices,&indices,0.125f,0.625f,0.3f,1.1f,0.5f);

		//			Pine snow small
		GeneratePinLevel(&svertices,&sindices,0.625f,0.625f,0.7f,1.1f,0.1f);
	}
	else if(_Type==TREE_PINEYELLOW)
	{
		//			Yellow pine
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.625f,0.375f,0.7f,0.3f,0.1f);
		GeneratePinLevel(&vertices,&indices,0.375f,0.375f,0.5f,0.7f,0.2f);
		GeneratePinLevel(&vertices,&indices,0.125f,0.375f,0.3f,1.1f,0.5f);

		//			Yellow pine small
		GeneratePinLevel(&svertices,&sindices,0.625f,0.375f,0.7f,1.1f,0.1f);
	}
	else if(_Type==TREE_PINLIGHT)
	{
		//			Pine light
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.625f,0.125f,0.7f,0.3f,0.1f);
		GeneratePinLevel(&vertices,&indices,0.375f,0.125f,0.5f,0.7f,0.2f);
		GeneratePinLevel(&vertices,&indices,0.125f,0.125f,0.3f,1.1f,0.5f);

		//			Pine light small
		GeneratePinLevel(&svertices,&sindices,0.625f,0.125f,0.7f,1.1f,0.1f);
	}
	else if(_Type==TREE_PINBURN)
	{
		//			Pine burn
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.875f,0.875f,0.7f,0.3f,0.1f);
		GeneratePinLevel(&vertices,&indices,0.875f,0.875f,0.5f,0.7f,0.2f);
		GeneratePinLevel(&vertices,&indices,0.875f,0.875f,0.3f,1.1f,0.5f);

		//			Pine burn small
		GeneratePinTrunc(&svertices,&sindices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&svertices,&sindices,0.875f,0.875f,0.7f,1.1f,0.1f);
	}
	else if(_Type==TREE_PIN2)
	{
		//			Pine2
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.836f,0.7f,0.3f,0.1f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.836f,0.5f,0.7f,0.2f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.836f,0.3f,1.1f,0.5f,0.18f,8.0f);

		//			Pine2 small
		GeneratePinLevel(&svertices,&sindices,0.18f,0.836f,0.7f,1.1f,0.1f,0.18f,8.0f);
	}
	else if(_Type==TREE_PIN3)
	{
		//			Pine3
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.5f,0.7f,0.3f,0.1f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.5f,0.5f,0.7f,0.2f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.5f,0.3f,1.1f,0.5f,0.18f,8.0f);

		//			Pine3 small
		GeneratePinLevel(&svertices,&sindices,0.18f,0.5f,0.7f,1.1f,0.1f,0.18f,8.0f);
	}
	else if(_Type==TREE_PIN4)
	{
		//			Pine4
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.172f,0.7f,0.3f,0.1f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.172f,0.5f,0.7f,0.2f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.18f,0.172f,0.3f,1.1f,0.5f,0.18f,8.0f);

		//			Pine4 small
		GeneratePinLevel(&svertices,&sindices,0.18f,0.172f,0.7f,1.1f,0.1f,0.18f,8.0f);
	}
	else if(_Type==TREE_PIN2SNOW)
	{
		//			Pine snow 2
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.836f,0.7f,0.3f,0.1f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.836f,0.5f,0.7f,0.2f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.836f,0.3f,1.1f,0.5f,0.18f,8.0f);

		//			Pine snow 2 small
		GeneratePinLevel(&svertices,&sindices,0.52f,0.836f,0.7f,1.1f,0.1f,0.18f,8.0f);
	}
	else if(_Type==TREE_PIN3SNOW)
	{
		//			Pine snow 3
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.5f,0.7f,0.3f,0.1f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.5f,0.5f,0.7f,0.2f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.5f,0.3f,1.1f,0.5f,0.18f,8.0f);

		//			Pine snow 3 small
		GeneratePinLevel(&svertices,&sindices,0.52f,0.5f,0.7f,1.1f,0.1f,0.18f,8.0f);
	}
	else if(_Type==TREE_PIN4SNOW)
	{
		//			Pine snow 4
		GeneratePinTrunc(&vertices,&indices,0.1f,1.1f,0.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.172f,0.7f,0.3f,0.1f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.172f,0.5f,0.7f,0.2f,0.18f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.52f,0.172f,0.3f,1.1f,0.5f,0.18f,8.0f);

		//			Pine snow 4 small
		GeneratePinLevel(&svertices,&sindices,0.52f,0.172f,0.7f,1.1f,0.1f,0.18f,8.0f);
	}
	else if(_Type==TREE_PALMNORMAL)
	{
		//			Palm
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.7f,5.0f);
		GeneratePalmLevel(&vertices,&indices,0.0f,0.6f,0.6f,0.8f,7.0f,0.0f,0.365f);

		//			Palm small
		GeneratePalmTrunc(&svertices,&sindices,0.07f,0.7f,5.0f);
		GeneratePalmLevel(&svertices,&sindices,0.0f,0.6f,0.6f,0.8f,3.0f,0.0f,0.365f);
	}
	else if(_Type==TREE_PALM2)
	{
		//			Palm2
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.7f,5.0f);
		GeneratePalmLevel(&vertices,&indices,0.0f,0.6f,0.6f,0.8f,7.0f,0.39f,0.365f);

		//			Palm2 small
		GeneratePalmTrunc(&svertices,&sindices,0.07f,0.7f,5.0f);
		GeneratePalmLevel(&svertices,&sindices,0.0f,0.6f,0.6f,0.8f,3.0f,0.39f,0.365f);
	}
	else if(_Type==TREE_PALM3)
	{
		//			Palm3
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.7f,5.0f);
		GeneratePalmLevel(&vertices,&indices,0.0f,0.6f,0.6f,0.8f,7.0f,0.0f,0.25f);

		//			Palm3 small
		GeneratePalmTrunc(&svertices,&sindices,0.07f,0.7f,3.0f);
		GeneratePalmLevel(&svertices,&sindices,0.0f,0.6f,0.6f,0.8f,3.0f,0.0f,0.25f);
	}
	else if(_Type==TREE_PALM4)
	{
		//			Palm4
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.7f,5.0f);
		GeneratePalmLevel(&vertices,&indices,0.0f,0.6f,0.6f,0.8f,3.0f,0.39f,0.25f);

		//			Palm4 small
		GeneratePalmTrunc(&svertices,&sindices,0.07f,0.7f,3.0f);
		GeneratePalmLevel(&svertices,&sindices,0.0f,0.6f,0.6f,0.8f,3.0f,0.39f,0.25f);
	}
	else if(_Type==TREE_PALMB)
	{
		//			PalmB
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.5f,5.0f,0.664f,0.0f,0.172f,0.656f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.836f,0.5f,0.4f,0.3f,0.164f,8.0f,true,15.0f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.836f,0.4f,0.5f,0.4f,0.164f,8.0f,false);
		bNoLOD=true;
	}
	else if(_Type==TREE_PALMB2)
	{
		//			PalmB2
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.5f,5.0f,0.664f,0.0f,0.172f,0.656f);
		GeneratePinLevel(&vertices,&indices,0.5f,0.836f,0.5f,0.4f,0.3f,0.164f,8.0f,true,15.0f);
		GeneratePinLevel(&vertices,&indices,0.5f,0.836f,0.4f,0.5f,0.4f,0.164f,8.0f,false);
		bNoLOD=true;
	}
	else if(_Type==TREE_PALMB3)
	{
		//			PalmB3
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.5f,5.0f,0.664f,0.0f,0.172f,0.656f);
		GeneratePinLevel(&vertices,&indices,0.836f,0.836f,0.5f,0.4f,0.3f,0.164f,8.0f,true,15.0f);
		GeneratePinLevel(&vertices,&indices,0.836f,0.836f,0.4f,0.5f,0.4f,0.164f,8.0f,false);
		bNoLOD=true;
	}
	else if(_Type==TREE_PALMB4)
	{
		//			PalmB4
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.5f,5.0f,0.664f,0.0f,0.172f,0.656f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.5f,0.5f,0.4f,0.3f,0.164f,8.0f,true,15.0f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.5f,0.4f,0.5f,0.4f,0.164f,8.0f,false);
		bNoLOD=true;
	}
	else if(_Type==TREE_PALMB5)
	{
		//			PalmB5
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.5f,5.0f,0.664f,0.0f,0.172f,0.656f);
		GeneratePinLevel(&vertices,&indices,0.5f,0.5f,0.5f,0.4f,0.3f,0.164f,8.0f,true,15.0f);
		GeneratePinLevel(&vertices,&indices,0.5f,0.5f,0.4f,0.5f,0.4f,0.164f,8.0f,false);
		bNoLOD=true;
	}
	else if(_Type==TREE_PALMB6)
	{
		//			PalmB6
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.5f,5.0f,0.664f,0.0f,0.172f,0.656f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.164f,0.5f,0.4f,0.3f,0.164f,8.0f,true,15.0f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.164f,0.4f,0.5f,0.4f,0.164f,8.0f,false);
		bNoLOD=true;
	}
	else if(_Type==TREE_PALMB7)
	{
		//			PalmB7
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.5f,5.0f,0.664f,0.0f,0.172f,0.656f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.5f,0.5f,0.4f,0.3f,0.164f,8.0f,true,15.0f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.5f,0.4f,0.5f,0.4f,0.164f,8.0f,false);
		bNoLOD=true;
	}
	else if(_Type==TREE_PALMBURN)
	{
		//			Palm burn
		GeneratePalmTrunc(&vertices,&indices,0.07f,0.7f,5.0f);

		//			Palm burn small
		GeneratePalmTrunc(&svertices,&sindices,0.07f,0.7f,3.0f);
	}
	else if(_Type==TREE_OAK)
	{
		//			Oak
		GeneratePinTrunc(&vertices,&indices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.5f,0.836f,0.4f,0.5f,0.3f,0.164f,8.0f,false,15.0f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.836f,0.3f,0.6f,0.5f,0.164f,8.0f,false);
		GeneratePinLevel(&vertices,&indices,0.5f,0.836f,0.2f,0.8f,0.6f,0.164f,8.0f,true,15.0f);

		//			Oak small
		GeneratePinTrunc(&svertices,&sindices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&svertices,&sindices,0.164f,0.836f,0.3f,0.6f,0.5f,0.164f,8.0f,true);
	}
	else if(_Type==TREE_OAK2)
	{
		//			Oak2
		GeneratePinTrunc(&vertices,&indices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.5f,0.4f,0.5f,0.3f,0.164f,8.0f,false,15.0f);
		GeneratePinLevel(&vertices,&indices,0.836f,0.836f,0.3f,0.6f,0.5f,0.164f,8.0f,false);
		GeneratePinLevel(&vertices,&indices,0.164f,0.5f,0.2f,0.8f,0.6f,0.164f,8.0f,true,15.0f);

		//			Oak2 small
		GeneratePinTrunc(&svertices,&sindices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&svertices,&sindices,0.836f,0.836f,0.3f,0.6f,0.5f,0.164f,8.0f,true);
	}
	else if(_Type==TREE_OAK3)
	{
		//			Oak3
		GeneratePinTrunc(&vertices,&indices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.836f,0.5f,0.4f,0.5f,0.3f,0.164f,8.0f,false,15.0f);
		GeneratePinLevel(&vertices,&indices,0.5f,0.5f,0.3f,0.6f,0.5f,0.164f,8.0f,false);
		GeneratePinLevel(&vertices,&indices,0.836f,0.5f,0.2f,0.8f,0.6f,0.164f,8.0f,true,15.0f);

		//			Oak3 small
		GeneratePinTrunc(&svertices,&sindices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&svertices,&sindices,0.5f,0.5f,0.3f,0.6f,0.5f,0.164f,8.0f,true);
	}
	else if(_Type==TREE_OAK4)
	{
		//			Oak4
		GeneratePinTrunc(&vertices,&indices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&vertices,&indices,0.5f,0.164f,0.4f,0.5f,0.3f,0.164f,8.0f,false,15.0f);
		GeneratePinLevel(&vertices,&indices,0.164f,0.164f,0.3f,0.6f,0.5f,0.164f,8.0f,false);
		GeneratePinLevel(&vertices,&indices,0.5f,0.164f,0.2f,0.8f,0.6f,0.164f,8.0f,true,15.0f);

		//			Oak4 small
		GeneratePinTrunc(&svertices,&sindices,0.02f,0.75f,0.0f,0.836f,0.0f,0.164f,0.164f,8.0f);
		GeneratePinLevel(&svertices,&sindices,0.164f,0.164f,0.3f,0.6f,0.5f,0.164f,8.0f, true);
	}
	else
		return false;

	m_hMeshes[_Type]=g_MeshMgr.CreateEmptyMesh(szTreeNames[_Type]);
	if(m_hMeshes[_Type].IsNull()) return false;

	Core::HMaterial hMat=g_MaterialMgr.CreateEmptyMaterial(szTreeNames[_Type]);
	if(hMat.IsNull())
	{
		g_MeshMgr.ReleaseMesh(m_hMeshes[_Type]);
		m_hMeshes[_Type]=Core::HMesh();
		return false;
	}

	const char *szTexNames[TREE_COUNT]=
	{
		"pineA.tga",
		"pineA.tga",
		"pineA.tga",
		"pineA.tga",
		"pineA.tga",
		"pineB.tga",
		"pineB.tga",
		"pineB.tga",
		"pineB.tga",
		"pineB.tga",
		"pineB.tga",

		"palmA.tga",
		"palmA.tga",
		"palmA.tga",
		"palmA.tga",
		"palmA.tga",
		"palmB.tga",
		"palmB.tga",
		"palmB.tga",
		"palmB.tga",
		"palmB.tga",
		"palmB.tga",
		"palmB.tga",

		"oak.tga",
		"oak.tga",
		"oak.tga",
		"oak.tga"
	};

	Core::Material *pMat=(Core::Material *)g_MaterialMgr.GetMaterial(hMat);
	pMat->SetSrcBlend(D3DBLEND_SRCALPHA);
	pMat->SetDstBlend(D3DBLEND_INVSRCALPHA);
	pMat->SetCullMode(D3DCULL_NONE);
	pMat->SetShader(RENDERINGPASS_AMBIENT,g_ShaderMgr.LoadShader("StdAmbient_Tex"));
	pMat->SetShader(RENDERINGPASS_SUN,g_ShaderMgr.LoadShader("StdSun_Tex"));
//	pMat->SetShader(RENDERINGPASS_LIGHT,g_ShaderMgr.LoadShader("StdLight_NoEffect"));
	pMat->AddTexture(g_TextureMgr.LoadTexture(szTexNames[_Type],DIR_TREES));

	Core::SubMesh* pSub=Core::SubMesh::GetNew();
	pSub->Construct(0,indices.GetItemCount() / 3,hMat);

	Core::Mesh *pMesh=(Core::Mesh *)g_MeshMgr.GetMesh(m_hMeshes[_Type]);
	pMesh->Construct(szTreeNames[_Type],vertices.GetItemCount(),vertices.GetBuffer(),
		indices.GetItemCount() / 3,indices.GetBuffer(),pSub,1000);

	if(!bNoLOD)
	{
		pSub=Core::SubMesh::GetNew();
		pSub->Construct(0,sindices.GetItemCount()/3,hMat);
		Core::Mesh *pLOD=Core::Mesh::GetNew();
		pLOD->Construct(szTreeNames[_Type],svertices.GetItemCount(),svertices.GetBuffer(),
			sindices.GetItemCount()/3,sindices.GetBuffer(),pSub,0xffffff);
		pMesh->AddLOD(pLOD,0xffffff);
	}

	return true;
}

void TreeGenerator::GeneratePinLevel(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
								  Core::TDynamicTab<uint16> *pIndices,
								  float fTexU, float fTexV,
								  float fWidth, float fHeight,
								  float fLowHeight,float fTexWidth,
								  float fCount, bool bDoubleSide,
								  float fAngOffset,
								  const Core::Color32& cColor)
{
	Core::VertexPNDTT vert;
	vert.vUV0=Core::Vector2(fTexU,fTexV);
	vert.vNorm=Core::Vector3::YAxisVector;
	vert.vPos=Core::Vector3(0.0f,fHeight,0.0f);
	vert.cColor=cColor;
	
	uint16 iFirst=pVertices->GetItemCount();
	pVertices->Add(vert);

	float i;
	for(i=360.0f; i>= 0.0f; i-=360.0f / fCount)
	{
		float fDiff=RAND * 40.0f - 20.0f;
		vert.vUV0=Core::Vector2(fTexU + (sinf((i+fAngOffset) / 180.0f * PI) * fTexWidth),
								 fTexV + (cosf((i+fAngOffset) / 180.0f * PI) * fTexWidth));
		vert.vNorm=Core::Vector3(sinf((i+fDiff) / 180.0f * PI) * (fHeight - fLowHeight),
			fWidth,
			cosf((i+fDiff) / 180.0f * PI) * (fHeight - fLowHeight));
		vert.vNorm.Normalize();
		vert.vPos=Core::Vector3(sinf(i / 180.0f * PI) * fWidth,
			fLowHeight,
			cosf(i / 180.0f * PI) * fWidth);

		pVertices->Add(vert);

		if((pVertices->GetItemCount()-iFirst) >= 3)
		{
			pIndices->Add(iFirst);
			pIndices->Add((uint16)pVertices->GetItemCount() - 2);
			pIndices->Add((uint16)pVertices->GetItemCount() - 1);
		}
	}

	if(bDoubleSide)
	{
		vert.vUV0=Core::Vector2(fTexU,fTexV);
		vert.vNorm=Core::Vector3::YAxisVector;
		vert.vPos=Core::Vector3(0.0f,fHeight,0.0f);

		iFirst=pVertices->GetItemCount();
		pVertices->Add(vert);

		for(i=0.0f;i < 360.0f; i+=360.0f / fCount)
		{
			float fDiff=RAND * 40.0f - 20.0f;
			vert.vUV0=Core::Vector2(fTexU + (sinf((i+fAngOffset) / 180.0f * PI) * fTexWidth),
									 fTexV + (cosf((i+fAngOffset) / 180.0f * PI) * fTexWidth));
			vert.vNorm=Core::Vector3(-sinf((i+fDiff) / 180.0f * PI) * (fHeight - fLowHeight),
				-fWidth,
				-cosf((i+fDiff) / 180.0f * PI) * (fHeight - fLowHeight));
			vert.vNorm.Normalize();
			vert.vPos=Core::Vector3(sinf(i / 180.0f * PI) * fWidth,
				fLowHeight,
				cosf(i / 180.0f * PI) * fWidth);

			pVertices->Add(vert);

			if((pVertices->GetItemCount()-iFirst) >= 3)
			{
				pIndices->Add(iFirst);
				pIndices->Add((uint16)pVertices->GetItemCount() - 2);
				pIndices->Add((uint16)pVertices->GetItemCount() - 1);
			}
		}
	}
}

void TreeGenerator::GeneratePinTrunc(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
								  Core::TDynamicTab<uint16> *pIndices,
								  float fWidth, float fHeight, float fLowHeight,
								  float x, float y, float w, float h, float fSteps,
								  const Core::Color32& cColor)
{
	Core::VertexPNDTT vert;
	vert.vUV0=Core::Vector2(x,y);
	vert.vNorm=Core::Vector3::YAxisVector;
	vert.vPos=Core::Vector3(0.0f,fHeight,0.0f);
	vert.cColor=cColor;

	uint16 iFirst=pVertices->GetItemCount();
	pVertices->Add(vert);

	float i;
	for(i=360.0f; i>= 0.0f; i-=360.0f / fSteps)
	{
		vert.vUV0=Core::Vector2(x + w*(float(int(i*5.0f)%360) / 360.0f),y+h);
		vert.vNorm=Core::Vector3(sinf(i/180.0f*PI),0.0f,cosf(i/180.0f*PI));
		vert.vPos=Core::Vector3(sinf(i/180.0f*PI)*fWidth,fLowHeight,cosf(i/180.0f*PI)*fWidth);

		pVertices->Add(vert);
		if((pVertices->GetItemCount()-iFirst) >= 3)
		{
			pIndices->Add(iFirst);
			pIndices->Add((uint16)pVertices->GetItemCount() - 2);
			pIndices->Add((uint16)pVertices->GetItemCount() - 1);
		}
	}
}

void TreeGenerator::GeneratePalmTrunc(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
								   Core::TDynamicTab<uint16> *pIndices,
								   float fWidth, float fHeight, float fCount,
								   float x, float y, float w, float h,
								   const Core::Color32& cColor)
{
	bool tex=false;
	int iCurrentPoint=0;

	Core::VertexPNDTT vert1,vert2;
	vert1.cColor=cColor;
	vert2.cColor=cColor;
	float i;
	
	for(i=360.0f;i>=0.0f;i-=360.0f / fCount)
	{
		vert1.vNorm=vert2.vNorm=Core::Vector3(sinf(i/180.0f*PI),0.0f,cosf(i/180.0f*PI));

		if(tex) vert1.vUV0=Core::Vector2(x,y);
		else vert1.vUV0=Core::Vector2(x,y+h);
		vert1.vPos=Core::Vector3(sinf(i/180.0f*PI) * (fWidth-0.05f),fHeight,cosf(i/180.0f*PI) * (fWidth-0.05f));

		if(tex) vert2.vUV0=Core::Vector2(x+w,y);
		else vert2.vUV0=Core::Vector2(x+w,y+h);
		vert2.vPos=Core::Vector3(sinf(i/180.0f*PI)*fWidth,0.0f,cosf(i/180.0f*PI)*fWidth);

		pVertices->Add(vert1);
		pVertices->Add(vert2);
		iCurrentPoint+=2;

		if(iCurrentPoint>=4)
		{
			uint16 ind[6];
			ind[0]=pVertices->GetItemCount()-4;
			ind[1]=ind[0]+1;
			ind[2]=ind[0]+2;

			ind[3]=ind[0]+1;
			ind[4]=ind[0]+3;
			ind[5]=ind[0]+2;

			pIndices->AddTab(ind,6);
			iCurrentPoint=2;
		}

		tex=!tex;
	}

	vert1.vUV0=Core::Vector2(x+w,y);
	vert1.vNorm=Core::Vector3::YAxisVector;
	vert1.vPos=Core::Vector3(0.0f,fHeight + 0.025f,0.0f);

	uint16 iFirst=pVertices->GetItemCount();
	pVertices->Add(vert1);

	for(i=360.0f;i>=0.0f;i-=360.0f / fCount)
	{
		if(tex) vert1.vUV0=Core::Vector2(x+w,y);
		else vert1.vUV0=Core::Vector2(x+w,y+h);
		vert1.vNorm=Core::Vector3(sinf(i/180.0f*PI),0.0f,cosf(i/180.0f*PI));
		vert1.vPos=Core::Vector3(sinf(i/180.0f*PI)*0.03f,fHeight-0.01f,cosf(i/180.0f*PI) * 0.03f);

		pVertices->Add(vert1);
		if((pVertices->GetItemCount()-iFirst) >= 3)
		{
			pIndices->Add(iFirst);
			pIndices->Add((uint16)pVertices->GetItemCount() - 2);
			pIndices->Add((uint16)pVertices->GetItemCount() - 1);
		}

		tex=!tex;
	}
}

void TreeGenerator::GeneratePalmLevel(Core::TDynamicTab<Core::VertexPNDTT> *pVertices,
								   Core::TDynamicTab<uint16> *pIndices,
								   float fWidth1, float fWidth2, float fHeight, float fHeight2,
								   float fCount, float fTexU, float fTexV,
								   const Core::Color32 &cColor)
{
	Core::VertexPNDTT vert;
	vert.cColor=cColor;
	uint16 ind[6];

	float i;
	for(i=360.0f;i>=0.0f;)
	{
		float fDiff=0.5f * RAND - 0.25f;
		float fWidth2_=(fWidth2 *RAND * 0.3f) + (0.7 * fWidth2);

		Core::Vector3 A1(sinf((i-15.0f)/180.0f*PI)*fWidth1,fHeight,cosf((i-15.0f)/180.0f*PI)*fWidth1);
		Core::Vector3 A2(sinf(i/180.0f*PI)*fWidth1,fHeight2,cosf(i/180.0f*PI)*fWidth1);
		Core::Vector3 A3(sinf(i/180.0f*PI)*fWidth2_,fHeight2+fDiff,cosf(i/180.0f*PI)*fWidth2_);
		Core::Vector3 A4(sinf((i-15.0f)/180.0f*PI)*fWidth2_,fHeight+fDiff,cosf((i-15.0f)/180.0f*PI)*fWidth2_);
		Core::Vector3 AN((A3-A4) ^ (A3-A2));
		AN.Normalize();

		vert.vNorm=AN; vert.vPos=A1; vert.vUV0=Core::Vector2(fTexU,fTexV);
		pVertices->Add(vert);
		vert.vPos=A2; vert.vUV0=Core::Vector2(fTexU,fTexV + 0.123f);
		pVertices->Add(vert);
		vert.vPos=A3; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV+0.123f);
		pVertices->Add(vert);
		vert.vPos=A4; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV);
		pVertices->Add(vert);

		ind[0]=pVertices->GetItemCount()-4;
		ind[1]=ind[0]+1; ind[2]=ind[0]+3;
		ind[3]=ind[0]+1; ind[4]=ind[0]+2; ind[5]=ind[0]+3;
		pIndices->AddTab(ind,6);

		Core::Vector3 B1(sinf((i-15.0f)/180.0f*PI)*fWidth2_,fHeight+fDiff,cosf((i-15.0f)/180.0f*PI)*fWidth2_);
		Core::Vector3 B2(sinf(i/180.0f*PI)*fWidth2_,fHeight2+fDiff,cosf(i/180.0f*PI)*fWidth2_);
		Core::Vector3 B3(sinf(i/180.0f*PI)*fWidth1,fHeight2,cosf(i/180.0f*PI)*fWidth1);
		Core::Vector3 B4(sinf((i-15.0f)/180.0f*PI)*fWidth1,fHeight,cosf((i-15.0f)/180.0f*PI)*fWidth1);
		Core::Vector3 BN((B1-B3)^(B2-B1));
		BN.Normalize();

		vert.vNorm=BN; vert.vPos=B1; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV);
		pVertices->Add(vert);
		vert.vPos=B2; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV+0.123f);
		pVertices->Add(vert);
		vert.vPos=B3; vert.vUV0=Core::Vector2(fTexU,fTexV+0.123f);
		pVertices->Add(vert);
		vert.vPos=B4; vert.vUV0=Core::Vector2(fTexU,fTexV);
		pVertices->Add(vert);

		ind[0]=pVertices->GetItemCount()-4;
		ind[1]=ind[0]+1; ind[2]=ind[0]+3;
		ind[3]=ind[0]+1; ind[4]=ind[0]+2; ind[5]=ind[0]+3;
		pIndices->AddTab(ind,6);

		Core::Vector3 C1(sinf((i+15.0f)/180.0f*PI)*fWidth2_,fHeight+fDiff,cosf((i+15.0f)/180.0f*PI)*fWidth2_);
		Core::Vector3 C2(sinf(i/180.0f*PI)*fWidth2_,fHeight2+fDiff,cosf(i/180.0f*PI)*fWidth2_);
		Core::Vector3 C3(sinf(i/180.0f*PI)*fWidth1,fHeight2,cosf(i/180.0f*PI)*fWidth1);
		Core::Vector3 C4(sinf((i+15.0f)/180.0f*PI)*fWidth1,fHeight,cosf((i+15.0f)/180.0f*PI)*fWidth1);
		Core::Vector3 CN((C2-C1)^(C3-C2));
		CN.Normalize();

		vert.vNorm=CN;
		vert.vPos=C1; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV);
		pVertices->Add(vert);
		vert.vPos=C2; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV+0.123f);
		pVertices->Add(vert);
		vert.vPos=C3; vert.vUV0=Core::Vector2(fTexU,fTexV+0.123f);
		pVertices->Add(vert);
		vert.vPos=C4; vert.vUV0=Core::Vector2(fTexU,fTexV);
		pVertices->Add(vert);

		ind[0]=pVertices->GetItemCount()-4;
		ind[1]=ind[0]+1; ind[2]=ind[0]+3;
		ind[3]=ind[0]+1; ind[4]=ind[0]+2; ind[5]=ind[0]+3;
		pIndices->AddTab(ind,6);

		Core::Vector3 D1(sinf((i+15.0f)/180.0f*PI)*fWidth1,fHeight,cosf((i+15.0f)/180.0f*PI)*fWidth1);
		Core::Vector3 D2(sinf(i/180.0f*PI)*fWidth1,fHeight2,cosf(i/180.0f*PI)*fWidth1);
		Core::Vector3 D3(sinf(i/180.0f*PI)*fWidth2_,fHeight2+fDiff,cosf(i/180.0f*PI)*fWidth2_);
		Core::Vector3 D4(sinf((i+15.0f)/180.0f*PI)*fWidth2_,fHeight+fDiff,cosf((i+15.0f)/180.0f*PI)*fWidth2_);
		Core::Vector3 DN((D4-D3)^(D2-D4));
		DN.Normalize();

		vert.vNorm=DN;
		vert.vPos=D1; vert.vUV0=Core::Vector2(fTexU,fTexV);
		pVertices->Add(vert);
		vert.vPos=D2; vert.vUV0=Core::Vector2(fTexU,fTexV+0.123f);
		pVertices->Add(vert);
		vert.vPos=D3; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV+0.123f);
		pVertices->Add(vert);
		vert.vPos=D4; vert.vUV0=Core::Vector2(fTexU+0.37f,fTexV);
		pVertices->Add(vert);

		ind[0]=pVertices->GetItemCount()-4;
		ind[1]=ind[0]+1; ind[2]=ind[0]+3;
		ind[3]=ind[0]+1; ind[4]=ind[0]+2; ind[5]=ind[0]+3;
		pIndices->AddTab(ind,6);

		i-=(360.0f / (fCount + (fCount-1)*RAND));
	}
}

}