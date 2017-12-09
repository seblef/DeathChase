
#include "Renderer2D.h"
#include "Renderer.h"
#include "ShaderMgr.h"
#include "TextureMgr.h"
#include <d3d9.h>

namespace Core
{

Renderer2D::Renderer2D(IDirect3DDevice9 *pDevice) : m_pDevice(pDevice), m_bTextureEnable(false),
	m_iTriCreatedCount(0)
{
	m_hFillShader=g_ShaderMgr.LoadShader("2d_fill");
	m_hTextureShader=g_ShaderMgr.LoadShader("2d_texture");

	BuildIdxBuffer();
}

Renderer2D::~Renderer2D()
{
}

void Renderer2D::Start()
{
	g_Renderer.SetRenderState(D3DRS_ZENABLE,FALSE);
	g_Renderer.SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	g_Renderer.SetRenderState(D3DRS_LIGHTING,FALSE);
	g_Renderer.SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_Renderer.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_Renderer.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	g_Renderer.SetRenderState(D3DRS_FOGENABLE,FALSE);

	D3DMATERIAL9 mat;
	mat.Ambient.r=mat.Diffuse.r=1.0f;
	mat.Ambient.g=mat.Diffuse.g=1.0f;
	mat.Ambient.b=mat.Diffuse.b=1.0f;
	mat.Ambient.a=mat.Diffuse.a=1.0f;
	mat.Emissive.r=mat.Emissive.g=mat.Emissive.b=mat.Emissive.a=0.0f;
	mat.Specular.r=mat.Specular.g=mat.Specular.b=mat.Specular.a=0.0f;
	mat.Power=0.0f;
		
	g_Renderer.SetMaterial(&mat);
	g_Renderer.SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
}

void Renderer2D::End()
{
	if(m_FillBuffer.GetItemCount() > 0 || m_TexBuffer.GetItemCount() > 0)
		Execute();

	g_Renderer.SetRenderState(D3DRS_ZENABLE,TRUE);
	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void Renderer2D::DrawRectangle(int x1, int y1, int x2, int y2, const Color32& cColor)
{
	if(m_bTextureEnable)
	{
		Execute();
		m_bTextureEnable=false;
	}

	static Vertex2DPD Vert[4];

	Vert[0].vPos.x=(float)x1; Vert[0].vPos.y=(float)y1; Vert[0].vPos.z=0.5f;
	Vert[0].rhw=1.0f; Vert[0].cColor=cColor;
	Vert[1].vPos.x=(float)x2; Vert[1].vPos.y=(float)y1; Vert[1].vPos.z=0.5f;
	Vert[1].rhw=1.0f; Vert[1].cColor=cColor;
	Vert[2].vPos.x=(float)x2; Vert[2].vPos.y=(float)y2; Vert[2].vPos.z=0.5f;
	Vert[2].rhw=1.0f; Vert[2].cColor=cColor;
	Vert[3].vPos.x=(float)x1; Vert[3].vPos.y=(float)y2; Vert[3].vPos.z=0.5f;
	Vert[3].rhw=1.0f; Vert[3].cColor=cColor;

	m_FillBuffer.AddTab(Vert,4);
}

void Renderer2D::DrawRectangle(const Color32& cColor, const Rect& rect, const Rect *clip)
{
	DrawRectangle(rect,cColor,cColor,cColor,cColor,clip);
}

void Renderer2D::DrawRectangle(const Rect& pos,
			Color32 lu, Color32 ru, Color32 ld, Color32 rd, const Rect *clip)
{
	if(m_bTextureEnable)
	{
		Execute();
		m_bTextureEnable=false;
	}

	Rect pos2(pos);
	if(clip)
		pos2.ClipAgainst(*clip);

	if (!pos2.IsValid())
		return;

	static Vertex2DPD v[4]={
		Vertex2DPD(Vector3(0.0f,0.0f,0.5f),1.0f,Color32(255,255,255,255)),
		Vertex2DPD(Vector3(0.0f,0.0f,0.5f),1.0f,Color32(255,255,255,255)),
		Vertex2DPD(Vector3(0.0f,0.0f,0.5f),1.0f,Color32(255,255,255,255)),
		Vertex2DPD(Vector3(0.0f,0.0f,0.5f),1.0f,Color32(255,255,255,255)) };

	v[0].vPos.x=(float)pos2.m_vUpperLeft.x; v[0].vPos.y=(float)pos2.m_vUpperLeft.y; v[0].cColor=lu;
	v[1].vPos.x=(float)pos2.m_vLowerRight.x; v[1].vPos.y=(float)pos2.m_vUpperLeft.y; v[1].cColor=ru;
	v[2].vPos.x=(float)pos2.m_vLowerRight.x; v[2].vPos.y=(float)pos2.m_vLowerRight.y; v[2].cColor=rd;
	v[3].vPos.x=(float)pos2.m_vUpperLeft.x; v[3].vPos.y=(float)pos2.m_vLowerRight.y; v[3].cColor=ld;

	m_FillBuffer.AddTab(v,4);
}

void Renderer2D::Draw2DImage(HTexture hTex, int x1, int y1, int x2, int y2,
			float u1,float v1, float u2, float v2)
{
	if(hTex.IsNull()) return;

	if(!m_bTextureEnable || hTex!=m_hTexture)
	{
		Execute();
		m_bTextureEnable=true;
		m_hTexture=hTex;
		int x,y;
		g_TextureMgr.GetTextureSize(hTex,x,y);
		m_vTextureSize.x=KINV((float)x);
		m_vTextureSize.y=KINV((float)y);
	}

	static Vertex2DPDT Vert[4]={
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector),
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector),
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector),
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector) };

	Vert[0].vPos.x=(float)x1; Vert[0].vPos.y=(float)y1;	Vert[0].vUV.x=u1; Vert[0].vUV.y=v1;
	Vert[1].vPos.x=(float)x2; Vert[1].vPos.y=(float)y1; Vert[1].vUV.x=u2; Vert[1].vUV.y=v1;
	Vert[2].vPos.x=(float)x2; Vert[2].vPos.y=(float)y2; Vert[2].vUV.x=u2; Vert[2].vUV.y=v2;
	Vert[3].vPos.x=(float)x1; Vert[3].vPos.y=(float)y2; Vert[3].vUV.x=u1; Vert[3].vUV.y=v2;

	m_TexBuffer.AddTab(Vert,4);
}

void Renderer2D::Draw2DImage(HTexture hTex,const Vector2i& vPos,
			const Rect& sRect,
			const Rect* pClip,
			Color32 cColor,
			bool bUseTextureAlpha)
{
	if(hTex.IsNull() || !sRect.IsValid()) return;

	if(!m_bTextureEnable || hTex!=m_hTexture)
	{
		Execute();
		m_bTextureEnable=true;
		m_hTexture=hTex;
		int x,y;
		g_TextureMgr.GetTextureSize(hTex,x,y);
		m_vTextureSize.x=KINV((float)x);
		m_vTextureSize.y=KINV((float)y);
	}

	Vector2i vTargetPos(vPos);
	Vector2i vSourcePos(sRect.m_vUpperLeft);
	Vector2i vSourceSize(sRect.GetSize());

	if(pClip)
	{
		if(vTargetPos.x < pClip->m_vUpperLeft.x)
		{
			vSourceSize.x+=vTargetPos.x - pClip->m_vUpperLeft.x;
			if(vSourceSize.x <= 0)
				return;
			
			vSourcePos.x-=vTargetPos.x - pClip->m_vUpperLeft.x;
			vTargetPos.x=pClip->m_vUpperLeft.x;
		}

		if(vTargetPos.x + vSourceSize.x > pClip->m_vLowerRight.x)
		{
			vSourceSize.x-=(vTargetPos.x + vSourceSize.x) - pClip->m_vLowerRight.x;
			if(vSourceSize.x <= 0)
				return;
		}

		if(vTargetPos.y < pClip->m_vUpperLeft.y)
		{
			vSourceSize.y+=vTargetPos.y - pClip->m_vUpperLeft.y;
			if(vSourceSize.y <= 0)
				return;

			vSourcePos.y-=vTargetPos.y - pClip->m_vUpperLeft.y;
			vTargetPos.y=pClip->m_vUpperLeft.y;
		}

		if(vTargetPos.y + vSourceSize.y > pClip->m_vLowerRight.y)
		{
			vSourceSize.y-=(vTargetPos.y + vSourceSize.y) - pClip->m_vLowerRight.y;
			if(vSourceSize.y <= 0)
				return;
		}
	}

	if(vTargetPos.x < 0)
	{
		vSourceSize.x+=vTargetPos.x;
		if(vSourceSize.x <= 0)
			return;

		vSourcePos.x-=vTargetPos.x;
		vTargetPos.x=0;
	}

	if(vTargetPos.x + vSourceSize.x > g_Renderer.GetViewportSize().x)
	{
		vSourceSize.x-=(vTargetPos.x + vSourceSize.x) - g_Renderer.GetViewportSize().x;
		if(vSourceSize.x <= 0)
			return;
	}

	if(vTargetPos.y < 0)
	{
		vSourceSize.y+=vTargetPos.y;
		if(vSourceSize.y <= 0)
			return;

		vSourcePos.y-=vTargetPos.y;
		vTargetPos.y=0;
	}

	if(vTargetPos.y + vSourceSize.y > g_Renderer.GetViewportSize().y)
	{
		vSourceSize.y-=(vTargetPos.y + vSourceSize.y) - g_Renderer.GetViewportSize().y;
		if(vSourceSize.y <= 0)
			return;
	}

	const Vector2 TCoords1((((float)vSourcePos.x) + 0.5f) * m_vTextureSize.x,
		(((float)vSourcePos.y) + 0.5f) * m_vTextureSize.y);
	const Vector2 TCoords2((((float)vSourcePos.x + 0.5f + (float)vSourceSize.x)) * m_vTextureSize.x,
		(((float)vSourcePos.y + 0.5f + (float)vSourceSize.y)) * m_vTextureSize.y);

	Rect poss(vTargetPos,vTargetPos + vSourceSize);

	static Vertex2DPDT v[4]={
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector),
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector),
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector),
		Vertex2DPDT(Vector3(0.0f,0.0f,0.5f), 1.0f, Color32(255,255,255,255), Vector2::NullVector) };

	v[0].vPos.x=(float)poss.m_vUpperLeft.x; v[0].vPos.y=(float)poss.m_vUpperLeft.y; v[0].cColor=cColor;
	v[0].vUV=TCoords1;
	v[1].vPos.x=(float)poss.m_vLowerRight.x; v[1].vPos.y=(float)poss.m_vUpperLeft.y; v[1].cColor=cColor;
	v[1].vUV=Vector2(TCoords2.x,TCoords1.y);
	v[2].vPos.x=(float)poss.m_vLowerRight.x; v[2].vPos.y=(float)poss.m_vLowerRight.y; v[2].cColor=cColor;
	v[2].vUV=TCoords2;
	v[3].vPos.x=(float)poss.m_vUpperLeft.x; v[3].vPos.y=(float)poss.m_vLowerRight.y; v[3].cColor=cColor;
	v[3].vUV=Vector2(TCoords1.x,TCoords2.y);

	m_TexBuffer.AddTab(v,4);
}

void Renderer2D::Execute()
{
	if(m_FillBuffer.GetItemCount() > 0)
	{
		g_Renderer.SetVertexDeclaration(VX_2DPD);
		g_ShaderMgr.SetShader(m_hFillShader);

		const int tri_count=m_FillBuffer.GetItemCount() / 2;
		if(tri_count > m_iTriCreatedCount)
			BuildIdxBuffer(tri_count);

		g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,m_FillBuffer.GetItemCount(),
			m_FillBuffer.GetBuffer(),VX_2DPD,tri_count*3,m_IdxBuffer.GetBuffer(),D3DFMT_INDEX16);

		m_FillBuffer.Clear();
	}
	else if(m_TexBuffer.GetItemCount() > 0)
	{
		g_TextureMgr.SetTexture(0,m_hTexture);
		g_Renderer.SetVertexDeclaration(VX_2DPDT);
		g_ShaderMgr.SetShader(m_hTextureShader);

		const int tri_count=m_TexBuffer.GetItemCount() / 2;
		if(tri_count > m_iTriCreatedCount)
			BuildIdxBuffer(tri_count);

		g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,m_TexBuffer.GetItemCount(),
			m_TexBuffer.GetBuffer(),VX_2DPDT,tri_count*3,m_IdxBuffer.GetBuffer(),D3DFMT_INDEX16);

		m_TexBuffer.Clear();
	}
}

void Renderer2D::BuildIdxBuffer(int iCount)
{
	int tri_count=m_IdxBuffer.GetItemCount() / 3;
	int to_add=(iCount - tri_count) / 2;

	if(to_add < 0)
		return;

	int start_offset=(m_IdxBuffer.GetItemCount() / 6) * 4;
	uint16 tab[6]={start_offset,1+start_offset,2+start_offset,start_offset,2+start_offset,3+start_offset};
	int i,j;

	for(i=0;i<to_add;i++)
	{
		m_IdxBuffer.AddTab(tab,6);

		for(j=0;j<6;j++)
			tab[j]+=4;
	}

	m_iTriCreatedCount=iCount;
}

void Renderer2D::ShutDown()
{
	g_ShaderMgr.ReleaseShader(m_hFillShader);
	g_ShaderMgr.ReleaseShader(m_hTextureShader);
}

bool Renderer2D::BeginRestart()
{
	Execute();
	m_hTexture=HTexture();
	m_bTextureEnable=false;
	return true;
}

bool Renderer2D::EndRestart(IDirect3DDevice9 *pDevice)
{
	m_pDevice=pDevice;
	return true;
}

}