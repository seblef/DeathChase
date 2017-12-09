
#ifndef _IMEMDEBUGGER_H_
#define _IMEMDEBUGGER_H_

#ifdef DEDEV

#include <vector>

typedef struct _MemItem
{
	char szName[64];
	int iNewCount;
	int iDelCount;
	int iMNewCount;
	int iMDelCount;
	int iSize;
	int iExtraSize;
	int iTotalSize;
	int iInstances;
	int iExtraCount;

} MemItem;

typedef std::vector<MemItem> MemDebuggerVector;

class IMemDebugger
{
	public:

		IMemDebugger() {}
		virtual ~IMemDebugger() {}

		virtual const MemDebuggerVector& GetItems()=0;
};

#endif
#endif