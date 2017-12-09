
#ifndef _CHECKBOX_H_

#define _CHECKBOX_H_

#include "ICheckBox.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class CheckBox : public ICheckBox
{
	public:

		CheckBox(bool bChecked, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser, uint32 dwID, Core::Rect rect);

		virtual void Reset(bool bChecked, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser, uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual void SetChecked(bool bChecked);
		virtual bool IsChecked() const;

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

	  MEMDEBUG_IMPL
	  MEMDEBUG_CLASSNAME("CheckBox")
	  MEMDEBUG_CLASSSIZE(sizeof(CheckBox))

	private:

		bool m_bPressed;
		bool m_bChecked;
		uint32 m_dwCheckTime;
};

}

#endif
