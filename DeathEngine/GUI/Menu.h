
#ifndef _MENU_H_

#define _MENU_H_

#include "ContextMenu.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class Menu : public ContextMenu
{
	public:

		Menu(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual void Draw();
		virtual bool OnEvent(const Event& _Event);
		virtual void UpdateAbsolutePosition();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Menu")
		MEMDEBUG_CLASSSIZE(sizeof(Menu))

	protected:

		virtual void RecalculateSize();
		virtual Core::Rect GetHRect(const Item& i, const Core::Rect& absolute) const;
		virtual Core::Rect GetRect(const Item& i, const Core::Rect& absolute) const;

};

}

#endif