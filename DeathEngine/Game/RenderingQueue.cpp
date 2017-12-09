
#include "RenderingQueue.h"
#include "../Core/Renderer.h"
#include "../Core/Material.h"
#include "../Core/MeshMgr.h"
#include "../Core/ShaderMgr.h"
#include "../Core/ParticleEngine.h"

namespace Game
{

RenderingQueue::RenderingQueue() : m_bLightEnable(true), m_bShadowsEnable(true), m_pCamera(0)
{
}

RenderingQueue::~RenderingQueue()
{
	g_ShaderMgr.ReleaseShader(m_hShadowsShader);
}

void RenderingQueue::Register(IObject *pObj)
{
	if(pObj->IsLightPassEnable())
	{
		if(pObj->ReceiveShadows())
			m_AmbientSunPass.push_back(pObj);
		else
			m_NoReceiveShadows.push_back(pObj);
	}
	else
		m_NoLightPass.push_back(pObj);

	if(pObj->CastShadows())
		m_CastShadows.push_back(pObj);

	if(pObj->IsSpecialPassEnable())
		m_SpecialPass.push_back(pObj);
}

void RenderingQueue::Execute(const Core::Vector3& vSunDir)
{
	if(m_hShadowsShader.IsNull())
		m_hShadowsShader=g_ShaderMgr.LoadShader("StdShadows");

	if(!m_bLightEnable)
	{
		const float fAmbient[4]={1.0f,1.0f,1.0f,1.0f};
		g_Renderer.SetVertexShaderConstant(COLOR_AMBIENT_REG,fAmbient);
	}

	ObjectVector::iterator i;

	g_Renderer.SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	Core::Material::SetRenderingPass(RENDERINGPASS_AMBIENT);

	for(i=m_AmbientSunPass.begin();i!=m_AmbientSunPass.end();i++)
		(*i)->DrawAmbient();

	g_MeshMgr.Execute();

	if(m_bLightEnable)
	{
		if(m_CastShadows.size() > 0 && m_bShadowsEnable)
		{
			g_Renderer.ClearBuffer(D3DCLEAR_STENCIL,0);
			
			g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_Renderer.SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ZERO);
			g_Renderer.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
			g_Renderer.SetRenderState(D3DRS_STENCILENABLE,TRUE);
			g_Renderer.SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);
			g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

			g_Renderer.SetRenderState(D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP);
			g_Renderer.SetRenderState(D3DRS_STENCILFAIL,D3DSTENCILOP_KEEP);
			g_Renderer.SetRenderState(D3DRS_STENCILREF,0x1);
			g_Renderer.SetRenderState(D3DRS_STENCILMASK,0xffffffff);
			g_Renderer.SetRenderState(D3DRS_STENCILWRITEMASK,0xffffffff);
			g_Renderer.SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
			g_Renderer.SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
			g_Renderer.SetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
			g_Renderer.SetRenderState(D3DRS_CCW_STENCILPASS,D3DSTENCILOP_DECR);

			g_Renderer.SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
			g_Renderer.SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
			g_Renderer.SetRenderState(D3DRS_TWOSIDEDSTENCILMODE,TRUE);

			g_ShaderMgr.SetShader(m_hShadowsShader);

			for(i=m_CastShadows.begin();i!=m_CastShadows.end();i++)
				(*i)->CastSunShadows(vSunDir);

			g_Renderer.SetRenderState(D3DRS_TWOSIDEDSTENCILMODE,FALSE);
			g_Renderer.SetRenderState(D3DRS_STENCILFUNC,D3DCMP_GREATER);
			g_Renderer.SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);

			g_Renderer.SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
			g_Renderer.SetRenderState(D3DRS_ZFUNC,D3DCMP_EQUAL);
			Core::Material::SetRenderingPass(RENDERINGPASS_SUN);

			for(i=m_AmbientSunPass.begin();i!=m_AmbientSunPass.end();i++)
				(*i)->DrawSunLight();

			g_Renderer.SetRenderState(D3DRS_STENCILENABLE,FALSE);
		}
		else
		{
			g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			g_Renderer.SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
			g_Renderer.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
			g_Renderer.SetRenderState(D3DRS_ZFUNC,D3DCMP_EQUAL);
			g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			Core::Material::SetRenderingPass(RENDERINGPASS_SUN);

			for(i=m_AmbientSunPass.begin();i!=m_AmbientSunPass.end();i++)
				(*i)->DrawSunLight();
		}

		if(m_NoReceiveShadows.size() > 0)
		{
			g_MeshMgr.Execute();
			for(i=m_NoReceiveShadows.begin();i!=m_NoReceiveShadows.end();i++)
				(*i)->DrawSunLight();
		}

		g_MeshMgr.Execute();
	}

	g_Renderer.SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	Core::Material::SetRenderingPass(RENDERINGPASS_AMBIENT);

	for(i=m_NoLightPass.begin();i!=m_NoLightPass.end();i++)
		(*i)->Draw();

	g_MeshMgr.Execute();

	Core::Material::SetRenderingPass(RENDERINGPASS_SPECIAL);

	for(i=m_SpecialPass.begin();i!=m_SpecialPass.end();i++)
		(*i)->DrawSpecial();

	g_MeshMgr.Execute();
	g_ParticleEngine.Draw(m_pCamera);
	g_MeshMgr.Execute();

	Core::Material::SetRenderingPass(RENDERINGPASS_AMBIENT);

	m_AmbientSunPass.clear();
	m_NoLightPass.clear();
	m_CastShadows.clear();
	m_NoReceiveShadows.clear();
	m_SpecialPass.clear();
}

}