
#include "SpaceShip.h"
#include "../Core/MeshMgr.h"
#include "../Core/MaterialMgr.h"
#include "../Core/Renderer.h"
#include "../Core/FileSystem.h"
#include "../Dirs.h"
#include "Map.h"
#include "ExplosionFactory.h"
#include "Explosion.h"

enum
{
	SS_FLYING,
	SS_EXPLODING

};

namespace Game
{

#define SHIPINFO_END				-1
#define SHIPINFO_BASE				0
#define SHIPINFO_GEO				1

struct ShipInfo
{
	struct _Base
	{
		char szName[64];
		float fLinearSpeed;
		float fRotSpeed;
		int iLODCount;
		int iMainFirePointCount;
		int iSecFirePointCount;
		int iBackFirePointCount;
		int iDefensePointCount;
		int iBombDropPointCount;

	} Base;

};


#ifdef DEEXPORT
SpaceShip::SpaceShip() :
#else
SpaceShip::SpaceShip() : IObject(OBJ_SPACESHIP),
#endif
	m_fRotSpeed(90.0f * PI / 180.0f),m_fSpeed(20.0f), m_pController(0),
	m_vDirection(Core::Vector3::ZAxisVector),
	m_vLocalXAxis(Core::Vector3::XAxisVector), m_pCamera(0),
	m_iState(SS_FLYING),m_fTime(0.0f),
	m_iLastMainFirePoint(-1),
	m_iLastSecFirePoint(-1),
	m_iLastBackFirePoint(-1),
	m_iLastDefensePoint(-1),
	m_iLastBombDropPoint(-1),
	m_pExplosion(0)
{
	m_Transformation.SetRotation(Core::Quaternion(m_vDirection,0.0f));

	m_bStatic=false;
	m_bLightPassEnable=true;
	m_bSpecialPassEnable=false;
	m_bCastShadows=true;
	m_bReceiveShadows=true;
	m_bVisible=true;

	SetName("SpaceShip");
}

SpaceShip::~SpaceShip()
{
	g_MeshMgr.ReleaseMesh(m_hMesh);
	if(m_pExplosion)
		m_pExplosion->Delete();
}

void SpaceShip::Clear()
{
	g_MeshMgr.ReleaseMesh(m_hMesh);
	m_hMesh=Core::HMesh();

	m_fRotSpeed=90.0f * PI / 180.0f;
	m_fSpeed=20.0f;
	
	if(m_pController) delete m_pController;
	m_pController=0;

	m_vDirection=Core::Vector3::ZAxisVector;
	m_vLocalXAxis=Core::Vector3::XAxisVector;
	m_pCamera=0;
	m_Transformation.SetPosition(Core::Vector3::NullVector);
	m_Transformation.SetRotation(Core::Quaternion(m_vDirection,0.0f));
	m_iState=SS_FLYING;

	m_MainFirePoints.clear();
	m_SecFirePoints.clear();
	m_BackFirePoints.clear();
	m_DefensePoints.clear();
	m_BombDropPoints.clear();

	m_iLastMainFirePoint=-1;
	m_iLastSecFirePoint=-1;
	m_iLastBackFirePoint=-1;
	m_iLastDefensePoint=-1;
	m_iLastBombDropPoint=-1;

	if(m_pExplosion)
		m_pExplosion->Delete();

	m_pExplosion=0;

	IObject::Clear();
}


bool SpaceShip::Load(const char *szFileName, const char *szPath)
{
	if(!szFileName) return false;

	char szName[64];
	char szPathName[512];

	if(szPath)
	{
		strcpy(szName,szFileName);
		strcpy(szPathName,szPath);
	}
	else
	{
		if(!g_FileSystem.ExtractPath(szFileName,szPathName))
		{
			strcpy(szPathName,DIR_SPACESHIPS);
			strcpy(szName,szFileName);
		}
		else
			g_FileSystem.ExtractFileName(szFileName,szName);
	}

	g_FileSystem.ChangeWorkingDirectory(szPathName);

	Clear();

	FILE *pFile=fopen(szName,"rb");
	if(!pFile) return false;

	ShipInfo info;
	InitializeShipInfo(&info);

	int iChunk;
	bool bBaseLoaded=false;
	Core::Mesh *pMesh=0;
	Core::Vector3 vPoint;

	fread(&iChunk,sizeof(int),1,pFile);
	while(iChunk!=SHIPINFO_END && !feof(pFile))
	{
		switch(iChunk)
		{
			case SHIPINFO_BASE:
				fread(&info.Base,sizeof(info.Base),1,pFile);
				bBaseLoaded=true;

				for(int i=0;i<info.Base.iMainFirePointCount;i++)
				{
					fread(&vPoint,sizeof(Core::Vector3),1,pFile);
					AddMainFirePoint(vPoint);
				}

				for(int i=0;i<info.Base.iSecFirePointCount;i++)
				{
					fread(&vPoint,sizeof(Core::Vector3),1,pFile);
					AddSecFirePoint(vPoint);
				}

				for(int i=0;i<info.Base.iBackFirePointCount;i++)
				{
					fread(&vPoint,sizeof(Core::Vector3),1,pFile);
					AddBackFirePoint(vPoint);
				}

				for(int i=0;i<info.Base.iDefensePointCount;i++)
				{
					fread(&vPoint,sizeof(Core::Vector3),1,pFile);
					AddDefensePoint(vPoint);
				}

				for(int i=0;i<info.Base.iBombDropPointCount;i++)
				{
					fread(&vPoint,sizeof(Core::Vector3),1,pFile);
					AddBombDropPoint(vPoint);
				}
				break;

			case SHIPINFO_GEO:
				if(!bBaseLoaded)
				{
					fclose(pFile);
					g_FileSystem.ResetWorkingDirectory();
					return false;
				}
				else
				{
					m_hMesh=g_MeshMgr.CreateEmptyMesh(info.Base.szName);
					pMesh=(Core::Mesh *)g_MeshMgr.GetMesh(m_hMesh);
					if(!pMesh)
					{
						g_FileSystem.ResetWorkingDirectory();
						fclose(pFile);
						return false;
					}

					if(!pMesh->LoadFromFile(pFile))
					{
						g_FileSystem.ResetWorkingDirectory();
						fclose(pFile);
						return false;
					}

					for(int i=1;i<info.Base.iLODCount;i++)
					{
						Core::Mesh *pLOD=(Core::Mesh *)pMesh->CreateLOD(0);
						pLOD->LoadFromFile(pFile);
					}
				}
				break;

			default:
				g_FileSystem.ResetWorkingDirectory();
				fclose(pFile);
				return false;
		}

		fread(&iChunk,sizeof(int),1,pFile);
	}

	fclose(pFile);
	g_FileSystem.ResetWorkingDirectory();

	m_fSpeed=info.Base.fLinearSpeed;
	m_fRotSpeed=info.Base.fRotSpeed;
	SetName(info.Base.szName);

	if(pMesh)
	{
		m_BSphere.FromBBox(&pMesh->GetBBox());
		m_pColData=(CollisionModel3DImpl *)pMesh->GetCollisionData();
		return true;
	}
	else
		return false;
}

void SpaceShip::InitializeShipInfo(void *pInfo)
{
	ShipInfo *info=(ShipInfo*)pInfo;
	
	strcpy(info->Base.szName,"Ship");
	info->Base.fLinearSpeed=20.0f;
	info->Base.fRotSpeed=PI * 0.5f;
	info->Base.iBackFirePointCount=0;
	info->Base.iBombDropPointCount=0;
	info->Base.iDefensePointCount=0;
	info->Base.iMainFirePointCount=0;
	info->Base.iSecFirePointCount=0;
	info->Base.iLODCount=0;
}

bool SpaceShip::Save(const char *szFileName, const char *szPath)
{
	if(m_hMesh.IsNull() || !szFileName) return false;

	ShipInfo info;
	Core::Mesh *pMesh=(Core::Mesh *)g_MeshMgr.GetMesh(m_hMesh);

	if(!pMesh) return false;

	char szTemp[1024];
	g_FileSystem.ChangeWorkingDirectory(szPath ? szPath : DIR_SPACESHIPS);

	if(strlen(szFileName)!=0)
	{
		g_FileSystem.CreateDirectory_(szFileName);
		g_FileSystem.ChangeWorkingDirectory(szFileName);
	}

	sprintf(szTemp,"%s.dship",szFileName);

	FILE *pFile=fopen(szTemp,"wb");
	if(!pFile)
	{
		g_Output->AddMsg("x Erreur [%s]: Impossible de creer le fichier pour la sauvegarde.\n");
		return false;
	}

	info.Base.fLinearSpeed=m_fSpeed;
	info.Base.fRotSpeed=m_fRotSpeed;
	strcpy(info.Base.szName,GetName());
	info.Base.iBackFirePointCount=m_BackFirePoints.size();
	info.Base.iBombDropPointCount=m_BombDropPoints.size();
	info.Base.iDefensePointCount=m_DefensePoints.size();
	info.Base.iSecFirePointCount=m_SecFirePoints.size();
	info.Base.iMainFirePointCount=m_MainFirePoints.size();
	info.Base.iLODCount=pMesh->GetLODCount();

	int iChunk=SHIPINFO_BASE;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Base,sizeof(info.Base),1,pFile);

	for(int i=0;i<info.Base.iMainFirePointCount;i++)
	{
		Core::Vector3 vPoint(GetMainFirePoint(i));
		fwrite(&vPoint,sizeof(Core::Vector3),1,pFile);
	}

	for(int i=0;i<info.Base.iSecFirePointCount;i++)
	{
		Core::Vector3 vPoint(GetSecFirePoint(i));
		fwrite(&vPoint,sizeof(Core::Vector3),1,pFile);
	}

	for(int i=0;i<info.Base.iBackFirePointCount;i++)
	{
		Core::Vector3 vPoint(GetBackFirePoint(i));
		fwrite(&vPoint,sizeof(Core::Vector3),1,pFile);
	}

	for(int i=0;i<info.Base.iDefensePointCount;i++)
	{
		Core::Vector3 vPoint(GetDefensePoint(i));
		fwrite(&vPoint,sizeof(Core::Vector3),1,pFile);
	}

	for(int i=0;i<info.Base.iBombDropPointCount;i++)
	{
		Core::Vector3 vPoint(GetBombDropPoint(i));
		fwrite(&vPoint,sizeof(Core::Vector3),1,pFile);
	}

	iChunk=SHIPINFO_GEO;
	fwrite(&iChunk,sizeof(int),1,pFile);
	for(int i=0;i<info.Base.iLODCount;i++)
		((Core::Mesh *)pMesh->GetLODByIndex(i))->SaveToFile(pFile);

	iChunk=SHIPINFO_END;
	fwrite(&iChunk,sizeof(int),1,pFile);

	fclose(pFile);
	g_FileSystem.ResetWorkingDirectory();

	return true;
}

bool SpaceShip::Update(const float fTime)
{
	m_bHasMoved=false;

	if(m_iState==SS_FLYING)
	{
		if(m_pController)
		{
			Core::Vector3 vRot(m_pController->GetRotation());
			vRot*=m_fRotSpeed*fTime;

			Core::Quaternion z_rotate(m_vDirection,vRot.z);
			Core::Vector3 vLocalXAxis(z_rotate*m_vLocalXAxis);
			m_vLocalXAxis.Normalize();
			Core::Quaternion x_rotate(vLocalXAxis,vRot.x);

			Core::Quaternion qRot(z_rotate*x_rotate);
			Core::Vector3 vTranslation((x_rotate * m_vDirection) * (m_fSpeed*fTime));

			m_Transformation.Rotate(qRot);
			m_Transformation.GetRotation().GetMatrix(m_mRotation);
			m_Transformation.Translate(vTranslation);

			m_fTime+=fTime;
			if(m_fTime >= 0.1f)
			{
				m_pCamera->AddTrajectoryPoint(m_Transformation.GetPosition(),m_Transformation.GetRotation());
				m_fTime-=0.1f;
			}

			if(m_pCamera)
				m_pCamera->UpdateFollowShip(fTime,m_Transformation.GetPosition(),m_Transformation.GetRotation());

			m_vDirection=Core::Vector3::ZAxisVector * m_mRotation;
			m_vLocalXAxis=Core::Vector3::XAxisVector * m_mRotation;

			m_Transformation.RebuildMatrix();
			m_bHasMoved=true;
			UpdatePositionInOctree();

			return true;
		}
		else
			return false;
	}
	else if(m_iState==SS_EXPLODING)
	{
		if(!m_pExplosion || m_pExplosion->IsFinished())
		{
			if(m_pExplosion)
			{
				m_pExplosion->Delete();
				m_pExplosion=0;
			}

			m_bSpecialPassEnable=false;

			int iMiddle=g_Map.GetHeightmap()->GetSize() / 2;
			m_Transformation.SetPosition(Core::Vector3(
				(float)iMiddle,
				g_Map.GetHeightmap()->GetHeight(iMiddle,iMiddle)+10.0f,
				(float)iMiddle));
			m_Transformation.SetRotation(Core::Quaternion());
			m_mRotation.CreateIdentity();

			if(m_pCamera)
				m_pCamera->ActiveFollowShip();

			m_iState=SS_FLYING;
		}
		else
		{
			if(m_pCamera)
				m_pCamera->UpdateTurnAround(fTime);

			m_pExplosion->Update(fTime);
		}
	}

	return true;
}

void SpaceShip::Collision(const bool bOnWater)
{
	m_iState=SS_EXPLODING;

	if(!m_pExplosion)
	{
		m_pExplosion=g_ExplosionFactory.CreateExplosion("DefaultExplosion",m_Transformation.GetPosition(),10.0f,true,true,bOnWater);

		if(m_pCamera)
			m_pCamera->ActiveTurnAround(m_Transformation.GetPosition(),10.0f,20.0f,60.0f);

		m_bSpecialPassEnable=true;
	}
}

bool SpaceShip::DrawAmbient()
{
	if(m_iState==SS_FLYING)
		g_MeshMgr.DrawMesh(m_hMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());

	return true;
}

bool SpaceShip::DrawSunLight()
{
	if(m_iState==SS_FLYING)
		g_MeshMgr.DrawMesh(m_hMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());

	return true;
}

bool SpaceShip::DrawSpecial()
{
	if(m_iState==SS_EXPLODING && m_pExplosion)
		m_pExplosion->Draw(m_pCamera);

	return true;
}

bool SpaceShip::CastSunShadows(const Core::Vector3& vSunDir)
{
/*	Core::EdgeSet *pEdges=g_MeshMgr.GetMeshEdgeSet(m_hMesh);
	if(pEdges)
		m_SunShadows.GenerateFromDirLight(pEdges,vSunDir,m_Transformation.GetInverse());

	if(m_SunShadows.GetVerticesCount() > 0)
	{
		g_Renderer.SetTransformation(TRANS_MODEL,m_Transformation.GetMatrix(),0,false);
		g_Renderer.SetVertexBuffer(m_SunShadows.GetVertexBuffer(),m_SunShadows.GetVerticesCount(),Core::VertexSizeTable[Core::VX_P]);
		g_Renderer.SetIndexBuffer(m_SunShadows.GetIndexBuffer());
		g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_SunShadows.GetTriCount());
	}*/

	return true;
}

void SpaceShip::OnEvent(const GameEvent& e)
{
	if(e.GetType()==GE_COLLIDE)
		Collision();
}

void SpaceShip::AddMainFirePoint(const Core::Vector3 &vFire)
{
	Core::Matrix4 mPoint;
	mPoint.CreateTranslate(vFire.x,vFire.y,vFire.z);
	m_MainFirePoints.push_back(mPoint);
}

void SpaceShip::AddSecFirePoint(const Core::Vector3 &vFire)
{
	Core::Matrix4 mPoint;
	mPoint.CreateTranslate(vFire.x,vFire.y,vFire.z);
	m_SecFirePoints.push_back(mPoint);
}

void SpaceShip::AddBackFirePoint(const Core::Vector3 &vFire)
{
	Core::Matrix4 mPoint;
	mPoint.CreateTranslate(vFire.x,vFire.y,vFire.z);
	m_BackFirePoints.push_back(mPoint);
}

void SpaceShip::AddDefensePoint(const Core::Vector3 &vDefense)
{
	Core::Matrix4 mPoint;
	mPoint.CreateTranslate(vDefense.x,vDefense.y,vDefense.z);
	m_DefensePoints.push_back(mPoint);
}

void SpaceShip::AddBombDropPoint(const Core::Vector3& vDrop)
{
	Core::Matrix4 mPoint;
	mPoint.CreateTranslate(vDrop.x,vDrop.y,vDrop.z);
	m_BombDropPoints.push_back(mPoint);
}

Core::Vector3 SpaceShip::GetMainFirePoint(const int iIndex)
{
	if(iIndex >= 0 && iIndex < m_MainFirePoints.size())
		return Core::Vector3(m_MainFirePoints[iIndex].m[3][0],m_MainFirePoints[iIndex].m[3][1],m_MainFirePoints[iIndex].m[3][2]);
	else
		return Core::Vector3::NullVector;
}

Core::Vector3 SpaceShip::GetSecFirePoint(const int iIndex)
{
	if(iIndex >= 0 && iIndex < m_SecFirePoints.size())
		return Core::Vector3(m_SecFirePoints[iIndex].m[3][0],m_SecFirePoints[iIndex].m[3][1],m_SecFirePoints[iIndex].m[3][2]);
	else
		return Core::Vector3::NullVector;
}

Core::Vector3 SpaceShip::GetBackFirePoint(const int iIndex)
{
	if(iIndex >= 0 && iIndex < m_BackFirePoints.size())
		return Core::Vector3(m_BackFirePoints[iIndex].m[3][0],m_BackFirePoints[iIndex].m[3][1],m_BackFirePoints[iIndex].m[3][2]);
	else
		return Core::Vector3::NullVector;
}

Core::Vector3 SpaceShip::GetDefensePoint(const int iIndex)
{
	if(iIndex >= 0 && iIndex < m_DefensePoints.size())
		return Core::Vector3(m_DefensePoints[iIndex].m[3][0],m_DefensePoints[iIndex].m[3][1],m_DefensePoints[iIndex].m[3][2]);
	else
		return Core::Vector3::NullVector;
}

Core::Vector3 SpaceShip::GetBombDropPoint(const int iIndex)
{
	if(iIndex >= 0 && iIndex < m_BombDropPoints.size())
		return Core::Vector3(m_BombDropPoints[iIndex].m[3][0],m_BombDropPoints[iIndex].m[3][1],m_BombDropPoints[iIndex].m[3][2]);
	else
		return Core::Vector3::NullVector;
}

void SpaceShip::SetMesh(Core::HMesh hMesh)
{
	if(!m_hMesh.IsNull())
		g_MeshMgr.ReleaseMesh(m_hMesh);

	m_hMesh=hMesh;
	Core::Mesh *pMesh=(Core::Mesh *)g_MeshMgr.GetMesh(m_hMesh);

	if(pMesh)
	{
		m_BSphere.FromBBox(&pMesh->GetBBox());
		m_pColData=(CollisionModel3DImpl *)pMesh->GetCollisionData();
	}
	else
		m_pColData=0;

}

}