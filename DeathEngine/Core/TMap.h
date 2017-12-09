
#ifndef _TMAP_H_

#define _TMAP_H_

#include "TMemoryAllocator.h"

namespace Core
{

template<class KeyType, class ValueType>
class TMap
{
	template<class KeyTypeRB, class ValueTypeRB>
	class RBTree
	{
		public:

			RBTree(const KeyTypeRB& k, const ValueTypeRB& v) :
			  m_pLeftChild(0), m_pRightChild(0), m_pParent(0),
				  m_Key(k),m_Value(v),m_bIsRed(true) {};
			~RBTree() {};

			void Reset(const KeyTypeRB& k,const ValueTypeRB& v)
			{
				m_Key=k;
				m_Value=v;
			}
			void Clear()
			{
				m_bIsRed=true;
				m_pLeftChild=0;
				m_pRightChild=0;
			}

			void SetLeftChild(RBTree *p)
			{
				m_pLeftChild=p;
				if(p)
					p->SetParent(this);
			};
			void SetRightChild(RBTree *p)
			{
				m_pRightChild=p;
				if(p)
					p->SetParent(this);
			};

			void SetParent(RBTree *p) { m_pParent=p; };

			void SetValue(const ValueTypeRB& v) { m_Valie=v; };

			void SetRed() { m_bIsRed=true; };
			void SetBlack() { m_bIsRed=false; };

			RBTree *GetLeftChild() const { return m_pLeftChild; };
			RBTree *GetRightChild() const { return m_pRightChild; };
			RBTree *GetParent() const { return m_pParent; };

			ValueTypeRB GetValue() const { return m_Value; };
			KeyTypeRB GetKey() const { return m_Key; };

			bool IsRoot() const { return m_pParent==0; };
			bool IsLeftChild() const { return (m_pParent!=0) && (m_pParent->GetLeftChild()==this); };
			bool IsRightChild() const { return (m_pParent!=0) && (m_pParent->GetRightChild()==this); };
			bool IsLeaf() const { return (m_pLeftChild==0) && (m_pRightChild==0); };

			unsigned int GetLevel() const
			{
				if(IsRoot()) return 1;
				else return GetParent()->GetLevel() + 1;
			};

			bool IsRed() const { return m_bIsRed; };
			bool IsBlack() const { return !m_bIsRed; };

		private:

			RBTree();

			RBTree *m_pLeftChild,*m_pRightChild;
			RBTree *m_pParent;

			KeyTypeRB m_Key;
			ValueTypeRB m_Value;

			bool m_bIsRed;
	};

	public:

		typedef RBTree<KeyType,ValueType> Node;

		class Iterator
		{
			public:

				Iterator() : m_pRoot(0),m_pCur(0) {};
				Iterator(Node *pRoot) : m_pRoot(pRoot) { Reset(); };
				Iterator(const Iterator& src) : m_pRoot(src.m_pRoot), m_pCur(src.m_pCur) {};

				void Reset(bool bAtLowest=true)
				{
					if(bAtLowest) m_pCur=GetMin(m_pRoot);
					else m_pCur=GetMax(m_pRoot);
				};

				bool AtEnd() const { return m_pCur==0; };
				Node *GetNode() { return m_pCur; };

				Iterator& operator=(const Iterator& src)
				{
					m_pRoot=src.m_pRoot;
					m_pCur=src.m_pCur;
					return *this;
				};

				void operator++(int) { Inc(); };
				void operator--(int) { Dec(); };

				Node *operator->() { return GetNode(); };
				Node& operator*() { if(AtEnd()) throw "Iterator at end"; return *m_pCur; };

			private:

				Node *GetMin(Node *n)
				{
					while(n && n->GetLeftChild())
						n=n->GetLeftChild();
					return n;
				};

				Node *GetMax(Node *n)
				{
					while(n && n->GetRightChild())
						n=n->GetRightChild();
					return n;
				};

				void Inc()
				{
					if(m_pCur==0) return;
					if(m_pCur->GetRightChild()) m_pCur=GetMin(m_pCur->GetRightChild());
					else if(m_pCur->IsLeftChild()) m_pCur=m_pCur->GetParent();
					else
					{
						while(m_pCur->IsRightChild())
							m_pCur=m_pCur->GetParent();
						m_pCur=m_pCur->GetParent();
					}
				};

				void Dec()
				{
					if(m_pCur==0) return;
					if(m_pCur->GetLeftChild()) m_pCur=GetMax(m_pCur->GetLeftChild());
					else if(m_pCur->IsRightChild()) m_pCur=m_pCur->GetParent();
					else
					{
						while(m_pCur->IsLeftChild())
							m_pCur=m_pCur->GetParent();
						m_pCur=m_pCur->GetParent();
					}
				};

				Node *m_pRoot,m_pCur;
		};

		class ParentFirstIterator
		{
			public:

				ParentFirstIterator() : m_pRoot(0), m_pCur(0) {};
				explicit ParentFirstIterator(Node *pRoot) : m_pRoot(pRoot),m_pCur(0) { Reset(); };

				void Reset() { m_pCur=m_pRoot; };

				bool AtEnd() const { return m_pCur==0; };
				Node *GetNode() { return m_pCur; };

				ParentFirstIterator& operator=(const ParentFirstIterator& src)
				{
					m_pRoot=src.m_pRoot;
					m_pCur=src.m_pCur;
					return *this;
				};

				void operator++(int) { Inc(); };
				Node *operator->() { return GetNode(); };
				Node& operator*() { if(AtEnd()) throw "ParentFirstIterator at end"; return *GetNode(); };

			private:

				void Inc()
				{
					if(m_pCur==0) return;
					if(m_pCur->GetLeftChild()) m_pCur=m_pCur->GetLeftChild();
					else if(m_pCur->GetRightChild()) m_pCur=m_pCur->GetRightChild();
					else
					{
						while(m_pCur!=0)
						{
							if(m_pCur->IsLeftChild() && m_pCur->GetParent()->GetRightChild())
							{
								m_pCur=m_pCur->GetParent()->GetRightChild();
								return;
							}
							m_pCur=m_pCur->GetParent();
						}
					}
				};

				Node *m_pRoot,*m_pCur;
		};

		class ParentLastIterator
		{
			public:

				ParentLastIterator() : m_pRoot(0), m_pCur(0) {};
				explicit ParentLastIterator(Node *pRoot) : m_pRoot(pRoot), m_pCur(0) { Reset(); };

				void Reset() { m_pCur=GetMin(m_pRoot); };
				bool AtEnd() const { return m_pCur==0; };
				Node *GetNode() { return m_pCur; };

				ParentLastIterator& operator=(const ParentLastIterator& src)
				{
					m_pRoot=src.m_pRoot;
					m_pCur=src.m_pCur;
					return *this;
				};

				void operator++(int) { Inc(); };
				Node *operator->() { return GetNode(); };
				Node& operator*() { if(AtEnd()) throw "ParentLastIterator at end"; return *GetNode(); };

			private:

				Node *GetMin(Node *n)
				{
					while(n!=0 && (n->GetLeftChild()!=0 || n->GetRightChild()!=0))
					{
						if(n->GetLeftChild()) n=n->GetLeftChild();
						else n=n->GetRightChild();
					}
					return n;
				};

				void Inc()
				{
					if(m_pCur==0) return;
					if(m_pCur->IsLeftChild() && m_pCur->GetParent()->GetRightChild())
						m_pCur=GetMin(m_pCur->GetParent()->GetRightChild());
					else
						m_pCur=m_pCur->GetParent();
				};

				Node *m_pRoot,*m_pCur;
		};

		class AccessClass
		{
			friend class TMap<KeyType,ValueType>;

			public:

				void operator=(const ValueType& value) { m_Tree.Set(m_Key,value); };
				operator ValueType()
				{
					Node *pNode=m_Tree.Find(m_Key);

					if(pNode==0)
						throw "Item not found";

					return pNode->GetValue();
				};

			private:

				AccessClass(TMap& tree, const KeyType& key) : m_Tree(tree), m_Key(key) {};
				AccessClass();

				TMap m_Tree;
				const KeyType& m_Key;
		};

		TMap() : m_pRoot(0),m_dwSize(0) {};
		~TMap() { Clear(); };

		bool Insert(const KeyType& keynew, const ValueType& v)
		{
			Node *pNewNode=new Node(keynew,v);
			if(!Insert(pNewNode))
			{
				delete pNewNode;
				return false;
			}

			while(!pNewNode->IsRoot() && (pNewNode->GetParent()->IsRed()))
			{
				if(pNewNode->GetParent()->IsLeftChild())
				{
					Node *pNewNodesUncle=pNewNode->GetParent()->GetParent()->GetRightChild();
					if(pNewNodesUncle!=0 && pNewNodesUncle->IsRed())
					{
						pNewNode->GetParent()->SetBlack();
						pNewNodesUncle->SetBlack();
						pNewNode->GetParent()->GetParent()->SetRed();
						pNewNode=pNewNode->GetParent()->GetParent();
					}
					else
					{
						if(pNewNode->IsRightChild())
						{
							pNewNode=pNewNode->GetParent();
							RotateLeft(pNewNode);
						}

						pNewNode->GetParent()->SetBlack();
						pNewNode->GetParent()->GetParent()->SetRed();
						RotateRight(pNewNode->GetParent()->GetParent());
					}
				}
				else
				{
					Node *pNewNodesUncle=pNewNode->GetParent()->GetParent()->GetLeftChild();
					if(pNewNodesUncle!=0 && pNewNodesUncle->IsRed())
					{
						pNewNode->GetParent()->SetBlack();
						pNewNodesUncle->SetBlack();
						pNewNode->GetParent()->GetParent()->SetRed();
						pNewNode=pNewNode->GetParent()->GetParent();
					}
					else
					{
						if(pNewNode->IsLeftChild())
						{
							pNewNode=pNewNode->GetParent();
							RotateRight(pNewNode);
						}

						pNewNode->GetParent()->SetBlack();
						pNewNode->GetParent()->GetParent()->SetRed();
						RotateLeft(pNewNode->GetParent()->GetParent());
					}
				}
			}

			m_pRoot->SetBlack();
			return true;
		};

		void Set(const KeyType& k, const ValueType& v)
		{
			Node *p=Find(k);
			if(p)
				p->SetValue(v);
			else
				Insert(k,v);
		};

		Node *Delink(const KeyType& k)
		{
			Node *p=Find(k);
			if(p==0)
				return 0;

			while(p->GetRightChild())
				RotateLeft(p);

			Node *pLft=p->GetLeftChild();

			if(p->IsLeftChild())
				p->GetParent()->SetLeftChild(pLeft);
			else if(p->IsRightChild())
				p->GetParent()->SetRightChild(pLeft);
			else
				SetRoot(pLeft);

			m_dwSize--;
			return p;
		};

		bool Remove(const KeyType& k)
		{
			Node *p=Find(k);
			if(p==0)
				return false;

			while(p->GetRightChild())
				RotateLeft(p);

			Node *pLeft=p->GetLeftChild();

			if(p->IsLeftChild())
				p->GetParent()->SetLeftChild(pLeft);
			else if(p->IsRightChild())
				p->GetParent()->SetRightChild(pLeft);
			else
				SetRoot(pLeft);

			delete p;
			m_dwSize--;
			return true;
		};

		void Clear()
		{
			ParentLastIterator i(GetParentLastIterator());

			while(!i.AtEnd())
			{
				Node *p=i.GetNode();
				i++;
				delete p;
			}
			m_pRoot=0;
			m_dwSize=0;
		};

		bool IsEmpty() const { return m_pRoot=0; };

		Node *Find(const KeyType& keyToFind) const
		{
			Node *pNode=m_pRoot;
			while(pNode!=0)
			{
				KeyType key(pNode->GetKey());
				if(keyToFind==key)
					return pNode;
				else if(keyToFind < key)
					pNode=pNode->GetLeftChild();
				else
					pNode=pNode->GetRightChild();
			}
			return 0;
		};

		Node *GetRoot() const { return m_pRoot; };
		uint32 Size() const { return m_dwSize; };

		Iterator GetIterator() { return Iterator(GetRoot()); };
		ParentFirstIterator GetParentFirstIterator() { return ParentFirstIterator(m_pRoot); };
		ParentLastIterator GetParentLastIterator() { return ParentLastIterator(m_pRoot); };

		AccessClass operator[](const KeyType& k) { return AccessClass(*this,k); };

	private:

		explicit TMap(const TMap& src);
		TMap operator=(const TMap& src);

		void SetRoot(Node *pNewRoot)
		{
			m_pRoot=pNewRoot;
			if(m_pRoot!=0) m_pRoot->SetParent(0);
		};

		bool Insert(Node *pNewNode)
		{
			bool bResult=true;
			if(m_pRoot==0)
			{
				SetRoot(pNewNode);
				m_dwSize=1;
			}
			else
			{
				Node *pNode=m_pRoot;
				KeyType keyNew=pNewNode->GetKey();
				while(pNode)
				{
					KeyType key(pNode->GetKey());
					if(keyNew==key)
					{
						bResult=false;
						pNode=0;
					}
					else if(keyNew < key)
					{
						if(pNode->GetLeftChild()==0)
						{
							pNode->SetLeftChild(pNewNode);
							pNode=0;
						}
						else
							pNode=pNode->GetLeftChild();
					}
					else
					{
						if(pNode->GetRightChild()==0)
						{
							pNode->SetRightChild(pNewNode);
							pNode=0;
						}
						else
							pNode=pNode->GetRightChild();
					}
				}

				if(bResult)
					m_dwSize++;
			}

			return bResult;
		};

		void RotateLeft(Node *p)
		{
			Node *pRight=p->GetRightChild();
			p->SetRightChild(pRight->GetLeftChild());

			if(p->IsLeftChild())
				p->GetParent()->SetLeftChild(pRight);
			else if(p->IsRightChild())
				p->GetParent()->SetRightChild(pRight);
			else
				SetRoot(pRight);

			pRight->SetLeftChild(p);
		};

		void RotateRight(Node *p)
		{
			Node *pLeft=p->GetLeftChild();
			p->SetLeftChild(pLeft->GetRightChild());

			if(p->IsLeftChild())
				p->GetParent()->SetLeftChild(pLeft);
			else if(p->IsRightChild())
				p->GetParent()->SetRightChild(pLeft);
			else
				SetRoot(pLeft);

			pLeft->SetRightChild(p);
		};

		Node *m_pRoot;
		uint32 m_dwSize;

		static TMemoryAllocator<Node> m_Allocator;
		static Node *GetNewNode(const KeyType& keynew, const ValueType& v)
		{
			Node *pNode=m_Allocator.GetFree();
			if(!pNode)
			{
				pNode=new Node(keynew,v);
				m_Allocator.RegisterNew(pNode);
			}
			else
				pNode->Reset(keynew,v);

			return pNode;
		}
		static void DeleteNode(Node *pNode)
		{
			if(pNode)
				m_Allocator.Delete(pNode);
		}
};

}

#endif