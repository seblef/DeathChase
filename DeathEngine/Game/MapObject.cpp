
#include "IMapOBject.h"
#include "TreeGenerator.h"
#include "../Core/ViewFrustrum.h"
#include "../Core/MeshMgr.h"
#include "../Core/Renderer.h"
#include "../Dirs.h"

namespace Game
{

#define MAPOBJINFO_END				-1
#define MAPOBJINFO_BASECHUNK		0

struct MapObjectFileInfo
{
	struct _Base
	{
		char szType[64];
		char szBurnedModel[64];
		char szName[32];
		Core::Vector3 vPos;
		float fScale;
		Core::Quaternion qRot;
		Core::Vector2i vMapPos;
		bool bHasBurnedMesh;
		bool bCastShadows;
		bool bReceiveShadows;
		bool bLightPassEnable;

	} Base;
};

#ifdef DEEXPORT
MapObject::MapObject() : m_iRadius(TREE_RADIUS), m_fRadius(TREE_REALRADIUS),
			m_bBurned(false), m_fHeight(0.0f)
{
	m_bStatic=true;
	m_bCastShadows=true;
	m_bReceiveShadows=true;
	m_bLightPassEnable=true;
	m_bSpecialPassEnable=false;

	SetName("MapObject");
}

MapObject::MapObject(int iRadius, float fRadius) : m_iRadius(iRadius), m_fRadius(fRadius), m_fHeight(0.0f)
{
	m_bStatic=true;
	m_bCastShadows=true;
	m_bReceiveShadows=true;
	m_bLightPassEnable=true;
	m_bSpecialPassEnable=false;

	SetName("MapObject");
}
#else

MapObject::MapObject()  : IObject(OBJ_MAP), m_iRadius(TREE_RADIUS), m_fRadius(TREE_REALRADIUS),
			m_bBurned(false), m_fHeight(0.0f)
{
	m_bStatic=true;
	m_bCastShadows=true;
	m_bReceiveShadows=true;
	m_bLightPassEnable=true;
	m_bSpecialPassEnable=false;

	SetName("MapObject");
}

MapObject::MapObject(int iRadius, float fRadius) : IObject(OBJ_MAP), m_iRadius(iRadius), m_fRadius(fRadius), m_fHeight(0.0f)
{
	m_bStatic=true;
	m_bCastShadows=true;
	m_bReceiveShadows=true;
	m_bLightPassEnable=true;
	m_bSpecialPassEnable=false;

	SetName("MapObject");
}

#endif

void MapObject::Clear()
{
	if(m_Type==TREE_USER)
	{
		g_MeshMgr.ReleaseMesh(m_hNormalMesh);
		g_MeshMgr.ReleaseMesh(m_hBurnedMesh);
	}

	m_bStatic=true;
	m_bCastShadows=true;
	m_bReceiveShadows=true;
	m_bLightPassEnable=true;
	m_bSpecialPassEnable=false;

	m_bBurned=false;
	m_fHeight=0.0f;
	m_Type=TREE_PINNORMAL;

	IObject::Clear();
}

bool MapObject::Generate(const char *szName, const char *szBurnedModel)
{
	if(!szName) return false;

	bool bUser=true;
	for(int i=0;i<TREE_COUNT;i++)
	{
		if(strcmp(szTreeNames[i],szName)==0)
		{
			if(!g_TreeGenerator.GetTreeMesh((TreeType)i,m_hNormalMesh,m_hBurnedMesh))
				return false;
			
			m_Type=(TreeType)i;
			bUser=false;
		}
	}

	if(bUser)
	{
		m_hNormalMesh=g_MeshMgr.LoadMesh(szName,0);
		if(m_hNormalMesh.IsNull())
			return false;

		if(szBurnedModel)
			m_hBurnedMesh=g_MeshMgr.LoadMesh(szBurnedModel,0);

		m_Type=TREE_USER;
	}

	IMesh *pMesh=g_MeshMgr.GetMesh(m_hNormalMesh);
	if(pMesh)
	{
		m_BSphere.FromBBox(&pMesh->GetBBox());
		m_fHeight=-pMesh->GetBBox().m_vMin.y;
	}

	m_Transformation.SetBSpheres(&m_BSphere,&m_TransformedBSphere);
	m_Transformation.RebuildMatrix();

	m_iRadius=(int)m_TransformedBSphere.GetRadius();
	m_fRadius=m_TransformedBSphere.GetRadius();

	return true;
}

bool MapObject::DrawAmbient()
{
	if(m_hNormalMesh.IsNull()) return false;

	if(m_bBurned && !m_hBurnedMesh.IsNull())
		g_MeshMgr.DrawMesh(m_hBurnedMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());
	else
		g_MeshMgr.DrawMesh(m_hNormalMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());

	return true;
}

bool MapObject::DrawSunLight()
{
	if(m_hNormalMesh.IsNull()) return false;

	if(m_bBurned && !m_hBurnedMesh.IsNull())
		g_MeshMgr.DrawMesh(m_hBurnedMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());
	else
		g_MeshMgr.DrawMesh(m_hNormalMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());

	return true;
}

bool MapObject::Update(const float fTime)
{
	return true;
}

bool MapObject::Save(FILE *pFile)
{
	MapObjectFileInfo info;

	if(m_Type==TREE_USER)
	{
		IMesh *pMesh=g_MeshMgr.GetMesh(m_hNormalMesh);
		strcpy(info.Base.szType,pMesh ? pMesh->GetFileName() : "");

		if(m_hBurnedMesh.IsNull())
		{
			strcpy(info.Base.szBurnedModel,"");
			info.Base.bHasBurnedMesh=false;
		}
		else
		{
			pMesh=g_MeshMgr.GetMesh(m_hBurnedMesh);
			strcpy(info.Base.szBurnedModel,pMesh->GetFileName());
			info.Base.bHasBurnedMesh=true;
		}
	}
	else
		strcpy(info.Base.szType,szTreeNames[m_Type]);

	info.Base.vPos=m_Transformation.GetPosition();
	info.Base.vMapPos=m_vMapPos;
	info.Base.fScale=m_Transformation.GetScale().y;
	info.Base.qRot=m_Transformation.GetRotation();

	strcpy(info.Base.szName,GetName());

	info.Base.bCastShadows=m_bCastShadows;
	info.Base.bLightPassEnable=m_bLightPassEnable;
	info.Base.bReceiveShadows=m_bReceiveShadows;

	int iChunk=MAPOBJINFO_BASECHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Base,sizeof(info.Base),1,pFile);
	iChunk=MAPOBJINFO_END;
	fwrite(&iChunk,sizeof(int),1,pFile);

	return true;
}

void MapObject::InitializeMapObjectInfo(void *pInfo)
{
	MapObjectFileInfo *info=(MapObjectFileInfo *)pInfo;

	strcpy(info->Base.szType,"");
	strcpy(info->Base.szBurnedModel,"");
	strcpy(info->Base.szName,"MapObject");

	info->Base.vPos=Core::Vector3::NullVector;
	info->Base.fScale=1.0f;
	info->Base.qRot=Core::Quaternion();
	info->Base.vMapPos=Core::Vector2i::NullVector;
	info->Base.bHasBurnedMesh=false;
	info->Base.bCastShadows=true;
	info->Base.bReceiveShadows=true;
	info->Base.bLightPassEnable=true;
}


bool MapObject::Parse(FILE *pFile)
{
	MapObjectFileInfo info;
	InitializeMapObjectInfo(&info);

	int iChunk;
	fread(&iChunk,sizeof(int),1,pFile);

	while(iChunk!=MAPOBJINFO_END && !feof(pFile))
	{
		switch(iChunk)
		{
			case MAPOBJINFO_BASECHUNK:
				fread(&info.Base,sizeof(info.Base),1,pFile);
				break;

			default:
				return false;
		}

		fread(&iChunk,sizeof(int),1,pFile);
	}

	m_vMapPos=info.Base.vMapPos;

	m_Transformation.SetPosition(info.Base.vPos);
	m_Transformation.SetRotation(info.Base.qRot);
	m_Transformation.SetScale(Core::Vector3(info.Base.fScale,info.Base.fScale,info.Base.fScale));

	m_bLightPassEnable=info.Base.bLightPassEnable;
	m_bCastShadows=info.Base.bCastShadows;
	m_bReceiveShadows=info.Base.bReceiveShadows;

	SetName(info.Base.szName);

	return Generate(info.Base.szType,info.Base.bHasBurnedMesh ? info.Base.szBurnedModel : 0);
}

bool MapObject::CastSunShadows(const Core::Vector3& vSunDir)
{
	if(m_bMustRebuildShadowVolumes)
	{
		Core::EdgeSet *pEdges=g_MeshMgr.GetMeshEdgeSet(m_bBurned ? m_hBurnedMesh : m_hNormalMesh);
		if(pEdges)
			m_SunShadows.GenerateFromDirLight(pEdges,vSunDir,m_Transformation.GetInverse());

		SetShadowVolumesRebuilded();
	}

	if(m_SunShadows.GetVerticesCount() > 0)
	{
		g_Renderer.SetTransformation(TRANS_MODEL,m_Transformation.GetMatrix(),0,false);
		g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,m_SunShadows.GetVerticesCount(),m_SunShadows.GetVertexBuffer(),
			Core::VX_P,m_SunShadows.GetTriCount()*3,m_SunShadows.GetIndexBuffer(),D3DFMT_INDEX16);
	}

	return true;
}

}