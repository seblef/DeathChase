
#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "../Core/TSingleton.h"
#include "../Core/ViewFrustrum.h"

#include <vector>

namespace Game
{

class IObject;

class OctreeNode
{
	public:

		OctreeNode();
		~OctreeNode() { Clear(); }

		void Clear();
		bool Generate(const Core::Vector3& vPos,const Core::Vector3& vSize, int iCurrentDepth, int iMaxDepth);

		bool InsertObject(IObject *pObj);
		bool RemoveObject(IObject *pObj);
		bool UpdateObject(IObject *pObj);

		bool Visibility(const IViewFrustrum *pFrustrum);
		bool Update(float fTime);

		bool IsEmpty() const { return m_iEmpty==0; }
		int GetObjectCount() const { return m_iObjectCount; }

		void CollectObjects(std::vector<IObject *> *pObjects);
		void Pick(const Core::Vector3& vOrigin, const Core::Vector3& vDir, std::vector<IObject*> *pObjects);
		void RebuildAllShadowVolumes();

		void Collision();

	protected:

		void SetParent(OctreeNode *pNode) { m_pParent=pNode; }
		OctreeNode *GetParent() const { return m_pParent; }

		//			Pour aller plus vite (certaines zones de l'octree ne sont jamais utilisées, alors pourquoi calculer?)
		void SetChildFlags(int iFlag) { m_iChildFlag=iFlag; }
		void SetChildEmpty(int iFlag)
		{
			m_iEmpty&=(!iFlag);
			if(m_iEmpty==0 && m_iObjectCount==0 && m_pParent)
				m_pParent->SetChildEmpty(m_iChildFlag);
		}
		void SetChildNotEmpty(int iFlag)
		{
			m_iEmpty|=iFlag;
			if(m_pParent)
				m_pParent->SetChildNotEmpty(m_iChildFlag);
		}

		void CollectColliders(IObject *pObj);

		Core::BBox m_BBox;
		OctreeNode *m_pChildren[8];
		OctreeNode *m_pParent;

		class NodeObject
		{
			public:

				NodeObject(IObject *pObj) : pObject(pObj), bUsed(true) {}
				~NodeObject() {}

				const NodeObject& operator=(const NodeObject& obj)
				{
					pObject=obj.pObject;
					bUsed=obj.bUsed;
					return *this;
				}

				IObject *pObject;
				bool bUsed;
		};

		std::vector<NodeObject> m_Objects;
		std::vector<int> m_FreeIndex;

		int m_iObjectCount;
		int m_iEmpty;
		int m_iChildFlag;
};


class Octree : public Core::TSingleton<Octree>
{
	public:

		Octree();
		~Octree();

		void Clear();
		bool Generate(int iMapSize, int iHeight, int iDepth);

		bool InsertObject(IObject *pObj);

		bool Visibility(const IViewFrustrum *pFrustrum);
		bool Update(float fTime);

		void CollectObjects(std::vector<IObject *> *pObjects);
		void Pick(const Core::Vector3& vOrigin, const Core::Vector3& vDir, std::vector<IObject*> *pObjects);
		void RebuildAllShadowVolumes();

		void Collision();

	protected:

		OctreeNode *m_pRoot;

};

}

#define g_Octree				Game::Octree::GetSingletonRef()

#endif