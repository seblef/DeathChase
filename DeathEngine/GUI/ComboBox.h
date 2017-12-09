
#ifndef _COMBOBOX_H_

#define _COMBOBOX_H_

#include "IComboBox.h"
#include <string>
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class IButton;
class IListBox;

class ComboBox : public IComboBox
{
	public:

		ComboBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual uint32 GetItemCount() const;
		virtual const char *GetItem(uint32 dwIndex) const;
		virtual uint32 AddItem(const char *szText);
		virtual void RemoveItem(uint32 dwIndex);

		virtual void ClearList();

		virtual const char *GetText() const;
		virtual int GetSelected() const;
		virtual void SetSelected(int iIndex);

		virtual void UpdateAbsolutePosition();

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

	  MEMDEBUG_IMPL
	  MEMDEBUG_CLASSNAME("ComboBox")
	  MEMDEBUG_CLASSSIZE(sizeof(ComboBox))

	private:

		void OpenCloseMenu();
		void SendSelectionChangedEvent();

		IButton *m_pListButton;
		IListBox *m_pListBox;

		std::vector<std::string> m_Items;
		int m_iSelected;
		bool m_bHasFocus;
		IElement *m_pLastFocus;
};

}

#endif
