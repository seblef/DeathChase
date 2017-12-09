
#ifndef _MESSAGEBOX_H_

#define _MESSAGEBOX_H_

#include "Window.h"
#include "IStaticText.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class CMessageBox : public Window
{
	public:

		CMessageBox(IEnvironment *pEnvironment, const char *szCaption,
			const char *szText, int iFlags,IElement *pParent, IEventReceiver *pUser, uint32 dwID, Core::Rect rect);
		virtual ~CMessageBox();

		virtual void Reset(IEnvironment *pEnvironment, const char *szCaption,
			const char *szText, int iFlags, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);

	  MEMDEBUG_IMPL
	  MEMDEBUG_CLASSNAME("MessageBox")
	  MEMDEBUG_CLASSSIZE(sizeof(CMessageBox))

	private:

		void RefreshControls();

		IButton *m_pOK;
		IButton *m_pCancel;
		IButton *m_pYes;
		IButton *m_pNo;
		IStaticText *m_pText;

		int m_iFlags;
		std::string m_strMsgText;
		bool m_bPressed;
};

}

#endif
