
#ifdef DEDEV

#ifndef _IMEMORYALLOCATOR_H_
#define _IMEMORYALLOCATOR_H_

namespace Dev
{

class IMemoryAllocator
{
	public:

		IMemoryAllocator() {}
		~IMemoryAllocator() {}

		virtual int GetItemCount()=0;
		virtual int GetUsedItemsCount()=0;
		virtual int GetFreeCount()=0;
		virtual const char *GetClassName_() const=0;

};

}

#endif
#endif