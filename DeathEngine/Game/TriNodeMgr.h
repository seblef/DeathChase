
#ifndef _TRINODEMGR_H_

#define _TRINODEMGR_H_

namespace Game
{

class TriNode
{
	public:

		TriNode *LeftChild; 
		TriNode *RightChild; 
		TriNode *BaseNeighbor; 
		TriNode *LeftNeighbor; 
		TriNode *RightNeighbor; 

};

class TriNodeMgr
{
	public:
		static TriNodeMgr *Instance();

		TriNodeMgr();
		~TriNodeMgr();

		TriNode *GetNextTriNode();
		void Reset();

	protected:

		static TriNodeMgr *m_pInstance;
		
		TriNode *m_pNodeCount;
		TriNode *m_pMaxCount;
		TriNode *m_pNodes;

};

}

#endif