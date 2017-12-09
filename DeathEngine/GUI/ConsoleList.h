
#ifdef DEDEV

#ifndef _CONSOLELIST_H_
#define _CONSOLELIST_H_

#include "ListBox.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class ConsoleList : public ListBox
{
	public:

		ConsoleList(int iLineCount, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect, bool bClip=true,
			bool bDrawBack=false, bool bMoveOverSelect=false);
		virtual ~ConsoleList();

		virtual void Draw();
		virtual uint32 AddItem(const char *szText);
		virtual uint32 AddItem(const char *szText, int iIcon);
		virtual void ClearList();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("ConsoleList")
		MEMDEBUG_CLASSSIZE(sizeof(ConsoleList))

	protected:

		int m_iOutPos;
		int m_iLineCount;

};

}

#endif
#endif

