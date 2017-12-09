
#ifndef _COLLISIONMGR_H_
#define _COLLISIONMGR_H_

#include "../Core/TSingleton.h"
#include <vector>

namespace Game
{

class IObject;


class CollisionNode
{
	public:

		CollisionNode() : pObject1(0), pObject2(0) {}
		CollisionNode(IObject *pObj1, IObject *pObj2) : pObject1(pObj1), pObject2(pObj2) {}
		CollisionNode(const CollisionNode& node) : pObject1(node.pObject1), pObject2(node.pObject2) {}
		~CollisionNode() {}

		const CollisionNode& operator=(const CollisionNode& node)
		{
			pObject1=node.pObject1;
			pObject2=node.pObject2;
			return *this;
		}

		IObject *pObject1;
		IObject *pObject2;

};

class CollisionMgr : public Core::TSingleton<CollisionMgr>
{
	public:

		CollisionMgr();
		~CollisionMgr();

		void AddCollisionNode(IObject *pObj1, IObject *pObj2);
		void AddCollisionNode(CollisionNode *pNodes, int iCount);

		void ProcessCollisions();
		void ProcessMessages();

		void Execute();

	protected:

		bool CheckMapCollision(CollisionNode& node);

		std::vector<CollisionNode> m_ColBuffers[2];
		std::vector<CollisionNode> m_MsgBuffer;

		int m_iCurrentExecBuffer;
		int m_iCurrentFillBuffer;

};

}

#define g_CollisionMgr				Game::CollisionMgr::GetSingletonRef()

#endif