
#include "WaterSurround.h"
#include "../Core/Renderer.h"
#include "../Core/ShaderMgr.h"

namespace Game
{

WaterSurround::WaterSurround() :
	m_iTriangles(0),m_iHeightmapWidth(0),m_fHeight(1.0f),
	m_iTriCount(0), m_iVertCount(0),
	m_pVBuf(0), m_pIBuf(0) {}
WaterSurround::~WaterSurround()
{
	g_ShaderMgr.ReleaseShader(m_hShader);
}

void WaterSurround::Generate(int iHeightmapWidth,
							 int iWidth, int iHeight,
							 int iStartX, int iStartY,
							 int iWidthMult, int iHeightMult,
							 float fHeight)
{
	m_iHeightmapWidth=iHeightmapWidth;
	m_fStartX=(float)iStartX; m_fStartY=(float)iStartY;
	m_iWidth=iWidth; m_iHeight=iHeight;
	m_iWidthMult=iWidthMult; m_iHeightMult=iHeightMult;
	m_iTriangles=0;
	m_fHeight=fHeight;

	if(m_hShader.IsNull())
		m_hShader=g_ShaderMgr.LoadShader("StdWaterSurround");

	Clear();
}

void WaterSurround::Clear()
{
	if(m_pVBuf) m_pVBuf->Release();
	if(m_pIBuf) m_pIBuf->Release();

	m_pVBuf=0;
	m_pIBuf=0;

	m_iVertCount=0;
	m_iTriCount=0;
}

void WaterSurround::Draw()
{
	if(!m_pVBuf)
		GenerateList();
	else if(!m_hShader.IsNull())
	{
		g_ShaderMgr.SetShader(m_hShader);

		g_Renderer.SetVertexBuffer(m_pVBuf,m_iVertCount,Core::VertexSizeTable[Core::VX_PNDTT]);
		g_Renderer.SetIndexBuffer(m_pIBuf);
		g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_iTriCount);
	}
}

void WaterSurround::DrawPoint(Core::Vector3 &vStart,
							  Core::Vector3 &vDiff,
							  int iNumberX, int iMaxX,
							  int iNumberY, int iMaxY,
							  Core::TDynamicTab<Core::VertexPNDTT> *pVert,
							  Core::TDynamicTab<uint16> *pInd)
{
	if(m_iPointCount >= 4)
	{
		uint16 ind[6];
		ind[0]=pVert->GetItemCount()-4;
		ind[1]=ind[0]+1;
		ind[2]=ind[0]+2;

		ind[3]=ind[0]+1;
		ind[4]=ind[0]+3;
		ind[5]=ind[0]+2;

		pInd->AddTab(ind,6);
		m_iPointCount=2;
	}

	float fNumberY=(float)iNumberY;
	float fMaxY=(float)iMaxY;
	float fNumberX=(float)iNumberX;
	float fMaxX=(float)iMaxX;
	int iTemp=((int)((fNumberX / fMaxX) * 180.0f)) % 360;
	if(iTemp < 0) iTemp+=360;
	
	float fMult=Core::Matrix4::TSin[iTemp];

	fMult*=0.3f + (0.4f * fNumberX / fMaxX);
	fMult+=1.0f;
	fNumberY*=fMult;

	Core::VertexPNDTT vert;
	vert.vPos=vStart + (vDiff * fNumberY / fMaxY);
	vert.vPos.y=m_fHeight;
	vert.vUV1.x=vert.vPos.x / 32.0f;
	vert.vUV1.y=vert.vPos.z / 32.0f;
	vert.vUV0=vert.vUV1 / 16.0f;
	vert.vNorm=Core::Vector3::YAxisVector;
	vert.cColor=Core::Color32(229,178,178,178);

	pVert->Add(vert);
	m_iPointCount++;
}

void WaterSurround::GenerateList()
{
	Core::TDynamicTab<Core::VertexPNDTT> verts;
	Core::TDynamicTab<uint16> indices;

	const float fMapWidth=(float)m_iHeightmapWidth;
	const float fLargeMinWidth=m_fStartX - 1024.0f;
	const float fLargeMaxWidth=-m_fStartX + 1024.0f + fMapWidth;
	const float fLargeMinHeight=m_fStartY - 1024.0f;
	const float fLargeMaxHeight=-m_fStartY + 1024.0f + fMapWidth;
	const float fLargeLenWidth=fLargeMaxWidth - fLargeMinWidth;
	const float fLargeLenHeight=fLargeMaxHeight - fLargeMinHeight;
	const float fLargeMultWidth=fLargeLenWidth / float(m_iWidth);
	const float fLargeMultHeight=fLargeLenHeight / float(m_iHeight);

	int iMaxSquares=5;
	int i,j;
	for(j=0;j<iMaxSquares;j++)
	{
		float x=m_fStartX;
		float y=m_fStartY;
		float lx=fLargeMinWidth;
		float ly=fLargeMinHeight;

		m_iPointCount=0;

		for(i=0;i<m_iWidth-1;i++)
		{
			Core::Vector3 vStart(x,0.0f,y);
			Core::Vector3 vEnd(lx,0.0f,ly);
			Core::Vector3 vDiff(vEnd-vStart);

			DrawPoint(vStart,vDiff,i,m_iWidth - 1,j,iMaxSquares,&verts,&indices);
			DrawPoint(vStart,vDiff,i,m_iWidth-1,j+1,iMaxSquares,&verts,&indices);

			x+=(float)m_iWidthMult;
			lx+=fLargeMultWidth;
		}
		for(i=0;i<m_iHeight-1;i++)
		{
			Core::Vector3 vStart(x,0.0f,y);
			Core::Vector3 vEnd(lx,0.0f,ly);
			Core::Vector3 vDiff(vEnd-vStart);

			DrawPoint(vStart,vDiff,i,m_iHeight-1,j,iMaxSquares,&verts,&indices);
			DrawPoint(vStart,vDiff,i,m_iHeight-1,j+1,iMaxSquares,&verts,&indices);

			y+=(float)m_iHeightMult;
			ly+=fLargeMultHeight;
		}
		for(i=0;i<m_iWidth-1;i++)
		{
			Core::Vector3 vStart(x,0.0f,y);
			Core::Vector3 vEnd(lx,0.0f,ly);
			Core::Vector3 vDiff(vEnd - vStart);

			DrawPoint(vStart,vDiff,i,m_iWidth-1,j,iMaxSquares,&verts,&indices);
			DrawPoint(vStart,vDiff,i,m_iWidth-1,j+1,iMaxSquares,&verts,&indices);

			x-=(float)m_iWidthMult;
			lx-=fLargeMultWidth;
		}
		for(i=0;i<m_iHeight;i++)
		{
			Core::Vector3 vStart(x,0.0f,y);
			Core::Vector3 vEnd(lx,0.0f,ly);
			Core::Vector3 vDiff(vEnd-vStart);

			DrawPoint(vStart,vDiff,i,m_iHeight-1,j,iMaxSquares,&verts,&indices);
			DrawPoint(vStart,vDiff,i,m_iHeight-1,j+1,iMaxSquares,&verts,&indices);

			y-=(float)m_iHeightMult;
			ly-=fLargeMultHeight;
		}

		if(m_iPointCount >= 4)
		{
			uint16 ind[6];
			ind[0]=verts.GetItemCount()-4;
			ind[1]=ind[0]+1;
			ind[2]=ind[0]+2;

			ind[3]=ind[0]+1;
			ind[4]=ind[0]+3;
			ind[5]=ind[0]+2;

			indices.AddTab(ind,6);
			m_iPointCount=0;
		}
	}

	m_pVBuf=g_Renderer.CreateVertexBuffer(verts.GetBuffer(),verts.GetItemCount(),Core::VX_PNDTT);
	m_pIBuf=g_Renderer.CreateIndexBuffer(indices.GetBuffer(),indices.GetItemCount(),D3DFMT_INDEX16);

	m_iVertCount=verts.GetItemCount();
	m_iTriCount=indices.GetItemCount() / 3;
}

}