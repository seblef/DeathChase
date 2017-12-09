
#include "SObject.h"
#include "../DeathEditor/Interfaces.h"
#include "../DeathEngine/Export/IMesh.h"
#include "../DeathEngine/Export/IMeshMgr.h"
#include "../DeathEngine/Export/IMaterial.h"
#include "../DeathEngine/Export/IMaterialMgr.h"
#include "../DeathEngine/Export/IShaderMgr.h"


const Core::VertexPNDTT CubeVertices[12]={
	Core::VertexPNDTT(Core::Vector3(-1.0f,-1.0f,-1.0f), Core::Vector3(-1.0f,-1.0f,-1.0f),Core::Color32(255,255,255,255),Core::Vector2(0.0f,1.0f),Core::Vector2(0.0f,1.0f)),
	Core::VertexPNDTT(Core::Vector3( 1.0f,-1.0f,-1.0f), Core::Vector3( 1.0f,-1.0f,-1.0f),Core::Color32(255,255,255,255),Core::Vector2(1.0f,1.0f),Core::Vector2(1.0f,1.0f)),
	Core::VertexPNDTT(Core::Vector3( 1.0f, 1.0f,-1.0f), Core::Vector3( 1.0f, 1.0f,-1.0f),Core::Color32(255,255,255,255),Core::Vector2(1.0f,0.0f),Core::Vector2(1.0f,0.0f)),
	Core::VertexPNDTT(Core::Vector3(-1.0f, 1.0f,-1.0f), Core::Vector3(-1.0f, 1.0f,-1.0f),Core::Color32(255,255,255,255),Core::Vector2(0.0f,0.0f),Core::Vector2(0.0f,0.0f)),
	Core::VertexPNDTT(Core::Vector3( 1.0f,-1.0f, 1.0f), Core::Vector3( 1.0f,-1.0f, 1.0f),Core::Color32(255,255,255,255),Core::Vector2(0.0f,1.0f),Core::Vector2(0.0f,1.0f)),
	Core::VertexPNDTT(Core::Vector3( 1.0f, 1.0f, 1.0f), Core::Vector3( 1.0f, 1.0f, 1.0f),Core::Color32(255,255,255,255),Core::Vector2(0.0f,0.0f),Core::Vector2(0.0f,0.0f)),
	Core::VertexPNDTT(Core::Vector3(-1.0f, 1.0f, 1.0f), Core::Vector3(-1.0f, 1.0f, 1.0f),Core::Color32(255,255,255,255),Core::Vector2(1.0f,0.0f),Core::Vector2(1.0f,0.0f)),
	Core::VertexPNDTT(Core::Vector3(-1.0f,-1.0f, 1.0f), Core::Vector3(-1.0f,-1.0f, 1.0f),Core::Color32(255,255,255,255),Core::Vector2(1.0f,1.0f),Core::Vector2(1.0f,1.0f)),
	Core::VertexPNDTT(Core::Vector3(-1.0f, 1.0f, 1.0f), Core::Vector3(-1.0f, 1.0f, 1.0f),Core::Color32(255,255,255,255),Core::Vector2(0.0f,1.0f),Core::Vector2(0.0f,1.0f)),
	Core::VertexPNDTT(Core::Vector3( 1.0f,-1.0f, 1.0f), Core::Vector3( 1.0f,-1.0f, 1.0f),Core::Color32(255,255,255,255),Core::Vector2(1.0f,1.0f),Core::Vector2(1.0f,1.0f)),
	Core::VertexPNDTT(Core::Vector3( 1.0f,-1.0f, 1.0f), Core::Vector3( 1.0f,-1.0f, 1.0f),Core::Color32(255,255,255,255),Core::Vector2(1.0f,0.0f),Core::Vector2(1.0f,0.0f)),
	Core::VertexPNDTT(Core::Vector3( 1.0f,-1.0f,-1.0f), Core::Vector3( 1.0f,-1.0f,-1.0f),Core::Color32(255,255,255,255),Core::Vector2(0.0f,0.0f),Core::Vector2(0.0f,0.0f)),
};

/*const uint16 CubeIndices[36] = {   0,1,2,   0,2,3,   1,4,5,   1,5,2,   4,7,6,   4,6,5, 
            7,0,3,   7,3,6,   9,2,5,   9,5,8,   0,10,11,   0,7,10};*/

const uint16 CubeIndices[36] = {   0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6, 
            7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7};

const Core::Color32 ObjectColor[SOBJ_COUNT]=
{
	Core::Color32(255,255,255,255),
	Core::Color32(255,255,  0,  0),
	Core::Color32(255,  0,255,  0),
	Core::Color32(255,  0,  0,255),
	Core::Color32(255,  0,  0,  0),
	Core::Color32(255,  0,255,255)

};

Core::HMesh SObject::m_shTemplates[SOBJ_COUNT-1];

SObject::SObject(SObjectType _Type) : m_Type(_Type), m_bTemplate(false), m_pColData(0)
{
	strcpy(m_szName,"");
	m_Transformation.SetBSpheres(&m_OriginalBSphere,&m_TransBSphere);
}

SObject::~SObject()
{
	if(!m_bTemplate)
		g_MeshMgr->ReleaseMesh(m_hMesh);
}

void SObject::Clear()
{
	if(!m_bTemplate)
		g_MeshMgr->ReleaseMesh(m_hMesh);

	m_hMesh=Core::HMesh();
	strcpy(m_szName,"");

	m_Transformation.Initialize();
	m_Transformation.SetBSpheres(&m_OriginalBSphere,&m_TransBSphere);

	m_pColData=0;

	m_bTemplate=false;
}

void SObject::Draw()
{
	g_MeshMgr->DrawMesh(m_hMesh,m_Transformation.GetMatrix(),&m_Transformation.GetInverse());
}

void SObject::Transform()
{
	if(m_bTemplate) return;

	IMesh *pMesh=g_MeshMgr->GetMesh(m_hMesh);
	if(!pMesh) return;

	IDirect3DVertexBuffer9 *pVBuf=pMesh->GetVertexBuffer();
	const int iVCount=pMesh->GetVerticesCount();

	if(!pVBuf || iVCount < 1) return;

	Core::VertexPNDTT *pVert;
	if(pVBuf->Lock(0,iVCount*sizeof(Core::VertexPNDTT),(void **)&pVert,0)!=D3D_OK) return;

	const Core::Matrix4& mTrans(m_Transformation.GetMatrix());
	Core::Matrix4 mRot;
	m_Transformation.GetRotation().GetMatrix(mRot);

	for(int i=0;i<iVCount;i++,pVert++)
	{
		pVert->vPos*=mTrans;
		pVert->vNorm*=mRot;
	}

	pVBuf->Unlock();

	m_Transformation.Initialize();
}

void SObject::SetMesh(Core::HMesh hMesh)
{
	if(!m_bTemplate && !m_hMesh.IsNull())
		g_MeshMgr->ReleaseMesh(m_hMesh);

	m_pColData=0;
	m_hMesh=hMesh;
	IMesh *pMesh=g_MeshMgr->GetMesh(hMesh);
	if(pMesh)
	{
		m_OriginalBSphere.FromBBox(&pMesh->GetBBox());
		m_pColData=pMesh->GetCollisionData();
	}

	m_Transformation.Initialize();
}

void SObject::ConstructMesh()
{
	if(m_Type <= SOBJ_SPACESHIP || m_Type >= SOBJ_COUNT) return;
	if(m_shTemplates[m_Type-1].IsNull())
	{
		Core::HMesh hMesh=g_MeshMgr->CreateEmptyMesh(SObjectNames[m_Type]);
		IMesh *pMesh=g_MeshMgr->GetMesh(hMesh);

		if(!pMesh) return;

		Core::VertexPNDTT vVert[12];
		for(int i=0;i<12;i++)
		{
			vVert[i].vPos=CubeVertices[i].vPos*0.1f;
			vVert[i].vNorm=CubeVertices[i].vNorm;
			vVert[i].vUV0=vVert[i].vUV1=CubeVertices[i].vUV0;
			vVert[i].cColor=ObjectColor[m_Type];
		}

		Core::HMaterial hMat=g_MaterialMgr->GetMaterialByName(SObjectNames[m_Type]);
		if(hMat.IsNull())
		{
			hMat=g_MaterialMgr->CreateEmptyMaterial(SObjectNames[m_Type]);
			IMaterial *pMat=g_MaterialMgr->GetMaterial(hMat);

			if(!pMat) return;

			pMat->SetDiffuseColor(ObjectColor[m_Type]);
			pMat->SetShader(RENDERINGPASS_AMBIENT,g_ShaderMgr->LoadShader("StdAmbient_NoTex"));
			pMat->SetShader(RENDERINGPASS_SUN,g_ShaderMgr->LoadShader("StdSun_NoTex"));
		}
			
		ISubMesh *pSub=g_MeshMgr->CreateNewSubMesh();
		pSub->Construct(0,12,hMat);

		pMesh->Construct(SObjectNames[m_Type],12,vVert,12,(uint16*)CubeIndices,pSub,0);

		m_shTemplates[m_Type-1]=hMesh;
	}

	IMesh *pMesh=g_MeshMgr->GetMesh(m_shTemplates[m_Type-1]);
	if(pMesh)
		m_pColData=pMesh->GetCollisionData();
	else
		m_pColData=0;

	m_OriginalBSphere.SetCenter(Core::Vector3::NullVector);
	m_OriginalBSphere.SetRadius(1.0f);

	m_hMesh=m_shTemplates[m_Type-1];
	m_bTemplate=true;
}