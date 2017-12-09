
#include "CollisionMgr.h"
#include "IObject.h"
#include "Map.h"
#include "../ColDet/coldetimpl.h"

namespace Game
{

HANDLE g_hCollisionThread=0;
uint32 g_dwCollisionThreadID=0;
bool g_bCollisionFinished=true;

CollisionMgr::CollisionMgr() : m_iCurrentExecBuffer(1), m_iCurrentFillBuffer(2)
{
}

CollisionMgr::~CollisionMgr()
{
}

void CollisionMgr::AddCollisionNode(IObject *pObj1, IObject *pObj2)
{
	m_ColBuffers[m_iCurrentFillBuffer].push_back(CollisionNode(pObj1,pObj2));
}

void CollisionMgr::AddCollisionNode(CollisionNode *pNodes, int iCount)
{
	for(int i=0;i<iCount;i++,pNodes++)
		m_ColBuffers[m_iCurrentFillBuffer].push_back(*pNodes);
}

DWORD WINAPI CollisionThread(LPVOID pParam)
{
	CRITICAL_SECTION Sync;

	InitializeCriticalSection(&Sync);
	EnterCriticalSection(&Sync);

	g_CollisionMgr.Execute();

	LeaveCriticalSection(&Sync);
	DeleteCriticalSection(&Sync);

	g_bCollisionFinished=true;

	return 0;
}

void CollisionMgr::ProcessCollisions()
{
	g_bCollisionFinished=false;
	g_hCollisionThread=CreateThread(NULL,NULL,CollisionThread,NULL,NULL,&g_dwCollisionThreadID);
}

void CollisionMgr::Execute()
{
	std::vector<CollisionNode>::iterator i;
	for(i=m_ColBuffers[m_iCurrentExecBuffer].begin();i!=m_ColBuffers[m_iCurrentExecBuffer].end();i++)
	{
		const ObjectType type=i->pObject2 ? i->pObject2->GetObjectType() : OBJ_PATCHGRID;
		if(type==OBJ_PATCHGRID || type==OBJ_SHIELD || type==OBJ_WATER)
		{
			if(CheckMapCollision(*i))
				m_MsgBuffer.push_back(*i);
		}
		else
		{
			CollisionModel3DImpl *pCol1=i->pObject1->GetCollisionData();
			CollisionModel3DImpl *pCol2=i->pObject2->GetCollisionData();

			if(pCol1 && pCol2)
			{
				pCol1->setTransform(*((const Matrix3D *)&i->pObject1->GetTransformation().GetMatrix()),(const Matrix3D*)&i->pObject1->GetTransformation().GetInverse());
				pCol2->setTransform(*((const Matrix3D *)&i->pObject2->GetTransformation().GetMatrix()),(const Matrix3D*)&i->pObject2->GetTransformation().GetInverse());

				if(pCol1->collision(pCol2,-1,0,0))
					m_MsgBuffer.push_back(*i);
			}
		}
	}
}

void CollisionMgr::ProcessMessages()
{
	if(!g_bCollisionFinished)
		WaitForSingleObject(g_hCollisionThread,INFINITE);

	if(m_iCurrentFillBuffer==0)
	{
		m_iCurrentFillBuffer=1;
		m_iCurrentExecBuffer=0;
	}
	else
	{
		m_iCurrentFillBuffer=0;
		m_iCurrentExecBuffer=1;
	}

	GameEvent e;
	e.SetType(GE_COLLIDE);

	std::vector<CollisionNode>::iterator i;
	for(i=m_MsgBuffer.begin();i!=m_MsgBuffer.end();i++)
	{
		e.SetPParam(i->pObject2);
		i->pObject1->OnEvent(e);

		if(i->pObject2)
		{
			e.SetPParam(i->pObject1);
			i->pObject2->OnEvent(e);
		}
	}

	m_ColBuffers[m_iCurrentFillBuffer].clear();
	m_MsgBuffer.clear();
}

bool CollisionMgr::CheckMapCollision(CollisionNode& node)
{
	const Core::Vector3& vPos(node.pObject1->GetTransformation().GetPosition());

	Core::Vector3 vDiff(vPos - g_Map.GetShield()->GetPosition());
	vDiff.Normalize();

	Core::Vector3 vSize(g_Map.GetShield()->GetSize());
	vSize.x*=vDiff.x; vSize.y*=vDiff.y; vSize.z*=vDiff.z;
	const float fRadius2=vSize.GetLength() - 5.0f * ((float)g_Map.GetHeightmap()->GetSize() / 256.0f);
	const float fDist2=(vPos-g_Map.GetShield()->GetPosition()).GetLength();

	if(fDist2 >= fRadius2)
	{
		node.pObject2=(IObject *)g_Map.GetShield();
		return true;
	}

	const float fMinHeight=vPos.y;
	if(fMinHeight <= g_Map.GetWater()->GetHeight())
	{
		node.pObject2=g_Map.GetWater();
		return true;
	}

	const float fMidRadius=node.pObject1->GetTransformedBSphere().GetRadius() * 0.5f;

	const int xmin=int(vPos.x - fMidRadius);
	const int xmax=int(vPos.x + fMidRadius);
	const int ymin=int(vPos.z - fMidRadius);
	const int ymax=int(vPos.z + fMidRadius);

	if(xmin >= 0 && xmax < g_Map.GetHeightmap()->GetSize() &&
	   ymin >= 0 && ymax < g_Map.GetHeightmap()->GetSize())
	{
		int i,j;
		for(i=xmin;i<=xmax;i++)
			for(j=ymin;j<=ymax;j++)
				if(fMinHeight < g_Map.GetHeightmap()->GetHeight(i,j))
				{
					node.pObject2=g_Map.GetPatchGrid();
					return true;
				}
	}

	return false;
}

}