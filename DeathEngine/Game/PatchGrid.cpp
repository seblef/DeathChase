
#include "PatchGrid.h"
#include "HeightmapVertexBuffer.h"
#include "../Core/Renderer.h"
#include "Patch.h"
#include "RenderingQueue.h"
#include "../Core/ViewFrustrum.h"
#include "../Core/TextureMgr.h"
#include "../Core/ShaderMgr.h"
#include "../Core/Config.h"

namespace Game
{

const char *szPatchShaderNames[]=
{
	"StdHeightmapNoTex",
	"StdHeightmapAmbientTex",
	"StdHeightmapAmbientTexDetail",
	"StdHeightmapSunTex",
	"StdHeightmapSunTexDetail"

};

#ifdef DEEXPORT
PatchGrid::PatchGrid() :
#else
PatchGrid::PatchGrid() : IObject(OBJ_PATCHGRID),
#endif
	m_pMap(0), m_vLastPos(-1.0f,-2.0f,-3.0f),m_iPatchSize(4),
	m_fTime(0.0f),m_iDrawnPatches(0),m_ppPatches(0),
	m_iWidth(0)
{
	m_fVarianceStart=1.0f;
	m_fVarianceRamp=g_Config.GetFParameter("VarianceRamp",0.75f);

	m_bCastShadows=false;
	m_bReceiveShadows=true;
	m_bStatic=true;
	m_bVisible=true;
	m_bLightPassEnable=true;
	m_bSpecialPassEnable=false;

	SetName("PatchGrid");
}

PatchGrid::~PatchGrid()
{
	for(int i=0;i<PATCHSHADER_COUNT;i++)
		g_ShaderMgr.ReleaseShader(m_hShaders[i]);

	if(m_ppPatches)
	{
		int i;
		for(i=0;i<m_iWidth*m_iWidth;i++)
			if(m_ppPatches[i]) delete m_ppPatches[i];

		delete[] m_ppPatches;
	}
}

void PatchGrid::Create(IHeightmap *pMap, int iPatchSize)
{
	m_pMap=(Heightmap *)pMap;
	m_iPatchSize=iPatchSize;

/*	if(!m_pMap->IsTextureBuilded())
		m_pMap->BuildTexture();

	m_hHeightmapTexture=m_pMap->GetTexture();*/

	for(int i=0;i<PATCHSHADER_COUNT;i++)
	{
		if(m_hShaders[i].IsNull())
			m_hShaders[i]=g_ShaderMgr.LoadShader(szPatchShaderNames[i]);
	}
}

void PatchGrid::Clear(bool bReleaseTexture)
{
	if(bReleaseTexture)
	{
		g_TextureMgr.ReleaseTexture(m_hTerrainTexture);
		g_TextureMgr.ReleaseTexture(m_hDetailTexture);

		m_hTerrainTexture=Core::HTexture();
		m_hDetailTexture=Core::HTexture();
	}

	if(m_ppPatches)
	{
		int i;
		for(i=0;i<m_iWidth*m_iWidth;i++)
			if(m_ppPatches[i]) delete m_ppPatches[i];

		delete[] m_ppPatches;
	}

	m_ppPatches=0;
	m_pMap=0;
	m_vLastPos=Core::Vector3(-1.0f,-2.0f,-3.0f);
	m_iPatchSize=4;
	m_fTime=0.0f;
	m_iDrawnPatches=0;
	m_iWidth=0;
}

void PatchGrid::Generate()
{
	if(m_pMap->GetSize() % m_iPatchSize !=0)
		return;

	if(m_ppPatches)
	{
		int i;
		for(i=0;i<m_iWidth*m_iWidth;i++)
			if(m_ppPatches[i]) delete m_ppPatches[i];

		delete[] m_ppPatches;
	}

	m_iWidth=(m_pMap->GetSize()) / m_iPatchSize;
	m_TexCoords.Generate(m_pMap);

	m_ppPatches=new Patch*[m_iWidth*m_iWidth];
	int x,y;
	for(y=0;y<m_iWidth;y++)
		for(x=0;x<m_iWidth;x++)
			m_ppPatches[x+y*m_iWidth]=new Patch(m_pMap,&m_TexCoords,x*m_iPatchSize,y*m_iPatchSize,m_iPatchSize);
	
}

void PatchGrid::Simulate(float fTime)
{
	if(!m_ppPatches) return;

	const float fSimulationTimeStep=0.25f;

	m_fTime+=fTime;
	while(m_fTime > fSimulationTimeStep)
	{
		m_fTime-=fSimulationTimeStep;

		int iTotalChanged=0;
		int iPatchCount=m_iWidth*m_iWidth;
		int p;
		Patch **pPatch=m_ppPatches;

		for(p=0;p<iPatchCount;p++)
		{
			if((*pPatch)->GetRecalculate())
			{
				(*pPatch)->GetRecalculate()=false;
				(*pPatch)->ComputeVariance();

				if(++iTotalChanged >= 3)
					break;
			}

			pPatch++;
		}
	}
}

void PatchGrid::Reset()
{
	if(!m_ppPatches) return;

	Patch **pPatch=m_ppPatches;
	int iPatchCount=m_iWidth*m_iWidth;
	int p;
	for(p=0;p<iPatchCount;p++,pPatch++)
	{
		(*pPatch)->GetRecalculate()=false;
		(*pPatch)->ComputeVariance();
	}
}


void PatchGrid::Recalculate(int iPosX, int iPosY, int iDist)
{
	if(!m_ppPatches) return;

	m_vLastPos=Core::Vector3(-1.0f,-2.0f,-3.0f);

	Patch **pPatch=m_ppPatches;
	int p;
	for(p=0;p<(m_iWidth*m_iWidth);p++,pPatch++)
	{
		Patch *pCurrent=*pPatch;

		const int dist_x=abs(pCurrent->GetX() - iPosX);
		const int dist_y=abs(pCurrent->GetY() - iPosY);
		if(dist_x < iDist + pCurrent->GetWidth() && 
		   dist_y < iDist + pCurrent->GetWidth())
		   pCurrent->GetRecalculate()=true;
	}
}

void PatchGrid::Tesselate(const Core::Vector3& vPos)
{
	if((fabs(vPos.x - m_vLastPos.x) < 2.0f) &&
	   (fabs(vPos.y - m_vLastPos.y) < 2.0f) &&
	   (fabs(vPos.z - m_vLastPos.z) < 2.0f))
	   return;

	m_vLastPos=vPos;

	TriNodeMgr::Instance()->Reset();
	Patch **pPatch=m_ppPatches;
	int x,y;
	for(y=0;y<m_iWidth;y++)
		for(x=0;x<m_iWidth;x++)
		{
			(*pPatch)->Reset();

			if(x < (m_iWidth-1))
				(*pPatch)->GetLeftTri()->LeftNeighbor=(*(pPatch+1))->GetRightTri();
			else
				(*pPatch)->GetLeftTri()->LeftNeighbor=0;

			if(x > 0)
				(*pPatch)->GetRightTri()->LeftNeighbor=(*(pPatch-1))->GetLeftTri();
			else
				(*pPatch)->GetRightTri()->LeftNeighbor=0;

			if(y > 0)
				(*pPatch)->GetLeftTri()->RightNeighbor=(*(pPatch-m_iWidth))->GetRightTri();
			else
				(*pPatch)->GetLeftTri()->RightNeighbor=0;

			if(y < (m_iWidth-1))
				(*pPatch)->GetRightTri()->RightNeighbor=(*(pPatch+m_iWidth))->GetLeftTri();
			else
				(*pPatch)->GetRightTri()->RightNeighbor=0;

			pPatch++;
		}

	Core::Vector3 point;
	pPatch=m_ppPatches;
	int p;
	for(p=0;p<(m_iWidth*m_iWidth);p++,pPatch++)
	{
		if((*pPatch)->GetVisible())
		{
			point=(*pPatch)->GetMidPoint();
			point.y=(float)m_pMap->GetHeight((int)point.x,(int)point.z);
			point-=vPos;

			float dist=point.GetLength();
			float ramp=m_fVarianceRamp * dist;
			uint32 variance=(uint32)(m_fVarianceStart+ramp);

			(*pPatch)->Tesselate(variance);
		}
	}
}

bool PatchGrid::Visibility(const IViewFrustrum *pFrustrum)
{
	if(!m_ppPatches) return false;

	Patch **pPatch=m_ppPatches;
	int x,y;

	for(y=0;y<m_iWidth;y++)
		for(x=0;x<m_iWidth;x++)
		{
			(*pPatch)->GetVisible()=pFrustrum->IsBoxInside((*pPatch)->GetBBox());
			pPatch++;
		}

	Tesselate(pFrustrum->GetPosition());

	HeightmapVertexBuffer::Instance()->Reset();

	m_iDrawnPatches=0;
	pPatch=m_ppPatches;
	int p;
	for(p=0;p<(m_iWidth*m_iWidth);p++,pPatch++)
		if((*pPatch)->GetVisible())
		{
			m_iDrawnPatches++;
			(*pPatch)->Draw();
		}

	m_iDrawnPatches=HeightmapVertexBuffer::Instance()->GetCount()/3;

	if(m_iDrawnPatches > 0)
		g_RenderingQueue.Register(this);

	return true;
}

bool PatchGrid::DrawAmbient()
{
	if(!m_ppPatches || HeightmapVertexBuffer::Instance()->GetCount()==0) return false;

	if(m_hTerrainTexture.IsNull())
	{
		g_ShaderMgr.SetShader(m_hShaders[PATCHSHADER_NOTEXTURE]);
		const float fScale=m_pMap->GetScale();
		float fConstant[4]={ 1.0f / (fScale==0.0f ? 255.0f : fScale), 0.0f, 0.0f, 0.0f };
		g_Renderer.SetVertexShaderConstant(12,fConstant);
	}
	else
	{
		g_TextureMgr.SetTexture(0,m_hTerrainTexture);

		if(m_hDetailTexture.IsNull())
			g_ShaderMgr.SetShader(m_hShaders[PATCHSHADER_AMBIENT_TEX]);
		else
		{
			g_TextureMgr.SetTexture(1,m_hDetailTexture);
			g_ShaderMgr.SetShader(m_hShaders[PATCHSHADER_AMBIENT_TEXDETAIL]);
		}
	}

	g_Renderer.SetVertexDeclaration(Core::VX_PDT);
#ifdef DEEXPORT
	g_Renderer.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#else
	g_Renderer.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
#endif
	g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,HeightmapVertexBuffer::Instance()->GetCount(),
		HeightmapVertexBuffer::Instance()->GetBuffer(),Core::VX_PDT,0,0,D3DFMT_INDEX16);

	g_Renderer.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return true;
}

bool PatchGrid::DrawSunLight()
{
	if(!m_ppPatches || HeightmapVertexBuffer::Instance()->GetCount()==0 ||
	   m_hTerrainTexture.IsNull()) return false;

	g_TextureMgr.SetTexture(0,m_hTerrainTexture);

	if(m_hDetailTexture.IsNull())
		g_ShaderMgr.SetShader(m_hShaders[PATCHSHADER_SUN_TEX]);
	else
	{
		g_TextureMgr.SetTexture(1,m_hDetailTexture);
		g_ShaderMgr.SetShader(m_hShaders[PATCHSHADER_SUN_TEXDETAIL]);
	}

	g_Renderer.SetTransformation(TRANS_MODEL,Core::Matrix4::Identity,0,false);
	g_Renderer.SetVertexDeclaration(Core::VX_PDT);
	g_Renderer.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,HeightmapVertexBuffer::Instance()->GetCount(),
		HeightmapVertexBuffer::Instance()->GetBuffer(),Core::VX_PDT,0,0,D3DFMT_INDEX16);

	g_Renderer.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return true;
}

}