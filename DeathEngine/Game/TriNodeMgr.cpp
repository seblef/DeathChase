
#include "TriNodeMgr.h"

namespace Game
{

TriNodeMgr *TriNodeMgr::m_pInstance=0;

TriNodeMgr *TriNodeMgr::Instance()
{
	if(!m_pInstance)
		m_pInstance=new TriNodeMgr;

	return m_pInstance;
}

TriNodeMgr::TriNodeMgr()
{
	m_pNodeCount=m_pNodes=new TriNode[100000];
	m_pMaxCount=&m_pNodes[100000-1];
}

TriNodeMgr::~TriNodeMgr()
{
	if(m_pNodes)
		delete[] m_pNodes;
}

TriNode *TriNodeMgr::GetNextTriNode()
{
	if(m_pNodeCount==m_pMaxCount)
		return 0;

	TriNode *pTri=m_pNodeCount++;
	pTri->LeftChild=pTri->RightChild=0;

	return pTri;
}

void TriNodeMgr::Reset()
{
	m_pNodeCount=m_pNodes;
}

}