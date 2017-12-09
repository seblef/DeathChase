
#ifndef _MULTILISTBOX_H_
#define _MULTILISTBOX_H_

#include "IMultiListBox.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class IScrollBar;

class MultiListBox : public IMultiListBox
{
	public:

		MultiListBox(int iColumnCount,int *pCWidth, IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect& rect);
		~MultiListBox();

		virtual uint32 AddItem(MultiListBoxVector *pItem);

		virtual void ClearList();

		virtual void SetAutoScrollEnable(bool bScroll);
		virtual bool IsAutoScrollEnable() const;

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual void UpdateAbsolutePosition();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("MultiListBox")
		MEMDEBUG_CLASSSIZE(sizeof(MultiListBox))

	protected:

		void RecalculateItemHeight();

		std::vector<MultiListBoxVector> m_Items;
		int m_iItemHeight,m_iTotalItemHeight;
		IFont *m_pFont;
		IScrollBar *m_pScrollBar;
		bool m_bAutoScroll;
		int m_iColumnCount;
		std::vector<int> m_ColumnsWidth;	

};

}

#endif