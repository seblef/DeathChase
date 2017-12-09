
#ifndef _SCROLLBAR_H_

#define _SCROLLBAR_H_

#include "IScrollBar.h"
#include "IButton.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class ScrollBar : public IScrollBar
{
	public:

		ScrollBar(bool bHorizontal, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, uint32 dwID, Core::Rect rect, bool bNoClip=false);
		virtual ~ScrollBar();

		virtual void Reset(bool bHorizontal, IEnvironment *pEnvironment,
			IElement *pParent, IEventReceiver *pUser, uint32 dwID, Core::Rect rect, bool bNoClip=false);
		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual int GetMax() const;
		virtual void SetMax(int iMax);

		virtual int GetSmallStep() const;
		virtual void SetSmallStep(int iStep);

		virtual int GetLargeStep() const;
		virtual void SetLargeStep(int iStep);

		virtual int GetPos() const;
		virtual void SetPos(int iPos);

		virtual void UpdateAbsolutePosition();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("ScrollBar")
		MEMDEBUG_CLASSSIZE(sizeof(ScrollBar))

	private:

		void RefreshControls();
		int GetPosFromMousePos(int x, int y) const;

		IButton *m_pUp,*m_pDown;
		Core::Rect m_SliderRect;

		bool m_bDragging;
		bool m_bHorizontal;
		bool m_bDraggedBySlider;
		bool m_bTrayClick;

		int m_iPos,m_iDrawPos;
		int m_iDrawHeight;
		int m_iMax;
		int m_iSmallStep,m_iLargeStep;
		int m_iDesiredPos;
		uint32 m_dwLastChange;
};

}

#endif
