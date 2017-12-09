
#ifndef _IMEMALLOCMGR_H_
#define _IMEMALLOCMGR_H_

#ifdef DEDEV

class IMemAllocMgr
{
	public:

		IMemAllocMgr() {}
		virtual ~IMemAllocMgr() {}

		virtual void GetAllocatorInfos(int iIndex, char *szClassName,
			int& iUsedCount, int& iFreeCount)=0;
		virtual int GetAllocatorCount() const=0;
};

#endif
#endif

