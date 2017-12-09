
#include "Map.h"
#include "TreeGenerator.h"
#include "ObjectMap.h"
#include "Octree.h"
#include "MapObjectFactory.h"
#include "MapObjectDefs.h"
#include "RenderingQueue.h"
#include "../Core/TextureMgr.h"
#include "../Core/Renderer.h"
#include "../Core/Material.h"
#include "../Core/FileSystem.h"
#include "../Dirs.h"
#include <string.h>
#include <d3dx9.h>


namespace Game
{

#define MAPINFO_END				-1
#define MAPINFO_BASECHUNK		0
#define MAPINFO_SKYBOXCHUNK		1
#define MAPINFO_WATERCHUNK		2
#define MAPINFO_SHIELDCHUNK		3
#define MAPINFO_SUNCHUNK		4
#define MAPINFO_WEATHERCHUNK	5

struct MapFileInfo
{
	typedef char TextureName[64];

	struct _Base
	{
		char szName[64];
		TextureName szDetailTex;
		int iObjectCount;
	} Base;

	struct _SkyBox
	{
		TextureName szTexture[5];
	} SkyBox;

	struct _Water
	{
		bool bEnable;
		bool bSurround;
		float fHeight;
		float fWaveHeight;
		float fWaveSpeed;
		TextureName szTexture;
		char szShader[64];
	} Water;

	struct _Shield
	{
		Core::Vector3 vSize;
		TextureName szTexture;
		uint32 dwVisDist;
		float fSpeed;

	} Shield;

	struct _Sun
	{
		TextureName szTexture;
		Core::Vector3 vDirection;
		Core::Color32f cColor;
		Core::Color32f cAmbient;

	} Sun;

	struct _Weather
	{
		int iWeatherType;
		int iParticleDensity;
		Core::Color32 cParticleColor;
		TextureName szParticleTexture;
		Core::Vector3 vWindDir;
		float fWindForce;
		float fParticleSpeed;
		float fParticleSize;

	} Weather;

};

Map::Map() : m_bWaterEnable(false), m_pSun(0)
{
	m_pHeightmap=new Heightmap;
	m_pPatchGrid=new PatchGrid;
	m_pSkyBox=new SkyBox;
	m_pWater=new Water;
	m_pShield=new Shield;
	m_pSun=new Sun;

	new TreeGenerator;
	new ObjectMap;
	new Octree;
	new RenderingQueue;
}

Map::~Map()
{
	if(m_pHeightmap) delete m_pHeightmap;
	if(m_pPatchGrid) delete m_pPatchGrid;
	if(m_pSkyBox) delete m_pSkyBox;
	if(m_pWater) delete m_pWater;
	if(m_pShield) delete m_pShield;
	if(m_pSun) delete m_pSun;
}

void Map::Release()
{
	m_pHeightmap->Clear();
	m_pPatchGrid->Clear();
	m_pSkyBox->Clear();
	m_pWater->Clear();
	m_pShield->Clear();
	m_pSun->Clear();
	g_ObjectMap.Clear();

	strcpy(m_szName,"DefaultMap");
	m_bWaterEnable=false;
}

bool Map::Load(const char *szFileName, const char *szPath)
{
	Release();

	char szTempPath[1024];
	if(szPath)
		strcpy(szTempPath,szPath);
	else
		strcpy(szTempPath,"Maps\\");

	char szBaseFileName[1024];
	sprintf(szBaseFileName,"%s%s",szTempPath,szFileName);

	char szTemp[1024];
	sprintf(szTemp,"%s.dmap",szBaseFileName);

	std::vector<IObject *> objects;

	FILE *pFile=fopen(szTemp,"rb");
	if(!pFile) return false;

	MapFileInfo info;
	InitializeMapInfo(&info);

	int iChunk;
	fread(&iChunk,sizeof(int),1,pFile);

	while(iChunk!=MAPINFO_END && !feof(pFile))
	{
		switch(iChunk)
		{
			case MAPINFO_BASECHUNK:
				fread(&info.Base,sizeof(info.Base),1,pFile);
				break;

			case MAPINFO_SHIELDCHUNK:
				fread(&info.Shield,sizeof(info.Shield),1,pFile);
				break;

			case MAPINFO_SKYBOXCHUNK:
				fread(&info.SkyBox,sizeof(info.SkyBox),1,pFile);
				break;

			case MAPINFO_SUNCHUNK:
				fread(&info.Sun,sizeof(info.Sun),1,pFile);
				break;

			case MAPINFO_WATERCHUNK:
				fread(&info.Water,sizeof(info.Water),1,pFile);
				break;

			case MAPINFO_WEATHERCHUNK:
				fread(&info.Weather,sizeof(info.Weather),1,pFile);
				break;

			default:
				fclose(pFile);
				return false;
		}

		fread(&iChunk,sizeof(int),1,pFile);
	}

	//				Objects
	for(int i=0;i<info.Base.iObjectCount;i++)
	{
		ObjectType type;
		fread(&type,sizeof(type),1,pFile);
		
		IObject *pObj=g_MapObjectFactory.CreateObject(type);
		if(pObj && pObj->Parse(pFile))
			objects.push_back(pObj);
	}

	fclose(pFile);

	sprintf(szTemp,"%s.hgt",szBaseFileName);
	if(!m_pHeightmap->Load(szTemp,""))
	{
		g_Output->AddMsg("x Erreur [%s]: Impossible de charger la heightmap.\n",szTemp);
		return false;
	}

	m_pHeightmap->BuildNormalMap();

	sprintf(szTemp,"%s.jpg",szFileName);
	Core::HTexture hTex=g_TextureMgr.LoadTexture(szTemp,szTempPath);
	if(hTex.IsNull())
		g_Output->AddMsg("x Erreur [%s]: La texture n'existe pas ou est corrompue.\n",szTemp);
	else
		m_pPatchGrid->SetTerrainTexture(hTex);

	//				Général
	strcpy(m_szName,info.Base.szName);
	if(strlen(info.Base.szDetailTex) > 0)
	{
		Core::HTexture hDetail=g_TextureMgr.LoadTexture(info.Base.szDetailTex,szTempPath);
		if(hDetail.IsNull())
			g_Output->AddMsg("x Erreur [%s]: La texture %s n'existe pas ou est corrompue.\n",szTemp,info.Base.szDetailTex);
		else
			m_pPatchGrid->SetDetailTexture(hDetail);
	}

	//				SkyBox
	for(int i=0;i<SKY_COUNT;i++)
	{
		if(strlen(info.SkyBox.szTexture[i]) > 0)
		{
			Core::HTexture hTex=g_TextureMgr.LoadTexture(info.SkyBox.szTexture[i],DIR_SKYBOXTEXTURES);
			if(hTex.IsNull())
				g_Output->AddMsg("x Erreur [%s]: La texture %s n'existe pas ou est corrompue.\n",szTemp,info.SkyBox.szTexture[i]);
			else
				m_pSkyBox->SetTexture((SkyFace)i,hTex);
		}
	}

	//				Water
	if(info.Water.bEnable)
	{
		Core::HTexture hTex=g_TextureMgr.LoadTexture(info.Water.szTexture,"");
		if(hTex.IsNull())
			g_Output->AddMsg("x Erreur [%s]: La texture d'eau %s n'existe pas ou est corrompue.\n",szTemp,info.Water.szTexture);
		else
			m_pWater->SetWaterTexture(hTex);
	}

	m_pPatchGrid->Create(m_pHeightmap,16);
	m_pPatchGrid->Generate();
	m_pPatchGrid->Recalculate(0,0,2048);
	m_pPatchGrid->Simulate(0.4f);

	m_pSkyBox->Create(g_Renderer.GetDevice(),(float)m_pHeightmap->GetSize() * 2.0f);

	if(info.Water.bEnable)
	{
		if(m_pWater->Generate(m_pHeightmap,8,info.Water.fHeight,info.Water.bSurround,false))
			m_bWaterEnable=true;
	}

	m_pShield->Create(info.Shield.vSize,m_pHeightmap->GetSize(),
		info.Shield.dwVisDist,g_TextureMgr.LoadTexture(info.Shield.szTexture,""));

	Core::HTexture hSunTex;
	if(strlen(info.Sun.szTexture) > 0)
		hSunTex=g_TextureMgr.LoadTexture(info.Sun.szTexture,szTempPath);

	m_pSun->Generate(hSunTex,info.Sun.cColor,info.Sun.cAmbient,info.Sun.vDirection);

	g_Octree.Generate(m_pHeightmap->GetSize(),(int)info.Shield.vSize.y,3);
	g_ObjectMap.Generate(m_pHeightmap,8,info.Water.fHeight);

	std::vector<IObject *>::iterator obj;
	for(obj=objects.begin();obj!=objects.end();obj++)
	{
		if((*obj)->GetObjectType()==OBJ_MAP)
			g_ObjectMap.AddObject((MapObject *)*obj);
		else
			g_Octree.InsertObject(*obj);
	}

	return true;
}

void Map::InitializeMapInfo(void *pInfo)
{
	MapFileInfo *info=(MapFileInfo *)pInfo;

	strcpy(info->Base.szDetailTex,"");
	strcpy(info->Base.szName,"");
	info->Base.iObjectCount=0;

	info->Shield.dwVisDist=128;
	info->Shield.fSpeed=1.0f;
	info->Shield.vSize=Core::Vector3(128.0f,128.0f,128.0f);
	strcpy(info->Shield.szTexture,"");

	for(int i=0;i<5;i++)
		strcpy(info->SkyBox.szTexture[i],"");

	info->Sun.cAmbient=Core::Color32f(1.0f,0.2f,0.2f,0.2f);
	info->Sun.cColor=Core::Color32f(1.0f,0.8f,0.7f,0.7f);
	info->Sun.vDirection=Core::Vector3(0.0f,-1.0f,0.0f);
	strcpy(info->Sun.szTexture,"");

	info->Water.bEnable=false;
	info->Water.bSurround=false;
	info->Water.fHeight=0.0f;
	info->Water.fWaveHeight=0.0f;
	info->Water.fWaveSpeed=1.0f;
	strcpy(info->Water.szShader,"");
	strcpy(info->Water.szTexture,"");

	info->Weather.cParticleColor=Core::Color32(255,255,255,255);
	info->Weather.fParticleSize=1.0f;
	info->Weather.fParticleSpeed=1.0f;
	info->Weather.fWindForce=0.0f;
	info->Weather.iParticleDensity=128;
	info->Weather.iWeatherType=0;
	info->Weather.vWindDir=Core::Vector3::XAxisVector;
	strcpy(info->Weather.szParticleTexture,"");
}

bool Map::Save()
{
	char szTemp[1024];
	sprintf(szTemp,"Maps\\%s.dmap",m_szName);

	FILE *pFile=fopen(szTemp,"wb");
	if(!pFile) return false;

	MapFileInfo info;
	InitializeMapInfo(&info);

	strcpy(info.Base.szName,m_szName);

	g_TextureMgr.GetTextureName(m_pPatchGrid->GetDetailTexture(),info.Base.szDetailTex);

	for(int i=0;i<SKY_COUNT;i++)
		g_TextureMgr.GetTextureName(m_pSkyBox->GetTexture((SkyFace)i),info.SkyBox.szTexture[i]);

	info.Water.bEnable=m_bWaterEnable;
	info.Water.bSurround=m_pWater->IsSurroundEnable();
	info.Water.fHeight=m_pWater->GetHeight();
	g_TextureMgr.GetTextureName(m_pWater->GetWaterTexture(),info.Water.szTexture);
	strcpy(info.Water.szShader,"");

	info.Shield.vSize=m_pShield->GetSize();
	info.Shield.dwVisDist=m_pShield->GetVisibilityDistance();
	g_TextureMgr.GetTextureName(m_pShield->GetTexture(),info.Shield.szTexture);

	info.Sun.cAmbient=m_pSun->GetAmbientLight();
	info.Sun.cColor=m_pSun->GetColor();
	info.Sun.vDirection=m_pSun->GetDirection();
	g_TextureMgr.GetTextureName(m_pSun->GetTexture(),info.Sun.szTexture);

	std::vector<IObject *> objects;
	g_Octree.CollectObjects(&objects);

	info.Base.iObjectCount=objects.size();

	int iChunk=MAPINFO_BASECHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Base,sizeof(info.Base),1,pFile);

	iChunk=MAPINFO_SHIELDCHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Shield,sizeof(info.Shield),1,pFile);

	iChunk=MAPINFO_SKYBOXCHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.SkyBox,sizeof(info.SkyBox),1,pFile);

	iChunk=MAPINFO_SUNCHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Sun,sizeof(info.Sun),1,pFile);

	iChunk=MAPINFO_WATERCHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Water,sizeof(info.Water),1,pFile);

	iChunk=MAPINFO_WEATHERCHUNK;
	fwrite(&iChunk,sizeof(int),1,pFile);
	fwrite(&info.Weather,sizeof(info.Weather),1,pFile);

	iChunk=MAPINFO_END;
	fwrite(&iChunk,sizeof(int),1,pFile);

	if(objects.size() > 0)
	{
		std::vector<IObject *>::iterator pObj;
		for(pObj=objects.begin();pObj!=objects.end();pObj++)
		{
			ObjectType type=(*pObj)->GetObjectType();
			fwrite(&type,sizeof(type),1,pFile);
			(*pObj)->Save(pFile);
		}
	}

	fclose(pFile);

	sprintf(szTemp,"Maps\\%s.hgt",m_szName);
	m_pHeightmap->Save(szTemp);

	IDirect3DTexture9 *pTex=g_TextureMgr.GetTexture(m_pPatchGrid->GetTerrainTexture());
	if(pTex)
	{
		sprintf(szTemp,"Maps\\%s.jpg",m_szName);
		D3DXSaveTextureToFile(szTemp,D3DXIFF_JPG,pTex,0);
	}

	return true;
}

bool Map::CreateEmpty(const char *szName, int iSize)
{
	m_pHeightmap->Clear();
	m_pPatchGrid->Clear();
	m_pSkyBox->Clear();
	m_pWater->Clear();
	m_pShield->Clear();
	m_pSun->Clear();
	g_ObjectMap.Clear();

	if(!m_pHeightmap->CreateFlat(iSize))
		return false;

	m_pHeightmap->BuildNormalMap();

	m_pSkyBox->Create(g_Renderer.GetDevice(),(float)m_pHeightmap->GetSize() * 2.0f);
	m_pPatchGrid->Create(m_pHeightmap,16);
	m_pPatchGrid->Generate();
	m_pPatchGrid->Recalculate(0,0,2048);
	m_pPatchGrid->Simulate(0.4f);

	const Core::Vector3 vShieldSize((float)iSize/2,(float)iSize/2,(float)iSize/2);
	m_pShield->Create(vShieldSize,iSize,iSize/2,Core::HTexture());
	m_pShield->EnableShield(false);

	g_Octree.Generate(iSize,iSize/2,3);

	strcpy(m_szName,szName);
	m_bWaterEnable=false;

	return true;
}

void Map::Visibility(const IViewFrustrum *pFrustrum)
{
	m_pSkyBox->Draw(pFrustrum->GetPosition());
	m_pSun->Draw();

	//			Visibility

	m_pPatchGrid->Visibility(pFrustrum);
	if(m_bWaterEnable)
		m_pWater->Visibility(pFrustrum);
	
	g_Octree.Visibility(pFrustrum);
	m_pShield->SetCameraPosition(pFrustrum->GetPosition());
	
	g_RenderingQueue.Register(m_pShield);

	m_pSun->Set();
}

void Map::Draw(const IViewFrustrum *pFrustrum)
{
	m_pPatchGrid->Simulate(0.4f);

	m_pSun->Set();

	m_pSkyBox->Draw(pFrustrum->GetPosition());
	m_pSun->Draw();

	//			Visibility

	m_pPatchGrid->Visibility(pFrustrum);
	if(m_bWaterEnable) m_pWater->Visibility(pFrustrum);
	g_Octree.Visibility(pFrustrum);

	m_pShield->SetCameraPosition(pFrustrum->GetPosition());
	g_RenderingQueue.Register(m_pShield);

	g_RenderingQueue.Execute(m_pSun->GetDirection());
}

void Map::Simulate(const float fTime)
{
	m_pPatchGrid->Simulate(fTime);
	g_Octree.Update(fTime);
}

#ifdef DEEXPORT
IObjectMap *Map::GetObjectMap()
{
	return ObjectMap::GetSingletonPtr();
}

IMapObjectFactory *Map::GetObjectFactory()
{
	return MapObjectFactory::GetSingletonPtr();
}

ITreeGenerator *Map::GetTreeGenerator()
{
	return TreeGenerator::GetSingletonPtr();
}

void Map::AddObject(IObject *pObj)
{
	if(pObj && pObj->GetObjectType()==OBJ_MESH)
		g_Octree.InsertObject(pObj);
}

void Map::RemoveObject(IObject *pObj)
{
	if(pObj && pObj->GetObjectType()==OBJ_MESH)
		(pObj)->RemoveFromOctree();
}

void Map::UpdateObject(IObject *pObj)
{
	if(pObj && pObj->GetObjectType()==OBJ_MESH)
		(pObj)->UpdatePositionInOctree();
}

std::vector<IObject *> *Map::CollectObjects()
{
	static std::vector<IObject*> obj;
	obj.clear();

	obj.push_back(m_pPatchGrid);
	obj.push_back(m_pShield);

	if(m_bWaterEnable)
		obj.push_back(m_pWater);

	g_Octree.CollectObjects(&obj);

	return &obj;
}

#endif

std::vector<IObject*> * Map::Pick(const Core::Vector3 &vOrigin, const Core::Vector3 &vDir)
{
	static std::vector<IObject *> Selection;
	Selection.clear();
	g_Octree.Pick(vOrigin,vDir,&Selection);

	return &Selection;
}

void Map::RebuildAllShadowVolumes()
{
	g_Octree.RebuildAllShadowVolumes();
}

}