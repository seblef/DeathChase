
#include "ObjectMap.h"
#include "Heightmap.h"
#include "MapObjectFactory.h"
#include "RenderingQueue.h"
#include "Octree.h"
#include "../Core/ViewFrustrum.h"
#include "../Core/MeshMgr.h"

#define OMAPFLAGS_UNDERWATER			1
#define OMAPFLAGS_USED					2

#define MAX_PLACEMENT_TRIES				200

namespace Game
{

ObjectMap::ObjectMap() : m_pMap(0),m_pHeightmap(0),
	m_iMapSize(0)
{
	new MapObjectFactory;
}

ObjectMap::~ObjectMap()
{
	Clear();
}

void ObjectMap::Clear()
{
	if(m_pMap) delete[] m_pMap;

	m_pMap=0;
	m_pHeightmap=0;
	m_iMapSize=0;
}

void ObjectMap::ClearMap()
{
}

bool ObjectMap::Generate(IHeightmap *pMap, int iMapRes, float fWaterHeight)
{
	if(!pMap || pMap->GetSize()<=0) return false;

	Clear();

	const int iSize=pMap->GetSize();

	m_pMap=new ObjMapEntry[iSize*iSize];
	memset(m_pMap,0,iSize*iSize*sizeof(ObjMapEntry));

	ObjMapEntry *pEntry=m_pMap;

	if(fWaterHeight > 0.0f)
	{
		for(int y=0;y<iSize;y++)
			for(int x=0;x<iSize;x++)
			{
				pEntry->fHeight=pMap->GetHeight(x,y);
				if(pEntry->fHeight <= fWaterHeight)
					pEntry->dwFlags=OMAPFLAGS_UNDERWATER;

				pEntry++;
			}
	}
	else
	{
		for(int y=0;y<iSize;y++)
			for(int x=0;x<iSize;x++)
			{
				pEntry->fHeight=pMap->GetHeight(x,y);
				pEntry++;
			}
	}

	m_iMapSize=iSize;
	m_pHeightmap=pMap;

	return true;
}

bool ObjectMap::UpdateHeightmap(int x, int y, int iWidth, int iHeight)
{
	if(!m_pHeightmap) return false;

	Core::Vector3 vTemp(Core::Vector3::NullVector);

	for(int j=y;j<(y+iHeight);j++)
	{
		ObjMapEntry *pEntry=m_pMap + j * m_iMapSize + x;
		for(int i=x;i<(x+iWidth);i++)
		{
			const float fNewHeight=m_pHeightmap->GetHeight(i,j);
			float fDiff=fNewHeight - pEntry->fHeight;
			if(fDiff!=0.0f && pEntry->pObject)
			{
				vTemp.y=fDiff;
				pEntry->pObject->GetTransformation().Translate(vTemp);
				pEntry->pObject->GetTransformation().RebuildMatrix();
				pEntry->pObject->UpdatePositionInOctree();
			}
		}
	}

	return true;
}

bool ObjectMap::AddObject(IMapObject *pObj, bool bOnGround)
{
	if(!pObj) return false;

	const int x=pObj->GetMapPos().x;
	const int y=pObj->GetMapPos().y;

	const int xstart=MAX(x-pObj->GetRadius(),0);
	const int ystart=MAX(y-pObj->GetRadius(),0);
	const int xend=MIN(x+pObj->GetRadius(),m_iMapSize-1);
	const int yend=MIN(y+pObj->GetRadius(),m_iMapSize-1);

	for(int j=ystart;j<=yend;j++)
	{
		ObjMapEntry *pEntry=m_pMap+j*m_iMapSize+xstart;
		for(int i=xstart;i<=xend;i++)
		{
			pEntry->dwFlags|=OMAPFLAGS_USED;
			pEntry->pObject=(MapObject *)pObj;
			pEntry++;
		}
	}

	if(bOnGround)
	{
		Core::Vector3 vPos(pObj->GetTransformation().GetPosition());
		vPos.y=m_pHeightmap->GetInterpolatedHeight(vPos.x,vPos.z) + pObj->GetHeight();
		pObj->GetTransformation().SetPosition(vPos);
	}

	pObj->GetTransformation().RebuildMatrix();

	g_Octree.InsertObject(pObj);

	return true;
}

bool ObjectMap::RemoveObject(IMapObject *pObj)
{
	if(!pObj) return false;

	const int x=pObj->GetMapPos().x;
	const int y=pObj->GetMapPos().y;

	const int xstart=MAX(x-pObj->GetRadius(),0);
	const int ystart=MAX(y-pObj->GetRadius(),0);
	const int xend=MIN(x+pObj->GetRadius(),m_iMapSize-1);
	const int yend=MIN(y+pObj->GetRadius(),m_iMapSize-1);

	float fMinHeight=256.0f;

	for(int j=ystart;j<=yend;j++)
	{
		ObjMapEntry *pEntry=m_pMap+j*m_iMapSize+xstart;
		for(int i=xstart;i<=xend;i++)
		{
			pEntry->dwFlags&= !OMAPFLAGS_USED;
			pEntry->pObject=0;
			pEntry++;
		}
	}

	pObj->RemoveFromOctree();

	return true;
}

std::vector<IMapObject *> *ObjectMap::GetObjectsFromPlace(int x, int y, int iRadius)
{
	if(!m_pMap) return 0;

	static std::vector<IMapObject*> sObjects;
	sObjects.clear();

	if(iRadius==1)
	{
		const int iIndex=y*m_iMapSize+x;
		if(m_pMap[iIndex].pObject)
			sObjects.push_back(m_pMap[iIndex].pObject);
	}
	else
	{
		const int xstart=MAX(x-iRadius,0);
		const int ystart=MAX(y-iRadius,0);
		const int xend=MIN(x+iRadius,m_iMapSize-1);
		const int yend=MIN(y+iRadius,m_iMapSize-1);

		MapObject *pLastObj=0;
		for(int j=ystart;j<=yend;j++)
		{
			ObjMapEntry *pEntry=m_pMap + j*m_iMapSize+xstart;
			for(int i=xstart;i<=xend;i++)
			{
				if(pEntry->pObject && pEntry->pObject!=pLastObj)
				{
					std::vector<IMapObject*>::iterator k;
					bool bAdded=false;
					for(k=sObjects.begin();k!=sObjects.end();k++)
						if((*k)==pEntry->pObject)
						{
							bAdded=true;
							break;
						}

					if(!bAdded)
					{
						sObjects.push_back(pEntry->pObject);
						pLastObj=pEntry->pObject;
					}
				}

				pEntry++;
			}
		}
	}

	return &sObjects;
}


bool ObjectMap::GetFreePlace(int iRadius, int &x, int &y)
{
	if(!m_pMap) return false;

	bool end=false;
	int count=0;

	while(1)
	{
		int tx=int(RAND * (float)(m_iMapSize-iRadius)) + iRadius / 2;
		int ty=int(RAND * (float)(m_iMapSize-iRadius)) + iRadius / 2;

		if(!IsPlaceUsed(tx,ty,iRadius))
		{
			x=tx;
			y=ty;
			return true;
		}

		if((count++) > MAX_PLACEMENT_TRIES)
			break;
	}

	return false;
}

bool ObjectMap::IsPlaceUsed(int x, int y, int iRadius)
{
	if(!m_pMap) return true;

	int xmin=MAX(x-iRadius,0),xmax=MIN(m_iMapSize-1,x+iRadius);
	int ymin=MAX(y-iRadius,0),ymax=MIN(m_iMapSize-1,y+iRadius);

	int i,j;
	for(j=ymin;j<=ymax;j++)
	{
		ObjMapEntry *pEntry=m_pMap + j*m_iMapSize+xmin;
		for(i=xmin;i<=xmax;i++)
		{
			if((pEntry->dwFlags & OMAPFLAGS_USED) ||
			   (pEntry->dwFlags & OMAPFLAGS_UNDERWATER))
				return true;
			pEntry++;
		}
	}

	return false;
}

bool ObjectMap::IsUnderWater(int x, int y, int iRadius)
{
	if(!m_pMap) return true;

	int xmin=MAX(x-iRadius,0),xmax=MIN(m_iMapSize-1,x+iRadius);
	int ymin=MAX(y-iRadius,0),ymax=MIN(m_iMapSize-1,y+iRadius);

	int i,j;
	for(j=ymin;j<=ymax;j++)
	{
		ObjMapEntry *pEntry=m_pMap + j*m_iMapSize+xmin;
		for(i=xmin;i<=xmax;i++)
		{
			if(pEntry->dwFlags & OMAPFLAGS_UNDERWATER)
				return true;
			pEntry++;
		}
	}

	return false;
}

}