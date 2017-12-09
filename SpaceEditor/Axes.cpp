
#include "Axes.h"

#include "../DeathEditor/Interfaces.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IShaderMgr.h"

const Core::VertexPD OriginVertices[4]={
	Core::VertexPD(Core::Vector3::NullVector, Core::Color32(255,255,255,255)),
	Core::VertexPD(Core::Vector3(1.0f,0.0f,0.0f), Core::Color32(255,255,0,0)),
	Core::VertexPD(Core::Vector3(0.0f,1.0f,0.0f), Core::Color32(255,0,255,0)),
	Core::VertexPD(Core::Vector3(0.0f,0.0f,1.0f), Core::Color32(255,0,0,255))
};

const uint16 OriginIndices[6]={0,1, 0,2, 0,3};

Core::HShader Axes::m_hShader;

void Axes::Draw(const Core::Matrix4 &mTrans)
{
	if(m_hShader.IsNull())
		m_hShader=g_ShaderMgr->LoadShader("StdAxes");

	g_ShaderMgr->SetShader(m_hShader);
	g_Renderer->SetTransformation(TRANS_MODEL,mTrans,0,false);
	g_Renderer->SetVertexDeclaration(Core::VX_PD);
	g_Renderer->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	g_Renderer->SetRenderState(D3DRS_ZENABLE,FALSE);
	g_Renderer->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

	g_Renderer->RenderDynamicBuffers(D3DPT_LINELIST,4,(void *)OriginVertices,Core::VX_PD,6,(void *)OriginIndices,D3DFMT_INDEX16);

	g_Renderer->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	g_Renderer->SetRenderState(D3DRS_ZENABLE,TRUE);
}