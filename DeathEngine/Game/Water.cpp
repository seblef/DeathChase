
#include "Water.h"
#include "../Core/ShaderMgr.h"
#include "../Core/TextureMgr.h"
#include "../Core/Renderer.h"
#include "RenderingQueue.h"

#define WATER_SIMULATIONTIME			50

namespace Game
{

Water::WaterPatch::WaterPatch() : m_bVisible(false), m_pVBuf(0), m_pIBuf(0),
	m_iTriCount(0), m_iVertCount(0)
{}

Water::WaterPatch::~WaterPatch()
{
	Clear();
}

bool Water::WaterPatch::Generate(int iStartX, int iStartY,
								 int iX, int iY,
								 int iWidth, int iHeight,
								 int iMapWidth, int iMapRes,
								 WaterEntry *pMap, IHeightmap *pHMap)
{
	Clear();

	const float tfactx=2.0f / (float)iMapWidth;
	const float tfacty=tfactx;
	const float tfactx2=16.0f * tfactx;
	const float tfacty2=tfactx2;
	float fHeight=pMap[1].fHeight;
	const float fHeightFact=155.0f * KINV(fHeight);

	struct VItem
	{
		Core::VertexPNDTT vert;
		int newid;
	};

	VItem *pVTemp=new VItem[(iWidth+1) * (iHeight+1)];
	VItem *pVert=pVTemp;

	int yh=iStartY;
	int x,y;
	for(y=iY;y<=(iY+iHeight);y++)
	{
		int xh=iStartX;
		for(x=iX;x<=(iX+iWidth);x++)
		{
			WaterEntry *pEntry=pMap + y*(iMapWidth+1) + x;
			pEntry->pbVisible=&m_bVisible;

			uint32 alpha=100 + MIN((uint32)(MAX(pEntry->fDepth,0.0f) * fHeightFact),155);
			pVert->vert=Core::VertexPNDTT(Core::Vector3((float)xh,pEntry->fHeight,(float)yh),
				pEntry->vNormal,Core::Color32((byte)alpha,178,178,178),
				Core::Vector2((float)x * tfactx, (float)y * tfacty),
				Core::Vector2((float)x * tfactx2, (float)y * tfacty2));

			pVert->newid=-1;
			xh+=iMapRes;
			pVert++;
		}
		yh+=iMapRes;
	}

	Core::TDynamicTab<Core::VertexPNDTT> m_Vertices;
	Core::TDynamicTab<uint16> m_Indices;

	yh=iStartY;
	int iHWidth=pHMap->GetSize();

	for(y=iY;y<(iY+iHeight);y++,yh+=iMapRes)
	{
		int v1=(y-iY) * (iWidth+1);
		int v2=v1 + (iWidth+1);
		int xh=iStartX;
		for(x=iX;x<(iX+iWidth);x++,xh+=iMapRes)
		{
			bool bVisible=false;
			if(xh >= iHWidth || (xh + iMapRes) < 0 ||
			   yh >= iHWidth || (yh + iMapRes) < 0)
			  bVisible=true;
			else
			{
				for(int j=MAX(yh,0);j<=MIN((yh+iMapRes),iHWidth);j++)
					for(int i=MAX(xh,0);i<=MIN((xh+iMapRes),iHWidth);i++)
					{
						if(fHeight >= pHMap->GetHeight(i,j))
						{
							bVisible=true;
							break;
							break;
						}
					}
			}

			if(bVisible)
			{
				uint16 ind[6]={v1,v2,v1+1,v2,v2+1,v1+1};

				for(int i=0;i<6;i++)
				{
					if(pVTemp[ind[i]].newid==-1)
					{
						m_Vertices.Add(pVTemp[ind[i]].vert);
						pVTemp[ind[i]].newid=m_Vertices.GetItemCount()-1;
						ind[i]=m_Vertices.GetItemCount()-1;
					}
					else
						ind[i]=pVTemp[ind[i]].newid;
				}

				m_Indices.AddTab(ind,6);
			}

			v1++;
			v2++;
		}
	}

	delete[] pVTemp;

	if(m_Indices.GetItemCount() > 0)
	{
		m_pVBuf=g_Renderer.CreateVertexBuffer(m_Vertices.GetBuffer(),m_Vertices.GetItemCount(),Core::VX_PNDTT);
		m_pIBuf=g_Renderer.CreateIndexBuffer(m_Indices.GetBuffer(),m_Indices.GetItemCount(),D3DFMT_INDEX16);
	}

	m_iVertCount=m_Vertices.GetItemCount();
	m_iTriCount=m_Indices.GetItemCount() / 3;

	m_iStartX=iStartX; m_iStartY=iStartY;
	m_iWidth=iWidth; m_iHeight=iHeight;

	m_BBox.SetMin(Core::Vector3((float)m_iStartX,fHeight - 5.0f,(float)m_iStartY ));
	m_BBox.SetMax(Core::Vector3((float)(m_iStartX + m_iWidth * iMapRes),fHeight+5.0f,(float)(m_iStartY + m_iHeight * iMapRes)));

	return true;
}

void Water::WaterPatch::Clear()
{
	if(m_pVBuf) m_pVBuf->Release();
	if(m_pIBuf) m_pIBuf->Release();

	m_pVBuf=0;
	m_pIBuf=0;

	m_iTriCount=0;
	m_iVertCount=0;
}

void Water::WaterPatch::Update(WaterEntry *pMap, int iMapWidth)
{
/*	if(m_Vertices.GetItemCount()==0) return;

	const float fHeightFact=KINV(pMap[1].fHeight) * 255.0f;

	Core::VertexPNDTT *pVert=m_Vertices.GetBuffer();
	for(int y=m_iStartY;y<=(m_iStartY+m_iHeight);y++)
	{
		WaterEntry *pEntry=pMap + y*(iMapWidth+1)+m_iStartX;
		for(int x=m_iStartX;x<=(m_iStartX+m_iWidth);x++)
		{
			pVert->vNorm=pEntry->vNormal;
			pVert->vPos.y=pEntry->fHeight;
			pVert->cColor.a=(byte)(((uint32)(pEntry->fDepth * fHeightFact)) & 0xFF);

			pVert++; pEntry++;
		}
	}*/
}



Water::Water() : m_bDynamic(true), m_bSurround(false), m_fHeight(1.0f),
	m_iResolution(8),m_pHMap(0),m_pMap(0),m_pPatches(0), m_iPatchesCount(0)
#ifndef DEEXPORT
	,IObject(OBJ_WATER)
#endif
{
	m_bStatic=true;
	m_bCastShadows=true;
	m_bReceiveShadows=false;
	m_bLightPassEnable=false;
	m_bSpecialPassEnable=false;
	m_bVisible=true;
	SetName("Water");
}

Water::~Water()
{
	g_ShaderMgr.ReleaseShader(m_hStaticShader);
	g_ShaderMgr.ReleaseShader(m_hDynamicShader);
	g_TextureMgr.ReleaseTexture(m_hTexture);

	Clear();
}

void Water::Clear()
{
	m_bDynamic=true;
	m_bSurround=false;
	m_fHeight=1.0f;
	m_iResolution=8;
	m_pHMap=0;

	if(m_pMap) delete[] m_pMap;
	if(m_pPatches) delete[] m_pPatches;

	m_pMap=0;
	m_pPatches=0;
	m_iPatchesCount=0;

	m_Surround.Clear();
}

bool Water::Generate(IHeightmap *pHMap, const int iRes,
					 const float fHeight, const bool bSurround,
					 const bool bDynamic)
{
	Clear();

	if(m_hDynamicShader.IsNull()) m_hDynamicShader=g_ShaderMgr.LoadShader("StdWaterDynamic");
	if(m_hStaticShader.IsNull()) m_hStaticShader=g_ShaderMgr.LoadShader("StdWaterStatic");

	const int iHSize=pHMap->GetSize();
	const int iMapWidth=iHSize / iRes;
	const int iPatchSideCount=iMapWidth / 16;
		
	m_pPatches=new WaterPatch[iPatchSideCount * iPatchSideCount];
	m_pMap=new WaterEntry[(iMapWidth + 1) * (iMapWidth+1)];

	if(bSurround)
	{
		const int iMapMult=iRes + iRes / 2;
		const int iStart=-iMapWidth*2;

		m_Surround.Generate(iHSize,iMapWidth,iMapWidth,iStart,iStart,iMapMult,iMapMult,fHeight);

		for(int j=0;j<=iMapWidth;j++)
		{
			WaterEntry *pEntry=&m_pMap[j*(iMapWidth + 1)];
			int yh=iStart + j * iMapMult;

			if(yh >= 0 && yh < iHSize)
			{
				int xh=iStart;
				for(int i=0;i<=iMapWidth;i++)
				{
					if(xh >= 0 && xh < iHSize)
						pEntry->fDepth=fHeight - pHMap->GetHeight(xh,yh);
					else
						pEntry->fDepth=fHeight;

					pEntry->fForce=0.0f;
					pEntry->fHeight=fHeight;
					pEntry->fVelocity=0.0f;
					pEntry->vNormal=Core::Vector3::YAxisVector;

					xh+=iMapMult;
					pEntry++;
				}
			}
			else
			{
				for(int i=0;i<=iMapWidth;i++)
				{
					pEntry->fDepth=fHeight;
					pEntry->fForce=0.0f;
					pEntry->fHeight=fHeight;
					pEntry->fVelocity=0.0f;
					pEntry->vNormal=Core::Vector3::YAxisVector;

					pEntry++;
				}
			}			
		}

		WaterPatch *pPatch=m_pPatches;

		for(int j=0;j<iPatchSideCount;j++)
		{
			int y=iStart + j*16*iMapMult;
			int x=iStart;
			for(int i=0;i<iPatchSideCount;i++)
			{
				pPatch->Generate(x,y,i*16,j*16,16,16,iMapWidth,iMapMult,m_pMap,pHMap);
				x+=16*iMapMult;
				pPatch++;
			}
		}
	}
	else
	{
		const int iMapMult=iRes;

		m_Surround.Generate(iHSize,iMapWidth,iMapWidth,0,0,iMapMult,iMapMult,fHeight);

		for(int j=0;j<=iMapWidth;j++)
		{
			WaterEntry *pEntry=&m_pMap[j*(iMapWidth + 1)];
			int yh=j * iMapMult;
			int xh=0;

			for(int i=0;i<=iMapWidth;i++)
			{
				pEntry->fDepth=fHeight - pHMap->GetHeight(xh,yh);
				pEntry->fForce=0.0f;
				pEntry->fHeight=fHeight;
				pEntry->fVelocity=0.0f;
				pEntry->vNormal=Core::Vector3::YAxisVector;

				xh+=iMapMult;
				pEntry++;
			}
		}

		WaterPatch *pPatch=m_pPatches;

		for(int j=0;j<iPatchSideCount;j++)
		{
			int y=j*16*iMapMult;
			int x=0;
			for(int i=0;i<iPatchSideCount;i++)
			{
				pPatch->Generate(x,y,i*16,j*16,16,16,iMapWidth,iMapMult,m_pMap,pHMap);
				x+=16*iMapMult;
				pPatch++;
			}
		}
	}

	m_iResolution=iRes;
	m_bSurround=bSurround;
	m_bDynamic=bDynamic;
	m_pHMap=pHMap;
	m_fHeight=fHeight;
	m_iPatchesCount=iPatchSideCount*iPatchSideCount;

	return true;
}

void Water::Simulate(const uint32 dwTime)
{

}

bool Water::Visibility(const IViewFrustrum *pFrustrum)
{
	if(!m_pPatches) return false;

	int iVisCount=0;

	for(int i=0;i<m_iPatchesCount;i++)
	{
		m_pPatches[i].m_bVisible=m_pPatches[i].m_iTriCount > 0 ? pFrustrum->IsBoxInside(m_pPatches[i].m_BBox) : false;
		if(m_pPatches[i].m_bVisible)
			iVisCount++;
	}

	if(iVisCount > 0)
		g_RenderingQueue.Register(this);

	return true;
}

bool Water::Draw()
{
	if(!m_pPatches) return false;
	
	g_TextureMgr.SetTexture(0,m_hTexture);
	g_TextureMgr.SetTexture(1,m_hTexture);

	g_Renderer.SetVertexDeclaration(Core::VX_PNDTT);

	if(m_bSurround)
	{
		g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		g_Renderer.SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
		m_Surround.Draw();
		g_Renderer.SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	}

	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	g_Renderer.SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_Renderer.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	if(m_bDynamic)
		g_ShaderMgr.SetShader(m_hDynamicShader);
	else
		g_ShaderMgr.SetShader(m_hStaticShader);

	for(int i=0;i<m_iPatchesCount;i++)
	{
		if(m_pPatches[i].m_bVisible)
		{
			g_Renderer.SetVertexBuffer(m_pPatches[i].m_pVBuf,m_pPatches[i].m_iVertCount,Core::VertexSizeTable[Core::VX_PNDTT]);
			g_Renderer.SetIndexBuffer(m_pPatches[i].m_pIBuf);
			g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_pPatches[i].m_iTriCount);
		}
	}

	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	return true;
}

void Water::SetWaterTexture(Core::HTexture hTex)
{
	if(!m_hTexture.IsNull())
		g_TextureMgr.ReleaseTexture(m_hTexture);

	m_hTexture=hTex;
}

}