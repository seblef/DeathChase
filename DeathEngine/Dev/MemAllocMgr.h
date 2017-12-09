
#ifdef DEDEV

#ifndef _MEMALLOCMGR_H_
#define _MEMALLOCMGR_H_

#include "IMemoryAllocator.h"
#include "../Core/TSingleton.h"
#include <vector>

#ifdef DEEXPORT
#include "../Export/IMemAllocMgr.h"
#endif

namespace Dev
{

typedef std::vector<IMemoryAllocator *> MemAllocVector;

#ifdef DEEXPORT
class MemAllocMgr : public IMemAllocMgr, public Core::TSingleton<MemAllocMgr>
#else
class MemAllocMgr : public Core::TSingleton<MemAllocMgr>
#endif
{
	public:

		MemAllocMgr() {}
		~MemAllocMgr() {}

		void RegisterAllocator(IMemoryAllocator *pAllocator) { m_Allocators.push_back(pAllocator); }
		void RemoveAllocator(IMemoryAllocator *pAllocator)
		{
			MemAllocVector::iterator i;
			for(i=m_Allocators.begin();i!=m_Allocators.end();i++)
				if((*i)==pAllocator)
				{
					m_Allocators.erase(i);
					break;
				}
		}

		MemAllocVector& GetAllocators() { return m_Allocators; }
		int GetAllocatorCount() const { return m_Allocators.size(); }
		void GetAllocatorInfos(int iIndex, char *szClassName, int& iUsedCount, int& iFreeCount)
		{
			if(iIndex >= 0 && iIndex < m_Allocators.size())
			{
				strcpy(szClassName,m_Allocators[iIndex]->GetClassName_());
				iUsedCount=m_Allocators[iIndex]->GetUsedItemsCount();
				iFreeCount=m_Allocators[iIndex]->GetFreeCount();
			}
			else
			{
				strcpy(szClassName,"");
				iUsedCount=-1;
				iFreeCount=-1;
			}
		}

	protected:

		MemAllocVector m_Allocators;
};

}

#define g_MemAllocMgr			Dev::MemAllocMgr::GetSingletonRef()

#endif
#endif