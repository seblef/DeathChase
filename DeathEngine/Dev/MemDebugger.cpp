
#ifdef DEDEV

#include "MemDebugger.h"

namespace Dev
{

MemDebugger::MemDebugger()
{
}

MemDebugger::~MemDebugger()
{
	SaveResults();
}

void MemDebugger::AddObjNew(const char *szObjName, int iSize)
{
	const int index=GetObj(szObjName);
	m_ObjItems[index].iNewCount++;
	m_ObjItems[index].iSize=iSize;
	m_ObjItems[index].iTotalSize+=iSize;
	m_ObjItems[index].iInstances++;
}

void MemDebugger::AddObjDel(const char *szObjName)
{
	const int index=GetObj(szObjName);
	m_ObjItems[index].iDelCount++;
	m_ObjItems[index].iTotalSize-=m_ObjItems[index].iSize;
	m_ObjItems[index].iInstances--;
}

void MemDebugger::AddObjMNew(const char *szObjName, int iCount, int iSize)
{
	const int index=GetObj(szObjName);
	m_ObjItems[index].iMNewCount++;
	m_ObjItems[index].iSize=iSize;
	m_ObjItems[index].iTotalSize+=iSize * iCount;
	m_ObjItems[index].iInstances+=iCount;
}

void MemDebugger::AddObjMDel(const char *szObjName, int iCount)
{
	const int index=GetObj(szObjName);
	m_ObjItems[index].iMDelCount++;
	m_ObjItems[index].iTotalSize-=m_ObjItems[index].iSize * iCount;
	m_ObjItems[index].iInstances-=iCount;
}

void MemDebugger::AddObjExtraSize(const char *szObjName, int iSize)
{
	const int index=GetObj(szObjName);
	m_ObjItems[index].iExtraSize+=iSize;
	m_ObjItems[index].iExtraCount++;
	m_ObjItems[index].iTotalSize+=iSize;
}

void MemDebugger::RemObjExtraSize(const char *szObjName, int iSize)
{
	const int index=GetObj(szObjName);
	m_ObjItems[index].iExtraSize-=iSize;
	m_ObjItems[index].iExtraCount--;
	m_ObjItems[index].iTotalSize-=iSize;
}

void MemDebugger::SaveResults()
{
	FILE *file=fopen("memory.log","w");
	if(!file)
		return;

	fprintf(file,"Rapport de mémoire:\n\n");

	MemDebuggerVector::iterator i;
	for(i=m_ObjItems.begin();i!=m_ObjItems.end();i++)
	{
		bool error=false;

		if(i->iDelCount!=i->iNewCount)
		{
			error=true;
			fprintf(file,"x %s: %i delete pour %i new\n",i->szName,i->iDelCount,i->iNewCount);
		}

		if(i->iMDelCount!=i->iMNewCount)
		{
			error=true;
			fprintf(file,"x %s: %i delete[] pour %i new[]\n",i->szName,i->iMDelCount,i->iMNewCount);
		}

		if(i->iExtraCount!=0)
			fprintf(file,"x %s: %i extra\n",i->szName,i->iExtraCount);

		if(!error && i->iInstances!=0)
			fprintf(file,"x %s: il reste %i instances\n",i->szName,i->iInstances);
	}

	fclose(file);
}

inline int MemDebugger::GetObj(const char *szName)
{
	m_strObjName=szName;

	MemDebuggerMap::iterator i=m_ObjNameMap.find(m_strObjName);
	if(i!=m_ObjNameMap.end())
		return i->second;
	else
	{
		MemItem item;
		memset(&item,0,sizeof(item));
		strcpy(item.szName,szName);

		m_ObjItems.push_back(item);
		m_ObjNameMap.insert(std::make_pair(m_strObjName,(int)(m_ObjItems.size()-1)));
		return (int)(m_ObjItems.size()-1);
	}
}

}

#endif