
#include "EditorSun.h"
#include "../Interfaces.h"
#include "../../DeathEngine/Export/IRenderer.h"
#include "../../DeathEngine/Export/IMeshMgr.h"
#include "../../DeathEngine/Export/IMesh.h"
#include "../../DeathEngine/Export/IShaderMgr.h"
#include "../../DeathEngine/Export/IMap.h"
#include "../../DeathEngine/Export/ISun.h"
#include "../../DeathEngine/Export/IHeightmap.h"

EditorSun::EditorSun() : m_pVBuf(0),m_pIBuf(0),m_iTriCount(0),m_iVertCount(0)
{
	m_Transformation.SetPosition(Core::Vector3(128.0f,256.0f,128.0f));
	SetName("EditorSun");
}

EditorSun::~EditorSun()
{
	g_MeshMgr->ReleaseMesh(m_hMesh);
	g_ShaderMgr->ReleaseShader(m_hShader);
}

bool EditorSun::Draw()
{
	if(!m_pVBuf || !m_pIBuf) return false;

	const float fSun[4]={ m_Transformation.GetPosition().x, m_Transformation.GetPosition().y, m_Transformation.GetPosition().z,0.0f};
	g_Renderer->SetVertexShaderConstant(50,fSun);

	g_ShaderMgr->SetShader(m_hShader);
	g_Renderer->SetTransformation(TRANS_MODEL,m_Transformation.GetMatrix(),0,false);
	g_Renderer->SetVertexBuffer(m_pVBuf,m_iVertCount,sizeof(Core::VertexPNDTT));
	g_Renderer->SetIndexBuffer(m_pIBuf);
	g_Renderer->RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_iTriCount);

	return true;
}

bool EditorSun::Load(const char *szName, const char *szPath)
{
	m_hMesh=g_MeshMgr->LoadMesh(szName,szPath);
	if(!m_hMesh) return false;

	m_hShader=g_ShaderMgr->LoadShader("StdEditorSun");

	IMesh *pMesh=g_MeshMgr->GetMesh(m_hMesh);
	m_pVBuf=pMesh->GetVertexBuffer();
	m_pIBuf=pMesh->GetIndexBuffer();
	m_iTriCount=pMesh->GetTriCount();
	m_iVertCount=pMesh->GetVerticesCount();

	m_BSphere.FromBBox(&pMesh->GetBBox());

	return true;
}

void EditorSun::Translate(const Core::Vector3 &v)
{
	m_Transformation.Translate(v);
	m_Transformation.RebuildMatrix();
	Core::Vector3 vCenter;
	vCenter.x=vCenter.z=(float)g_Map->GetHeightmap()->GetSize() / 2;
	vCenter.y=0.0f;

	Core::Vector3 vDir(vCenter - m_Transformation.GetPosition());
	vDir.Normalize();

	if(!(g_Map->GetSun()->GetDirection()==vDir))
		g_Map->RebuildAllShadowVolumes();

	g_Map->GetSun()->SetDirection(vDir);

}