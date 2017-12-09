
#include "ParticleRendererStd.h"
#include "Particle.h"
#include "MaterialMgr.h"
#include "TextureMgr.h"
#include "Renderer.h"
#include "MeshMgr.h"
#include "Camera.h"
#include "ScriptFile.h"

namespace Core
{

TMemoryAllocator<ParticleRenderer_Point> ParticleRenderer_Point::m_sAllocator("ParticleRenderer_Point");
TMemoryAllocator<ParticleRenderer_Quad> ParticleRenderer_Quad::m_sAllocator("ParticleRenderer_Quad");
TMemoryAllocator<ParticleRenderer_Mesh> ParticleRenderer_Mesh::m_sAllocator("ParticleRenderer_Mesh");

ParticleRenderer_Point::ParticleRenderer_Point(bool bInstance) : m_bInstance(bInstance)
{
	if(!m_bInstance)
		m_hMaterial=g_MaterialMgr.LoadMaterial("StdParticle_Point.dmat");
}

ParticleRenderer_Point::~ParticleRenderer_Point()
{
	if(!m_bInstance)
		g_MaterialMgr.ReleaseMaterial(m_hMaterial);
}

void ParticleRenderer_Point::Draw(Particle *pParticles, int iCount, Camera *pCamera)
{
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
		if(pPart->m_fLife > 0.0f)
		{
			m_Tab.Add(VertexPD(pPart->m_vPosition,Color32((byte)pPart->m_cColor.fa,(byte)pPart->m_cColor.fr,
				(byte)pPart->m_cColor.fg,(byte)pPart->m_cColor.fb)));
		}

	if(m_Tab.GetItemCount() > 0)
	{
		g_MaterialMgr.SetMaterial(m_hMaterial);
		g_Renderer.SetTransformation(TRANS_MODEL,Matrix4::Identity,0,false);
		g_Renderer.SetVertexDeclaration(VX_PD);
		g_Renderer.RenderDynamicBuffers(D3DPT_POINTLIST,m_Tab.GetItemCount(),m_Tab.GetBuffer(),VX_PD,0,0,D3DFMT_INDEX16);

		m_Tab.Clear();
	}
}

IParticleRenderer *ParticleRenderer_Point::New()
{
	ParticleRenderer_Point *pRenderer=m_sAllocator.New();
	pRenderer->m_bInstance=true;

	pRenderer->m_hMaterial=m_hMaterial;
	return pRenderer;
}




ParticleRenderer_Quad::~ParticleRenderer_Quad()
{
	if(!m_bInstance)
		g_MaterialMgr.ReleaseMaterial(m_hMaterial);
}

void ParticleRenderer_Quad::Draw(Particle *pParticles, int iCount, Camera *pCamera)
{
	VertexPDT v[4];
	uint16 ind[6];
	uint16 iOffset=0;

	v[0].vUV.x=1.0f; v[0].vUV.y=1.0f;
	v[1].vUV.x=0.0f; v[1].vUV.y=1.0f;
	v[2].vUV.x=1.0f; v[2].vUV.y=0.0f;
	v[3].vUV.x=0.0f; v[3].vUV.y=0.0f;

	const Vector3& vX(pCamera->GetXAxis());
	const Vector3& vY(pCamera->GetYAxis());

	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
		if(pPart->m_fLife > 0.0f)
		{
			const Color32 cColor((byte)pPart->m_cColor.fa,(byte)pPart->m_cColor.fr,(byte)pPart->m_cColor.fg,(byte)pPart->m_cColor.fb);
			v[0].vPos=pPart->m_vPosition + (vX+vY) * pPart->m_fSize;
			v[1].vPos=pPart->m_vPosition + (vY-vX) * pPart->m_fSize;
			v[2].vPos=pPart->m_vPosition + (vX-vY) * pPart->m_fSize;
			v[3].vPos=pPart->m_vPosition - (vX+vY) * pPart->m_fSize;
			v[0].cColor=v[1].cColor=v[2].cColor=v[3].cColor=cColor;

			ind[0]=iOffset;
			ind[1]=iOffset+1;
			ind[2]=iOffset+2;

			ind[3]=iOffset+2;
			ind[4]=iOffset+1;
			ind[5]=iOffset+3;

			m_Vertices.AddTab(v,4);
			m_Indices.AddTab(ind,6);

			iOffset+=4;
		}

	if(m_Vertices.GetItemCount() > 0)
	{
		g_MaterialMgr.SetMaterial(m_hMaterial);
		g_Renderer.SetVertexDeclaration(VX_PDT);
		g_Renderer.SetTransformation(TRANS_MODEL,Matrix4::Identity,0,false);
		g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		g_Renderer.RenderDynamicBuffers(D3DPT_TRIANGLELIST,m_Vertices.GetItemCount(),m_Vertices.GetBuffer(),
			VX_PDT,m_Indices.GetItemCount(),m_Indices.GetBuffer(),D3DFMT_INDEX16);

		g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		m_Vertices.Clear();
		m_Indices.Clear();
	}
}

bool ParticleRenderer_Quad::Parse(ScriptFile *pFile)
{
	const char *szToken=pFile->GetToken();

	if(strcmp(szToken,"{")!=0)
	{
		g_Output->AddMsg("x Error [%s]: { attendu.\n",pFile->GetFileName());
		return false;
	}

	szToken=pFile->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"material")==0)
		{
			szToken=pFile->GetToken();
			SetMaterial(g_MaterialMgr.LoadMaterial(szToken));
		}
		else if(strcmp(szToken,"size")==0)
		{
			m_fSize=atof(pFile->GetToken());
		}
		else
			g_Output->AddMsg("x Error [%s]: Token %s inconnu.\n",pFile->GetFileName(),szToken);

		szToken=pFile->GetToken();
	}

	return true;
}

void ParticleRenderer_Quad::Clear()
{
	if(!m_bInstance)
		g_MaterialMgr.ReleaseMaterial(m_hMaterial);
}

void ParticleRenderer_Quad::SetMaterial(HMaterial hMaterial)
{
	if(!m_bInstance && !m_hMaterial.IsNull())
		g_MaterialMgr.ReleaseMaterial(m_hMaterial);

	m_hMaterial=hMaterial;
}

IParticleRenderer *ParticleRenderer_Quad::New()
{
	ParticleRenderer_Quad *pRenderer=m_sAllocator.New();
	pRenderer->m_bInstance=true;

	pRenderer->m_hMaterial=m_hMaterial;
	pRenderer->m_fSize=m_fSize;

	return pRenderer;
}




ParticleRenderer_Mesh::~ParticleRenderer_Mesh()
{
	if(!m_bInstance)
		g_MeshMgr.ReleaseMesh(m_hMesh);
}

void ParticleRenderer_Mesh::Draw(Particle *pParticles, int iCount, Camera *pCamera)
{
	if(m_hMesh.IsNull()) return;

	Matrix4 mTrans(Matrix4::Identity);
	Particle *pPart=pParticles;
	for(int i=0;i<iCount;i++,pPart++)
		if(pPart->m_fLife > 0.0f)
		{
			mTrans.m[0][0]=pPart->m_fSize;
			mTrans.m[1][1]=pPart->m_fSize;
			mTrans.m[2][2]=pPart->m_fSize;

			mTrans.m[3][0]=pPart->m_vPosition.x;
			mTrans.m[3][1]=pPart->m_vPosition.y;
			mTrans.m[3][2]=pPart->m_vPosition.z;

			g_MeshMgr.DrawMesh(m_hMesh,mTrans);
		}
}

bool ParticleRenderer_Mesh::Parse(ScriptFile *pFile)
{
	const char *szToken=pFile->GetToken();

	if(strcmp(szToken,"{")!=0)
	{
		g_Output->AddMsg("x Error [%s]: { attendu.\n",pFile->GetFileName());
		return false;
	}

	szToken=pFile->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"mesh")==0)
		{
			szToken=pFile->GetToken();
			SetMesh(g_MeshMgr.LoadMesh(szToken,0));
		}
		else if(strcmp(szToken,"size")==0)
		{
			m_fSize=atof(pFile->GetToken());
		}
		else
			g_Output->AddMsg("x Error [%s]: Token %s inconnu.\n",pFile->GetFileName(),szToken);

		szToken=pFile->GetToken();
	}

	return true;
}

void ParticleRenderer_Mesh::SetMesh(HMesh hMesh)
{
	if(!m_bInstance && !m_hMesh.IsNull())
		g_MeshMgr.ReleaseMesh(m_hMesh);

	m_hMesh=hMesh;
}

void ParticleRenderer_Mesh::Clear()
{
	if(!m_bInstance)
		g_MeshMgr.ReleaseMesh(m_hMesh);
}

IParticleRenderer *ParticleRenderer_Mesh::New()
{
	ParticleRenderer_Mesh *pRenderer=m_sAllocator.New();

	pRenderer->m_bInstance=true;
	pRenderer->m_hMesh=m_hMesh;
	pRenderer->m_fSize=m_fSize;

	return pRenderer;
}

}