
#ifndef _MEMDEBUGGER_H_
#define _MEMDEBUGGER_H_

#ifdef DEDEV

#include "../Core/TSingleton.h"
#include <map>
#include <string>

#ifdef DEEXPORT
#include "../Export/IMemDebugger.h"
#else
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
#endif

namespace Dev
{

#ifdef DEEXPORT
class MemDebugger : public IMemDebugger, public Core::TSingleton<MemDebugger>
#else
class MemDebugger : public Core::TSingleton<MemDebugger>
#endif
{
	public:

		typedef std::map<std::string,int> MemDebuggerMap;

		MemDebugger();
		~MemDebugger();

		void AddObjNew(const char *szObjName, int iSize);
		void AddObjDel(const char *szObjName);

		void AddObjMNew(const char *szObjName, int iCount, int iSize);
		void AddObjMDel(const char *szObjName, int iCount);

		void AddObjExtraSize(const char *szObjName, int iSize);
		void RemObjExtraSize(const char *szObjName, int iSize);

		const MemDebuggerVector& GetItems() { return m_ObjItems; };
		void SaveResults();

	private:

		inline int GetObj(const char *szName);

		MemDebuggerMap m_ObjNameMap;
		MemDebuggerVector m_ObjItems;

		std::string m_strObjName;
};

}

#define g_MemDebugger			Dev::MemDebugger::GetSingletonRef()

#define MEMDEBUG_NEW				void *operator new(size_t size) { \
	void *ptr=malloc(size); \
	if(!ptr) { g_Output->AddMsg("x Erreur [new], objet: %s, taille: %i\n",GetMClassName(),GetMClassSize()); return 0; } \
	g_MemDebugger.AddObjNew(GetMClassName(),size); return ptr; }

#define MEMDEBUG_DELETE				void operator delete(void *ptr) { \
	if(!ptr) { g_Output->AddMsg("x Erreur [delete], objet: %s, taille: %i\n",GetMClassName(),GetMClassSize()); return; } \
	free(ptr); g_MemDebugger.AddObjDel(GetMClassName()); }

#define MEMDEBUG_MNEW				void *operator new[](size_t size) { \
	void *ptr=malloc(size); \
	if(!ptr) { g_Output->AddMsg("x Erreur [new[]], objet: %s, taille: %i, nb: %i\n",GetMClassName(),GetMClassSize(),size / GetMClassSize()); return 0; } \
	g_MemDebugger.AddObjMNew(GetMClassName(),size / GetMClassSize(),GetMClassSize()); return ptr; }

#define MEMDEBUG_MDELETE			void operator delete[](void *ptr) { \
	if(!ptr) { g_Output->AddMsg("x Erreur [delete[]], objet: %s, taille: %i, nb: %i\n",GetMClassName(),GetMClassSize(),_msize(ptr) / GetMClassSize()); return; } \
	free(ptr); g_MemDebugger.AddObjMDel(GetMClassName(),_msize(ptr) / GetMClassSize()); }

//#define MEMDEBUG_CLASSSIZE			int GetClassSize() const { return sizeof(this); }

#define MEMDEBUG_IMPL				MEMDEBUG_NEW \
									MEMDEBUG_DELETE \
									MEMDEBUG_MNEW \
									MEMDEBUG_MDELETE 

#define MEMDEBUG_CLASSNAME(x)		static const char *GetMClassName() { return x; }
#define MEMDEBUG_CLASSSIZE(x)		static int GetMClassSize() { return x; }

#define MEMDEBUG_ADDEXTRASIZE(x)	g_MemDebugger.AddObjExtraSize(GetMClassName(),x)
#define MEMDEBUG_REMEXTRASIZE(x)	g_MemDebugger.RemObjExtraSize(GetMClassName(),x)

#else

#define MEMDEBUG_IMPL
#define MEMDEBUG_CLASSNAME(x)
#define MEMDEBUG_CLASSSIZE(x)
#define MEMDEBUG_ADDEXTRASIZE(x)
#define MEMDEBUG_REMEXTRASIZE(x)

#endif
#endif