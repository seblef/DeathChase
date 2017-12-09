
#ifndef _TMEMORYALLOCATOR_H_

#define _TMEMORYALLOCATOR_H_

#include "../MyAssert.h"
#include "TAllocatorPtr.h"
#include <vector>

#ifdef DEDEV
#include "../Dev/MemAllocMgr.h"
#endif

namespace Core
{

#ifndef DEDEV
template<class T>
class TMemoryAllocator
{
	public:

		TMemoryAllocator(const char *szClassName) : m_iUsedItemsCount(0)
		{
			strcpy(m_szClassName,szClassName);
		};
		~TMemoryAllocator()
		{
			Clear();
		};

		TAllocatorPtr<T> New()
		{
			if(m_Free.size()==0)
			{
				T *ptr=new T;
				m_Items.push_back(ptr);
				m_iUsedItemsCount++;
				return TAllocatorPtr<T>(m_Items.size()-1,ptr);
			}
			else
			{
				int idx=m_Free[m_Free.size()-1];
				T *ptr=m_Items[idx];
				m_Free.pop_back();
				m_iUsedItemsCount++;
				return TAllocatorPtr<T>(idx,ptr);
			}
		}

		TAllocatorPtr<T> GetFree()
		{
			if(m_Free.size() > 0)
			{
				int idx=m_Free[m_Free.size()-1];
				T *ptr=m_Items[idx];
				m_Free.pop_back();
				m_iUsedItemsCount++;
				return TAllocatorPtr<T>(idx,ptr);
			}
			else
				return TAllocatorPtr<T>();
		}

		void RegisterNew(T *ptr)
		{
			m_Items.push_back(ptr);
			m_iUsedItemsCount++;
		}

		void Delete(TAllocatorPtr<T>& ptr)
		{
			if(!ptr.IsValid() || ptr.GetIndex() >= m_Items.size())
				return;

			ptr.GetPtr()->Clear();
			m_Free.push_back(ptr.GetIndex());
			m_iUsedItemsCount--;

			ptr.SetPtr(0);
			ptr.SetIndex(-1);
		}

		void Delete(T *ptr)
		{
			if(!ptr)
				return;

			std::vector<T*>::iterator i;
			int count=0;
			for(i=m_Items.begin();i!=m_Items.end();i++,count++)
				if((*i)==ptr)
				{
					ptr->Clear();
					m_Free.push_back(count);
					m_iUsedItemsCount--;
					break;
				}
		}

		void ResetAll()
		{
			m_Free.clear();
			int i;
			for(i=0;i<m_Items.size();i++)
			{
				m_Items[i]->Clear();
				m_Free.push_back(m_Items.size() - i);
			}

			m_iUsedItemsCount=0;
		}

		void Clear()
		{
			std::vector<T*>::iterator i;
			for(i=m_Items.begin();i!=m_Items.end();i++)
				delete *i;

			m_Items.clear();
			m_Free.clear();
			m_iUsedItemsCount=0;
		}

		int GetItemCount() { return m_Items.size(); }
		int GetUsedItemsCount() { return m_iUsedItemsCount; }
		int GetFreeCount() { return m_Free.size(); }
		const char *GetClassName() const { return m_szClassName; }

	protected:

		std::vector<T*> m_Items;
		std::vector<int> m_Free;
		int m_iUsedItemsCount;
		char m_szClassName[128];

};

#else

template<class T>
class TMemoryAllocator : public Dev::IMemoryAllocator
{
	public:

		TMemoryAllocator(const char *szClassName) : m_iUsedItemsCount(0), m_bRegister(false)
		{
			MYASSERT(szClassName,"Bad class name",true);
			strcpy(m_szClassName,szClassName);
		};
		~TMemoryAllocator()
		{
//			g_MemAllocMgr.RemoveAllocator(this);

			Clear();
		};

		TAllocatorPtr<T> New()
		{
			if(!m_bRegister)
			{
				g_MemAllocMgr.RegisterAllocator(this);
				m_bRegister=true;
			}

			if(m_Free.size()==0)
			{
				T *ptr=new T;
				m_Items.push_back(ptr);
				m_iUsedItemsCount++;
				return TAllocatorPtr<T>(m_Items.size()-1,ptr);
			}
			else
			{
				int idx=m_Free[m_Free.size()-1];
				T *ptr=m_Items[idx];
				m_Free.pop_back();
				m_iUsedItemsCount++;
				return TAllocatorPtr<T>(idx,ptr);
			}
		}

		TAllocatorPtr<T> GetFree()
		{
			if(!m_bRegister)
			{
				g_MemAllocMgr.RegisterAllocator(this);
				m_bRegister=true;
			}

			if(m_Free.size() > 0)
			{
				int idx=m_Free[m_Free.size()-1];
				T *ptr=m_Items[idx];
				m_Free.pop_back();
				m_iUsedItemsCount++;
				return TAllocatorPtr<T>(idx,ptr);
			}
			else
				return TAllocatorPtr<T>();
		}

		void RegisterNew(T *ptr)
		{
			if(!m_bRegister)
			{
				g_MemAllocMgr.RegisterAllocator(this);
				m_bRegister=true;
			}

			MYASSERT(ptr,"Bad mem allocator pointer",true);
			m_Items.push_back(ptr);
			m_iUsedItemsCount++;
		}

		void Delete(TAllocatorPtr<T>& ptr)
		{
			if(!ptr.IsValid() || ptr.GetIndex() >= m_Items.size())
				return;

			ptr.GetPtr()->Clear();
			m_Free.push_back(ptr.GetIndex());
			m_iUsedItemsCount--;

			ptr.SetPtr(0);
			ptr.SetIndex(-1);
		}

		void Delete(T *ptr)
		{
			if(!ptr) return;

			std::vector<T*>::iterator i;
			int count=0;
			for(i=m_Items.begin();i!=m_Items.end();i++,count++)
				if((*i)==ptr)
				{
					ptr->Clear();
					m_Free.push_back(count);
					m_iUsedItemsCount--;
					break;
				}
		}

		void ResetAll()
		{
			m_Free.clear();
			int i;
			for(i=0;i<m_Items.size();i++)
			{
				m_Items[i]->Clear();
				m_Free.push_back(m_Items.size() - i-1);
			}

			m_iUsedItemsCount=0;
		}

		void Clear()
		{
			std::vector<T*>::iterator i;
			for(i=m_Items.begin();i!=m_Items.end();i++)
				delete *i;

			m_Items.clear();
			m_Free.clear();
			m_iUsedItemsCount=0;
		}

		int GetItemCount() { return m_Items.size(); }
		int GetUsedItemsCount() { return m_iUsedItemsCount; }
		int GetFreeCount() { return m_Free.size(); }
		const char *GetClassName_() const { return m_szClassName; }

	protected:

		std::vector<T*> m_Items;
		std::vector<int> m_Free;
		int m_iUsedItemsCount;
		char m_szClassName[128];

		bool m_bRegister;

};

#endif

}

#endif
