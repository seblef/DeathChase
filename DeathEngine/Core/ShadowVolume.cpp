
#include "ShadowVolume.h"
#include "EdgeSet.h"

namespace Core
{

ShadowVolume::ShadowVolume() : m_pVBuf(0), m_pIBuf(0), m_iVertCount(0), m_iTriCount(0),m_iEdgesCount(0)
{
}

ShadowVolume::~ShadowVolume()
{
	if(m_pVBuf) delete[] m_pVBuf;
	if(m_pIBuf) delete[] m_pIBuf;
}

void ShadowVolume::Clear()
{
	if(m_pVBuf) delete[] m_pVBuf;
	if(m_pIBuf) delete[] m_pIBuf;

	m_pVBuf=0;
	m_pIBuf=0;

	m_iTriCount=0;
	m_iVertCount=0;
	m_iEdgesCount=0;
}

bool ShadowVolume::GenerateFromDirLight(EdgeSet *pEdges, const Vector3 &vLightDir,
										const Matrix4 &mInvWorld)
{
	if(!Create(pEdges)) return false;

	Vector3 *pVert=(Vector3 *)m_pVBuf;
	uint16 *pInd=m_pIBuf;

	Core::Matrix4 mRot(mInvWorld);
	mRot.m[3][0]=0.0f; mRot.m[3][1]=0.0f; mRot.m[3][2]=0.0f;

	Vector3 vDir(vLightDir * mRot);
	vDir.Normalize();

	EdgeSet::Edge *e;
	int e_count;
	if(!pEdges->BuildSilhouetteFromDirLight(vDir,e_count,&e))
		return false;

	m_iVertCount=0;
	m_iTriCount=0;

	vDir*=256.0f;
	for(int i=0;i<e_count;i++,e++)
	{
		pVert[0]=e->v1;
		pVert[1]=e->v2;
		pVert[2]=e->v1 + vDir;
		pVert[3]=e->v2 + vDir;

		pInd[0]=m_iVertCount;
		pInd[1]=m_iVertCount+1;
		pInd[2]=m_iVertCount+2;

		pInd[3]=m_iVertCount+1;
		pInd[4]=m_iVertCount+3;
		pInd[5]=m_iVertCount+2;

		m_iVertCount+=4;
		m_iTriCount+=2;

		pVert+=4;
		pInd+=6;
	}

	return true;
}

bool ShadowVolume::GenerateFromPointLight(EdgeSet *pEdges, const Vector3& vLightPos,
										  const Matrix4& mInvWorld)
{
	return true;
}

bool ShadowVolume::Create(EdgeSet *pEdges)
{
	if(!pEdges || pEdges->GetCount()==0)
		return false;

	if(pEdges->GetCount() > m_iEdgesCount)
	{
		Clear();

		int v_count=pEdges->GetCount() * 4;
		int t_count=pEdges->GetCount() * 2;

		m_pVBuf=new VertexP[v_count];
		m_pIBuf=new uint16[t_count*3];

		m_iEdgesCount=pEdges->GetCount();
	}

	return true;
}

}