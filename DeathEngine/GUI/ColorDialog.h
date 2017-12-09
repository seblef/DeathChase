
#ifndef _COLORDIALOG_H_

#define _COLORDIALOG_H_

#include "IColorDialog.h"
#include "IButton.h"
#include "IEditBox.h"
#include "IScrollBar.h"
#include "IImage.h"
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class ColorDialog : public IColorDialog
{
	public:

		ColorDialog(const char *szTitle, IEnvironment *pEnvironment,
			IElement *pParent,  IEventReceiver *pUser, uint32 dwID);
		virtual ~ColorDialog();

		virtual void Reset(const char *szTitle, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, uint32 dwID);
		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

	  MEMDEBUG_IMPL
	  MEMDEBUG_CLASSNAME("ColorDialog")
	  MEMDEBUG_CLASSSIZE(sizeof(ColorDialog))

	private:

		void SendSelectedEvent();
		void SendCancelEvent();

		Core::Vector2i m_vDragStart;
		bool m_bDragging;
		IButton *m_pClose;
		IButton *m_pOK;
		IButton *m_pCancel;

		struct BatteryItem
		{
			float fIncoming;
			float fOutgoing;
			IEditBox *pEdit;
			IScrollBar *pScroll;
		};
		std::vector<BatteryItem> m_Battery;

		struct ColorCircle
		{
			IImage *pControl;
			Core::HTexture hTexture;
		};
		ColorCircle m_ColorRing;

		void BuildColorRing(const Core::Vector2i& dim, int supersamble, const uint32 bordercolor);
};

}

#endif