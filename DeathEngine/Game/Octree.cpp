
#include "Octree.h"
#include "IObject.h"
#include "RenderingQueue.h"
#include "CollisionMgr.h"
#include "ObjectMap.h"

namespace Game
{

OctreeNode::OctreeNode() : m_pParent(0), m_iObjectCount(0),m_iEmpty(0),m_iChildFlag(0)
{
	for(int i=0;i<8;i++)
		m_pChildren[i]=0;
}

void OctreeNode::Clear()
{
	if(m_pChildren[0])
	{
		for(int i=0;i<8;i++)
		{
			delete m_pChildren[i];
			m_pChildren[i]=0;
		}
	}

	m_pParent=0;
	m_iObjectCount=0;
	m_iEmpty=0;
	m_iChildFlag=0;
}

bool OctreeNode::InsertObject(IObject *pObj)
{
	if(m_pChildren[0])
	{
		const Core::BSphere& sphere=pObj->GetTransformedBSphere();

		for(int i=0;i<8;i++)
			if(m_pChildren[i]->m_BBox.IsIn(&sphere,true))
				return m_pChildren[i]->InsertObject(pObj);
	}

	int iIndex;
	
	if(m_FreeIndex.size() > 0)
	{
		iIndex=m_FreeIndex[m_FreeIndex.size()-1];
		m_Objects[iIndex].pObject=pObj;
		m_Objects[iIndex].bUsed=true;
		m_FreeIndex.pop_back();
	}
	else
	{
		iIndex=m_Objects.size();
		m_Objects.push_back(NodeObject(pObj));
	}

	pObj->SetNode(this);
	pObj->SetNodeIndex(iIndex);

	m_iObjectCount++;
	if(m_pParent)
		m_pParent->SetChildNotEmpty(m_iChildFlag);

	return true;
}

bool OctreeNode::RemoveObject(IObject *pObj)
{
	MYASSERT(pObj && pObj->GetNode()!=0 && pObj->GetNodeIndex() >= 0 && pObj->GetNodeIndex() < m_Objects.size(),"Les references au noeud de l'objet sont invalides",true);

	m_FreeIndex.push_back(pObj->GetNodeIndex());
	m_Objects[pObj->GetNodeIndex()].bUsed=false;

	pObj->SetNode(0);
	pObj->SetNodeIndex(-1);

	m_iObjectCount--;
	if(m_iObjectCount <= 0 && m_iEmpty==0 && m_pParent)
		m_pParent->SetChildEmpty(m_iChildFlag);

	return true;
}

bool OctreeNode::UpdateObject(IObject *pObj)
{
	if(m_pParent && !m_BBox.IsIn(&pObj->GetTransformedBSphere(),true))
		return m_pParent->UpdateObject(pObj);
	else
	{
		if(pObj->GetNode()==this)
			return true;
		else
		{
			pObj->RemoveFromOctree();
			return InsertObject(pObj);
		}
	}
}

bool OctreeNode::Generate(const Core::Vector3& vPos,
							const Core::Vector3& vSize,
							int iCurrentDepth,
							int iMaxDepth)
{
	m_BBox.m_vMin=vPos;
	m_BBox.m_vMax=vPos + vSize;

	if(iCurrentDepth==iMaxDepth) return true;

	Core::Vector3 vNewSize(vSize * 0.5f);
	const int iChildrenFlags[]={1,2,4,8,16,32,64,128};

	for(int i=0;i<8;i++)
	{
		m_pChildren[i]=new OctreeNode;
		m_pChildren[i]->SetParent(this);
		m_pChildren[i]->SetChildFlags(iChildrenFlags[i]);
	}

	iCurrentDepth++;

	//	l=left; r=right; t=top; b=bottom; f=front; bk=back
	m_pChildren[0]->Generate(vPos,vNewSize,iCurrentDepth,iMaxDepth); // l-f-b
	m_pChildren[1]->Generate(Core::Vector3(vPos.x + vNewSize.x,vPos.y,vPos.z),vNewSize,iCurrentDepth,iMaxDepth); // r-f-b
	m_pChildren[2]->Generate(Core::Vector3(vPos.x,vPos.y + vNewSize.y,vPos.z),vNewSize,iCurrentDepth,iMaxDepth); // l-f-t
	m_pChildren[3]->Generate(Core::Vector3(vPos.x+vNewSize.x,vPos.y+vNewSize.y,vPos.z),vNewSize,iCurrentDepth,iMaxDepth); // r-f-t
	m_pChildren[4]->Generate(Core::Vector3(vPos.x,vPos.y,vPos.z+vNewSize.z),vNewSize,iCurrentDepth,iMaxDepth); // l-bk-b
	m_pChildren[5]->Generate(Core::Vector3(vPos.x+vNewSize.x,vPos.y,vPos.z+vNewSize.z),vNewSize,iCurrentDepth,iMaxDepth); // r-bk-b
	m_pChildren[6]->Generate(Core::Vector3(vPos.x,vPos.y+vNewSize.y,vPos.z+vNewSize.z),vNewSize,iCurrentDepth,iMaxDepth); // l-bk-t
	m_pChildren[7]->Generate(vPos+vNewSize,vNewSize,iCurrentDepth,iMaxDepth); // r-bk-t

	return true;
}

bool OctreeNode::Visibility(const IViewFrustrum *pFrustrum)
{
	if(m_iObjectCount > 0)
	{
		std::vector<NodeObject>::iterator i;
		for(i=m_Objects.begin();i!=m_Objects.end();i++)
			if(i->bUsed && pFrustrum->IsSphereInside(i->pObject->GetTransformedBSphere()))
			{
				g_RenderingQueue.Register(i->pObject);
			}
	}

	if(m_iEmpty > 0 && m_pChildren[0])
	{
		for(int i=0;i<8;i++)
		{
			if(pFrustrum->IsBoxInside(m_pChildren[i]->m_BBox))
				m_pChildren[i]->Visibility(pFrustrum);
		}
	}

	return true;
}

bool OctreeNode::Update(float fTime)
{
	if(m_iObjectCount > 0)
	{
		std::vector<NodeObject>::iterator i;
		for(i=m_Objects.begin();i!=m_Objects.end();i++)
			if(i->bUsed)
			{
				i->pObject->Update(fTime);
			}
	}

	if(m_iEmpty > 0 && m_pChildren[0])
	{
		for(int i=0;i<8;i++)
			m_pChildren[i]->Update(fTime);
	}

	return true;
}

void OctreeNode::CollectObjects(std::vector<IObject *> *pObjects)
{
	std::vector<NodeObject>::iterator i;
	for(i=m_Objects.begin();i!=m_Objects.end();i++)
		if(i->bUsed)
			pObjects->push_back(i->pObject);

	if(m_pChildren[0])
	{
		for(int i=0;i<8;i++)
			m_pChildren[i]->CollectObjects(pObjects);
	}
}

void OctreeNode::Pick(const Core::Vector3 &vOrigin, const Core::Vector3 &vDir, std::vector<IObject*> *pObjects)
{
	if(!IsEmpty() && Core::IntersectSphere(vOrigin,vDir,m_BBox.GetCenter(),m_BBox.GetSize().GetLength() * 0.5f)!=-1.0f)
	{
		std::vector<NodeObject>::iterator i;
		for(i=m_Objects.begin();i!=m_Objects.end();i++)
			if(i->bUsed &&
				Core::IntersectSphere(vOrigin,vDir,i->pObject->GetTransformedBSphere().GetCenter(),i->pObject->GetTransformedBSphere().GetRadius())!=-1.0f)
			{
				pObjects->push_back(i->pObject);
			}

		if(m_pChildren[0])
		{
			for(int j=0;j<8;j++)
				m_pChildren[j]->Pick(vOrigin,vDir,pObjects);
		}
	}
}

void OctreeNode::RebuildAllShadowVolumes()
{
	std::vector<NodeObject>::iterator i;
	for(i=m_Objects.begin();i!=m_Objects.end();i++)
		if(i->bUsed)
		{
			i->pObject->RebuildShadows();
		}

	if(m_pChildren[0])
	{
		for(int j=0;j<8;j++)
			m_pChildren[j]->RebuildAllShadowVolumes();
	}

}

void OctreeNode::Collision()
{
//	if(IsEmpty()) return;

	std::vector<NodeObject>::iterator i;
	for(i=m_Objects.begin();i!=m_Objects.end();i++)
		if(i->bUsed && i->pObject->CollideWithWorld() && i->pObject->HasMoved())
		{
			g_CollisionMgr.AddCollisionNode(i->pObject,0);
			CollectColliders(i->pObject);
		}

	if(m_pChildren[0])
	{
		for(int j=0;j<8;j++)
			m_pChildren[j]->Collision();
	}
}

void OctreeNode::CollectColliders(IObject *pObj)
{
//	if(IsEmpty()) return;

	const Core::BSphere& sphere(pObj->GetTransformedBSphere());

	std::vector<NodeObject>::iterator i;
	for(i=m_Objects.begin();i!=m_Objects.end();i++)
		if(i->bUsed && i->pObject->CollideWithWorld() &&
			i->pObject!=pObj && sphere.IsSphereInside(i->pObject->GetTransformedBSphere()))
		{
			g_CollisionMgr.AddCollisionNode(pObj,i->pObject);
		}

	if(m_pChildren[0])
	{
		for(int j=0;j<8;j++)
		{
			if(!m_pChildren[j]->IsEmpty())// && m_pChildren[j]->m_BBox.IsIn(&sphere))
				m_pChildren[j]->CollectColliders(pObj);
		}
	}
}

// ********************************************* Octree *************************************************

Octree::Octree() : m_pRoot(0)
{
}

Octree::~Octree()
{
	Clear();
}

void Octree::Clear()
{
	if(m_pRoot)
		delete m_pRoot;

	m_pRoot=0;
}

bool Octree::Generate(int iMapSize, int iHeight, int iDepth)
{
	Clear();

	Core::Vector3 vPos(0.0f,0.0f,0.0f);
	Core::Vector3 vSize((float)iMapSize,(float)iHeight,(float)iMapSize);

	m_pRoot=new OctreeNode;
	
	return m_pRoot->Generate(vPos,vSize,0,iDepth);
}

bool Octree::InsertObject(IObject *pObj)
{
	if(pObj && m_pRoot)
		return m_pRoot->InsertObject(pObj);
	else
		return false;
}

bool Octree::Visibility(const IViewFrustrum *pFrustrum)
{
	if(m_pRoot)
		return m_pRoot->Visibility(pFrustrum);
	else
		return false;
}

bool Octree::Update(float fTime)
{
	if(m_pRoot)
		return m_pRoot->Update(fTime);
	else
		return false;
}

void Octree::CollectObjects(std::vector<IObject*> *pObjects)
{
	if(m_pRoot)
		m_pRoot->CollectObjects(pObjects);
}

void Octree::Pick(const Core::Vector3& vOrigin, const Core::Vector3& vDir, std::vector<IObject*> *pObjects)
{
	if(m_pRoot)
		m_pRoot->Pick(vOrigin,vDir,pObjects);

}

void Octree::RebuildAllShadowVolumes()
{
	if(m_pRoot)
		m_pRoot->RebuildAllShadowVolumes();
}

void Octree::Collision()
{
	if(m_pRoot)
		m_pRoot->Collision();
}

}