
#include "Shield.h"
#include "../Core/TextureMgr.h"
#include "../Core/ShaderMgr.h"
#include "../Core/Mesh.h"
#include "../Core/Renderer.h"
#include "../Core/FileSystem.h"


namespace Game
{

Shield::Shield() : IObject(OBJ_SHIELD),
	m_bEnable(false),
	m_pVBuf(0),m_pIBuf(0),
	m_dwVertCount(0),m_dwTriCount(0),
	m_mWorld(Core::Matrix4::Identity),
	m_vPosition(Core::Vector3::NullVector),
	m_vSize(256.0f,256.0f,256.0f),
	m_dwVisDist(0)
{
	m_bStatic=true;
	m_bLightPassEnable=false;
	m_bSpecialPassEnable=false;
	m_bCastShadows=false;
	m_bReceiveShadows=false;
	m_bVisible=true;

	SetName("Shield");
}

Shield::~Shield()
{
	Clear();
}

void Shield::Clear()
{
	g_TextureMgr.ReleaseTexture(m_hTexture);
	g_ShaderMgr.ReleaseShader(m_hShader);
	g_ShaderMgr.ReleaseShader(m_hNoTexShader);

	if(m_pIBuf) m_pIBuf->Release();
	if(m_pVBuf) m_pVBuf->Release();

	m_pIBuf=0;
	m_pVBuf=0;

	m_bEnable=false;
}

bool Shield::Create(const Core::Vector3& vSize,
					int iMapSize,
					uint32 dwVisDist,
					Core::HTexture hTex)
{
	if(vSize.x==0.0f || vSize.y==0.0f || vSize.z==0.0f) return false;

	Clear();

	m_hShader=g_ShaderMgr.LoadShader("StdShield");
	m_hNoTexShader=g_ShaderMgr.LoadShader("StdShieldNoTex");

	g_FileSystem.ResetWorkingDirectory();
	g_FileSystem.ChangeWorkingDirectory("Models\\Shield\\");

	std::string strPath(g_FileSystem.GetWorkingDirectory());
	strPath+="\\";

	Core::Mesh *pMesh=Core::Mesh::GetNew();
	if(!pMesh->Load("shield.dgeo",strPath.c_str()))
	{
		Core::Mesh::Delete(pMesh);
		return false;
	}

	const int iVertCount=pMesh->GetVerticesCount();
	const int iTriCount=pMesh->GetTriCount();

	void *pVert;
	if(pMesh->GetVertexBuffer()->Lock(0,iVertCount * Core::VertexSizeTable[Core::VX_PNDTT],&pVert,0)!=D3D_OK)
	{
		Core::Mesh::Delete(pMesh);
		return false;
	}

	m_pVBuf=g_Renderer.CreateVertexBuffer(pVert,iVertCount,Core::VX_PNDTT);
	if(!m_pVBuf)
	{
		pMesh->GetVertexBuffer()->Unlock();
		Core::Mesh::Delete(pMesh);
		return false;
	}

	pMesh->GetVertexBuffer()->Unlock();

	void *pInd;
	if(pMesh->GetIndexBuffer()->Lock(0,iTriCount*3*sizeof(uint16),&pInd,0)!=D3D_OK)
	{
		m_pVBuf->Release(); m_pVBuf=0;
		Core::Mesh::Delete(pMesh);
		return false;
	}

	m_pIBuf=g_Renderer.CreateIndexBuffer(pInd,iTriCount*3,D3DFMT_INDEX16);
	if(!m_pIBuf)
	{
		pMesh->GetIndexBuffer()->Unlock();
		m_pVBuf->Release(); m_pVBuf=0;
		Core::Mesh::Delete(pMesh);
		return false;
	}

	pMesh->GetIndexBuffer()->Unlock();
	Core::Mesh::Delete(pMesh);

	m_dwVertCount=iVertCount;
	m_dwTriCount=iTriCount;
	m_dwVisDist=dwVisDist;

	m_hTexture=hTex;
	m_bEnable=true;

	m_vPosition.x=0.5f * (float)iMapSize;
	m_vPosition.y=0.0f;
	m_vPosition.z=0.5f * (float)iMapSize;

	m_mWorld.m[3][0]=m_vPosition.x;
	m_mWorld.m[3][1]=m_vPosition.y;
	m_mWorld.m[3][2]=m_vPosition.z;

	SetSize(vSize);

	return true;
}

void Shield::SetTexture(Core::HTexture hTex)
{
	g_TextureMgr.ReleaseTexture(m_hTexture);
	m_hTexture=hTex;
}

void Shield::SetShader(Core::HShader hShader)
{
	if(!hShader.IsNull())
	{
		g_ShaderMgr.ReleaseShader(m_hShader);
		m_hShader=hShader;
	}
}

void Shield::SetSize(const Core::Vector3& vSize)
{
	if(vSize.x!=0.0f && vSize.y!=0.0f && vSize.z!=0.0f)
	{
		m_mWorld.m[0][0]=vSize.x;
		m_mWorld.m[1][1]=vSize.y;
		m_mWorld.m[2][2]=vSize.z;

		m_vSize=vSize;
	}
}

bool Shield::Draw()
{
	if(!m_bEnable || !m_pVBuf || !m_pIBuf) return false;

	if(m_dwVisDist > 0)
	{
		Core::Vector3 vDir(m_vCamPos - m_vPosition);
		const float fDistFromCenter=vDir.GetLength();
		vDir*=1.0f / fDistFromCenter; //  Normalization

		vDir.x*=m_vSize.x;
		vDir.y*=m_vSize.y;
		vDir.z*=m_vSize.z;
		vDir*=0.5f;

		const float fDistFromEdgeSq=vDir.GetSquareLength() - fDistFromCenter*fDistFromCenter;
		if(fDistFromEdgeSq > (float)(m_dwVisDist * m_dwVisDist))
			return false;
	}

	const float fDistFact=m_dwVisDist==0 ? 1e15 : 0.7f / (float)m_dwVisDist;
	const float fConstant[4]={m_vCamPos.x,m_vCamPos.y,m_vCamPos.z,fDistFact};

	g_Renderer.SetTransformation(TRANS_MODEL,m_mWorld,0,false);
	g_Renderer.SetVertexDeclaration(Core::VX_PNDTT);
	g_Renderer.SetVertexShaderConstant(SHIELD_CAMPOS_REG,fConstant,1);

	if(m_hTexture.IsNull())
		g_ShaderMgr.SetShader(m_hNoTexShader);
	else
	{
		g_ShaderMgr.SetShader(m_hShader);
		g_TextureMgr.SetTexture(0,m_hTexture);
		g_TextureMgr.SetTexture(1,m_hTexture);
	}

	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	g_Renderer.SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_Renderer.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	g_Renderer.SetVertexBuffer(m_pVBuf,m_dwVertCount,Core::VertexSizeTable[Core::VX_PNDTT]);
	g_Renderer.SetIndexBuffer(m_pIBuf);
	g_Renderer.RenderStaticBuffers(D3DPT_TRIANGLELIST,0,m_dwTriCount);

	g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

	return true;
}

void Shield::OnEvent(const GameEvent& Event)
{
}

}