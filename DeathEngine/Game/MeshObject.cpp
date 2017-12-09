
#include "MeshObject.h"
#include "../Core/MeshMgr.h"
#include "../Core/Renderer.h"

namespace Game
{

#define MESHUPDATE_SHADOWSTIME			0.1f

#define MESHINFO_BASECHUNK				0
#define MESHINFO_ANIMCHUNK				1
#define MESHINFO_END					-1

struct MeshFileInfo
{
	struct _Base
	{
		char szMeshModel[64];
		char szName[32];
		Core::Vector3 vPosition;
		Core::Vector3 vScale;
		Core::Quaternion qRotation;

		bool bCastShadows;
		bool bReceiveShadows;
		bool bLightPassEnable;

	} Base;

	struct _Anim
	{
		bool bAnim;
		int iAxe;
		float fSpeed;

	} Anim;

};

#ifdef DEEXPORT
MeshObject::MeshObject() : m_bMustRebuildSunShadows(true), m_fTime(0.0f),
	m_fAngle(0.0f), m_bAnim(false), m_fSpeed(0.0f), m_iAxe(0)
#else
MeshObject::MeshObject() : IObject(OBJ_MESH), m_bMustRebuildSunShadows(true), m_fTime(0.0f),
	m_fAngle(0.0f), m_bAnim(false), m_fSpeed(0.0f), m_iAxe(0)
#endif
{
	m_bStatic=true;
	m_bSpecialPassEnable=false;

	SetName("MeshObject");
}

void MeshObject::Clear()
{
	g_MeshMgr.ReleaseMesh(m_hMesh);
	IObject::Clear();

	m_bAnim=false;
	m_fSpeed=0.0f;
	m_iAxe=0;
	m_fAngle=0.0f;
}

void MeshObject::SetMesh(Core::HMesh hMesh)
{
	if(!hMesh.IsNull())
	{
		g_MeshMgr.ReleaseMesh(m_hMesh);
		m_hMesh=hMesh;

		Core::Mesh *pMesh=(Core::Mesh *)g_MeshMgr.GetMesh(m_hMesh);
		if(pMesh)
		{
			m_BSphere.FromBBox(&pMesh->GetBBox());
			m_pColData=(CollisionModel3DImpl *)pMesh->GetCollisionData();
		}
	}
}

bool MeshObject::DrawAmbient()
{
	g_MeshMgr.DrawMesh(m_hMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());
	return true;
}

bool MeshObject::DrawSunLight()
{
	g_MeshMgr.DrawMesh(m_hMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());
	return true;
}

bool MeshObject::Update(const float fTime)
{
	m_fTime+=fTime;
	if(m_fTime >= 0.1f)
	{
		if(!m_bStatic)
			m_bMustRebuildSunShadows=true;

		m_fTime-=0.1f;
	}

	if(m_bAnim && m_fSpeed!=0.0f)
	{
		assert(m_iAxe >= 0 && m_iAxe<3);

		m_fAngle+=m_fSpeed*fTime;

		if(m_fAngle < (2.0f*PI)) m_fAngle+=2.0f*PI;
		else if(m_fAngle > (2.0f*PI)) m_fAngle-=2.0f * PI;

		Core::Vector3 vAxis;
		switch(m_iAxe)
		{
			case 0:
				vAxis=Core::Vector3::XAxisVector;
				break;

			case 1:
				vAxis=Core::Vector3::YAxisVector;
				break;

			case 2:
				vAxis=Core::Vector3::ZAxisVector;
				break;

			default:
				vAxis=Core::Vector3::YAxisVector;
		}

		m_Transformation.SetRotation(Core::Quaternion(vAxis,m_fAngle));
	}

	return true;
}

bool MeshObject::CastSunShadows(const Core::Vector3 &vSunDir)
{
	if(m_bMustRebuildSunShadows)
	{
		Core::EdgeSet *pEdges=g_MeshMgr.GetMeshEdgeSet(m_hMesh);
		if(pEdges)
			m_SunShadows.GenerateFromDirLight(pEdges,vSunDir,m_Transformation.GetInverse());

		m_bMustRebuildSunShadows=false;
	}

	if(m_SunShadows.GetVerticesCount() > 0)
	{
		g_Renderer.SetTransformation(TRANS_MODEL,m_Transformation.GetMatrix(),0,false);
		g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,m_SunShadows.GetVerticesCount(),m_SunShadows.GetVertexBuffer(),
			Core::VX_P,m_SunShadows.GetTriCount()*3,m_SunShadows.GetIndexBuffer(),D3DFMT_INDEX16);
	}

	return true;
}

bool MeshObject::Parse(FILE *pFile)
{
	MeshFileInfo info;
	InitializeMeshInfo(&info);

	int iChunk;

	fread(&iChunk,sizeof(int),1,pFile);
	while(iChunk!=MESHINFO_END && !feof(pFile))
	{
		switch(iChunk)
		{
			case MESHINFO_BASECHUNK:
				fread(&info.Base,sizeof(info.Base),1,pFile);
				break;

			case MESHINFO_ANIMCHUNK:
				fread(&info.Anim,sizeof(info.Anim),1,pFile);
				break;

			default:
				return false;
		}

		fread(&iChunk,sizeof(int),1,pFile);
	}

	m_Transformation.SetPosition(info.Base.vPosition);
	m_Transformation.SetScale(info.Base.vScale);
	m_Transformation.SetRotation(info.Base.qRotation);

	SetName(info.Base.szName);

	m_hMesh=g_MeshMgr.LoadMesh(info.Base.szMeshModel,0);
	Core::Mesh *pMesh=(Core::Mesh *)g_MeshMgr.GetMesh(m_hMesh);

	if(pMesh)
	{
		m_pColData=(CollisionModel3DImpl *)pMesh->GetCollisionData();
		m_BSphere.FromBBox(&pMesh->GetBBox());
	}

	m_Transformation.RebuildMatrix();

	m_bCastShadows=info.Base.bCastShadows;
	m_bReceiveShadows=info.Base.bReceiveShadows;
	m_bLightPassEnable=info.Base.bLightPassEnable;

	m_bAnim=info.Anim.bAnim;
	m_iAxe=info.Anim.iAxe;
	m_fSpeed=info.Anim.fSpeed;

	m_bStatic=!m_bAnim;

	return true;
}

void MeshObject::InitializeMeshInfo(void *pInfo)
{
	MeshFileInfo *info=(MeshFileInfo *)pInfo;
	strcpy(info->Base.szMeshModel,"");
	strcpy(info->Base.szName,"MeshObject");
	info->Base.vPosition=Core::Vector3::NullVector;
	info->Base.vScale=Core::Vector3(1.0f,1.0f,1.0f);
	info->Base.qRotation=Core::Quaternion();
	info->Base.bCastShadows=true;
	info->Base.bReceiveShadows=true;
	info->Base.bLightPassEnable=true;

	info->Anim.bAnim=false;
	info->Anim.iAxe=0;
	info->Anim.fSpeed=0.0f;
}

bool MeshObject::Save(FILE *pFile)
{
	MeshFileInfo info;
	info.Base.bCastShadows=m_bCastShadows;
	info.Base.bReceiveShadows=m_bReceiveShadows;
	info.Base.bLightPassEnable=m_bLightPassEnable;
	strcpy(info.Base.szName,GetName());

	info.Base.vPosition=m_Transformation.GetPosition();
	info.Base.qRotation=m_Transformation.GetRotation();
	info.Base.vScale=m_Transformation.GetScale();

	IMesh *pMesh=g_MeshMgr.GetMesh(m_hMesh);
	if(pMesh)
		strcpy(info.Base.szMeshModel,pMesh->GetFileName());
	else
		strcpy(info.Base.szMeshModel,"NoMesh");

	info.Anim.bAnim=m_bAnim;
	info.Anim.iAxe=m_iAxe;
	info.Anim.fSpeed=m_fSpeed;

	int iChunk=MESHINFO_BASECHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Base,sizeof(info.Base),1,pFile);
	iChunk=MESHINFO_ANIMCHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Anim,sizeof(info.Anim),1,pFile);
	iChunk=MESHINFO_END;
	fwrite(&iChunk,sizeof(int),1,pFile);

	return true;
}

void MeshObject::OnEvent(const GameEvent& Event)
{
}

void MeshObject::SetAnimation(const int iAxe, const float fSpeed)
{
	m_bAnim=true;
	m_bStatic=false;
	m_iAxe=iAxe;
	m_fSpeed=fSpeed;
	m_fAngle=0.0f;
}

}
