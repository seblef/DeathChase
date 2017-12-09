
#include "SkyBox.h"
#include "../Core/TextureMgr.h"
#include "../Core/ShaderMgr.h"
#include "../Core/Renderer.h"


namespace Game
{

SkyBox::SkyBox() : m_pDevice(0)
{
}

SkyBox::~SkyBox()
{
	int i;
	for(i=0;i<SKY_COUNT;i++)
		g_TextureMgr.ReleaseTexture(m_hTextures[i]);

	g_ShaderMgr.ReleaseShader(m_hShader);
}

bool SkyBox::Create(IDirect3DDevice9 *pDevice, const float fSize)
{
	if(!pDevice) return false;
	m_pDevice=pDevice;

	if(m_hShader.IsNull())
		m_hShader=g_ShaderMgr.LoadShader("skybox");

	const float one_texel=1.0f / (512.0f*1.5f);
	const float o=one_texel;
	const float t=1.0f-one_texel;

	//		up
	m_Vertices[0].vPos=Core::Vector3( fSize, fSize, fSize);
	m_Vertices[0].vUV=Core::Vector2(o,o);
	m_Vertices[1].vPos=Core::Vector3(-fSize, fSize, fSize);
	m_Vertices[1].vUV=Core::Vector2(o,t);
	m_Vertices[2].vPos=Core::Vector3(-fSize, fSize,-fSize);
	m_Vertices[2].vUV=Core::Vector2(t,t);
	m_Vertices[3].vPos=Core::Vector3( fSize, fSize,-fSize);
	m_Vertices[3].vUV=Core::Vector2(t,o);

	//		front
	m_Vertices[4].vPos=Core::Vector3(-fSize,-fSize,-fSize);
	m_Vertices[4].vUV=Core::Vector2(o,t);
	m_Vertices[5].vPos=Core::Vector3( fSize,-fSize,-fSize);
	m_Vertices[5].vUV=Core::Vector2(t,t);
	m_Vertices[6].vPos=Core::Vector3( fSize, fSize,-fSize);
	m_Vertices[6].vUV=Core::Vector2(t,o);
	m_Vertices[7].vPos=Core::Vector3(-fSize, fSize,-fSize);
	m_Vertices[7].vUV=Core::Vector2(o,o);

	//		right
	m_Vertices[8].vPos=Core::Vector3(-fSize,-fSize, fSize);
	m_Vertices[8].vUV=Core::Vector2(o,t);
	m_Vertices[9].vPos=Core::Vector3(-fSize,-fSize,-fSize);
	m_Vertices[9].vUV=Core::Vector2(t,t);
	m_Vertices[10].vPos=Core::Vector3(-fSize, fSize,-fSize);
	m_Vertices[10].vUV=Core::Vector2(t,o);
	m_Vertices[11].vPos=Core::Vector3(-fSize, fSize, fSize);
	m_Vertices[11].vUV=Core::Vector2(o,o);

	//		back
	m_Vertices[12].vPos=Core::Vector3( fSize,-fSize, fSize);
	m_Vertices[12].vUV=Core::Vector2(o,t);
	m_Vertices[13].vPos=Core::Vector3(-fSize,-fSize, fSize);
	m_Vertices[13].vUV=Core::Vector2(t,t);
	m_Vertices[14].vPos=Core::Vector3(-fSize, fSize, fSize);
	m_Vertices[14].vUV=Core::Vector2(t,o);
	m_Vertices[15].vPos=Core::Vector3( fSize, fSize, fSize);
	m_Vertices[15].vUV=Core::Vector2(o,o);

	//		left
	m_Vertices[16].vPos=Core::Vector3( fSize,-fSize,-fSize);
	m_Vertices[16].vUV=Core::Vector2(o,t);
	m_Vertices[17].vPos=Core::Vector3( fSize,-fSize, fSize);
	m_Vertices[17].vUV=Core::Vector2(t,t);
	m_Vertices[18].vPos=Core::Vector3( fSize, fSize, fSize);
	m_Vertices[18].vUV=Core::Vector2(t,o);
	m_Vertices[19].vPos=Core::Vector3( fSize, fSize,-fSize);
	m_Vertices[19].vUV=Core::Vector2(o,o);

	m_wIndices[0]=0; m_wIndices[1]=1; m_wIndices[2]=2;
	m_wIndices[3]=0; m_wIndices[4]=2; m_wIndices[5]=3;

	return true;
}

bool SkyBox::Draw(const Core::Vector3 &vCamPos)
{
	Core::Matrix4 mWorld;
	mWorld.CreateTranslate(vCamPos.x,vCamPos.y,vCamPos.z);

	g_Renderer.SetTransformation(TRANS_MODEL,mWorld,0,false);
	g_Renderer.SetVertexDeclaration(Core::VX_PT);
	g_ShaderMgr.SetShader(m_hShader);

	g_Renderer.SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	g_Renderer.SetRenderState(D3DRS_LIGHTING,FALSE);
	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_Renderer.SetRenderState(D3DRS_FOGENABLE,FALSE);
	g_Renderer.SetRenderState(D3DRS_ZENABLE,FALSE);

	int i;
	for(i=0;i<SKY_COUNT;i++)
	{
		if(!m_hTextures[i].IsNull())
		{
			g_TextureMgr.SetTexture(0,m_hTextures[i]);
			g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,4,&m_Vertices[i*4],
				Core::VX_PT,6,m_wIndices,D3DFMT_INDEX16);
		}
	}

	g_Renderer.SetRenderState(D3DRS_ZENABLE,TRUE);

	return true;
}

bool SkyBox::SetTexture(SkyFace iFace, Core::HTexture hTex)
{
	if(iFace >= 0 && iFace < SKY_COUNT)
	{
		m_hTextures[iFace]=hTex;
		return true;
	}
	else
		return false;
}

Core::HTexture SkyBox::GetTexture(SkyFace iFace)
{
	if(iFace >= 0 && iFace < SKY_COUNT)
		return m_hTextures[iFace];
	else
		return Core::HTexture();
}

void SkyBox::Clear()
{
	for(int i=0;i<SKY_COUNT;i++)
	{
		g_TextureMgr.ReleaseTexture(m_hTextures[i]);
		m_hTextures[i]=Core::HTexture();
	}

}

}