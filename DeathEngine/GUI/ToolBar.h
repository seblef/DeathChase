
#ifndef _TOOLBAR_H_

#define _TOOLBAR_H_

#include "IToolBar.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class ToolBar : public IToolBar
{
	public:

		ToolBar(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual void UpdateAbsolutePosition();

		virtual IButton *AddButton(uint32 dwID=INVALID_ID, const char *szText=0,
			const char *szToolTipText=0, Core::HTexture hTex=Core::HTexture(),
			Core::HTexture hPressedTex=Core::HTexture(),
			bool bIsPush=false, bool bUseAlphaChannel=false);

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("ToolBar")
		MEMDEBUG_CLASSSIZE(sizeof(ToolBar))

	private:

		int m_iButtonX;
};

}

#endif

