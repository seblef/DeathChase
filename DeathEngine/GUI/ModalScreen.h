
#ifndef _MODALSCREEN_H_

#define _MODALSCREEN_H_

#include "IElement.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class ModalScreen : public IElement
{
	public:

		ModalScreen(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID);

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID);
		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);

		virtual void RemoveChild(IElement *pChild, bool bDelete);
		virtual void AddChild(IElement *pChild);

		virtual void Draw();
		virtual void UpdateAbsolutePosition();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("ModalScreen")
		MEMDEBUG_CLASSSIZE(sizeof(ModalScreen))

	private:

		uint32 m_dwMouseDownTime;
};

}

#endif