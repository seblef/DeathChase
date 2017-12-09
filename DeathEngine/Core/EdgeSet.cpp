
#include "EdgeSet.h"

#define MIN_EDGE_COLLAPSE				0,86602f // cos(30)


namespace Core
{

void EdgeSet::Clear()
{
	if(m_pEdges) delete[] m_pEdges;
	if(m_pSilhouette) delete[] m_pSilhouette;

	m_pEdges=0;
	m_dwEdgesCount=0;

	m_pSilhouette=0;
	m_iSilhouetteEdgesCount=0;
}

bool EdgeSet::Build(int iVertCount, VertexPNDTT *pVert,
					int iTriCount, uint16 *pInd)
{
	if(iVertCount==0 || iTriCount==0 ||
		pVert==0 || pInd==0) return false;

	Clear();

	m_pEdges=new Edge[iTriCount*3];
	m_pSilhouette=new Edge[iTriCount*3];

	Edge *pEdge=m_pEdges;
	uint16 *ind=pInd;
	for(int i=0;i<iTriCount;i++)
	{
		const Vector3 v0(pVert[ind[0]].vPos);
		const Vector3 v1(pVert[ind[1]].vPos);
		const Vector3 v2(pVert[ind[2]].vPos);

		Vector3 vNorm((v2-v1) ^ (v1 - v0));
		vNorm.Normalize();

		pEdge[0]=Edge(v0,v1,ind[0],ind[1],vNorm);
		pEdge[1]=Edge(v1,v2,ind[1],ind[2],vNorm);
		pEdge[2]=Edge(v2,v0,ind[2],ind[0],vNorm);

		ind+=3;
		pEdge+=3;
	}

	m_dwEdgesCount=iTriCount*3;

	return true;
}

void EdgeSet::LoadFromFile(FILE *pFile)
{
	Clear();

	uint32 dwEdgesCount;
	fread(&dwEdgesCount,sizeof(uint32),1,pFile);

	if(dwEdgesCount > 0)
	{
		m_pEdges=new Edge[dwEdgesCount];
		m_pSilhouette=new Edge[dwEdgesCount];
		fread(m_pEdges,dwEdgesCount * sizeof(Edge),1,pFile);
		m_dwEdgesCount=dwEdgesCount;
	}
}

void EdgeSet::SaveToFile(FILE *pFile)
{
	fwrite(&m_dwEdgesCount,sizeof(uint32),1,pFile);

	if(m_dwEdgesCount > 0)
		fwrite(m_pEdges,m_dwEdgesCount * sizeof(Edge),1,pFile);
}

bool EdgeSet::BuildSilhouetteFromDirLight(const Vector3 &vLightDir, int &iEdgesCount,
										  Edge **ppEdges)
{
	if(!m_pEdges || !m_pSilhouette) return false;

	int iCount=0;
	Edge *e=m_pEdges;
	for(uint32 i=0;i<m_dwEdgesCount;i++,e++)
	{
		if((e->vNormal * vLightDir) <= 0.0f)
			AddEdge(e,iCount);
	}

	m_iSilhouetteEdgesCount=iCount;
	iEdgesCount=iCount;
	*ppEdges=m_pSilhouette;

	return true;
}

bool EdgeSet::BuildSilhouetteFromPointLight(const Vector3 &vLightPos, int &iEdgesCount, Edge **ppEdges)
{
	return true;
}

void EdgeSet::AddEdge(Edge *pEdge, int& iCount)
{
	for(int i=0;i<iCount;i++)
	{
		if((m_pSilhouette[i].id1==pEdge->id1 && m_pSilhouette[i].id2==pEdge->id2) ||
		   (m_pSilhouette[i].id1==pEdge->id2 && m_pSilhouette[i].id2==pEdge->id1))
		{
			if(iCount > 1)
				m_pSilhouette[i]=m_pSilhouette[iCount-1];

			iCount--;
			return;
		}
	}

	m_pSilhouette[iCount]=*pEdge;
	iCount++;
}

}
