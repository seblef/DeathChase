
#ifndef _WINDOW_H_

#define _WINDOW_H_

#include "IWindow.h"
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class IButton;

class Window : public IWindow
{
	public:

		Window(IEnvironment *pEnvironment, IElement *pElement, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual ~Window();

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);
		virtual void UpdateAbsolutePosition();
		virtual void Draw();

		virtual IButton *GetCloseButton() const;
		virtual IButton *GetMinimizeButton() const;
		virtual IButton *GetMaximizeButton() const;

		virtual bool IsMinimized() const;

		virtual bool IsPointInside(const Core::Vector2i& p) const;

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Window")
		MEMDEBUG_CLASSSIZE(sizeof(Window))

	protected:

		void Minimize();
		void Restore();

		std::vector<bool> m_SavedVisible;

		Core::Vector2i m_vDragStart;
		bool m_bDragging;
		bool m_bMinimized;

		IButton *m_pClose;
		IButton *m_pMin;
		IButton *m_pRestore;
};

}

#endif
