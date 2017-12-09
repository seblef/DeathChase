
#ifndef _LISTBOX_H_

#define _LISTBOX_H_

#include "IListBox.h"
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class IFont;
class IScrollBar;

class ListBox : public IListBox
{
	public:

		ListBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect, bool bClip=true,
			bool bDrawBack=false, bool bMoveOverSelect=false);
		virtual ~ListBox();

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect, bool bClip=true,
			bool bDrawBack=false, bool bMoveOverSelect=false);
		virtual void Clear();

		virtual uint32 GetItemCount() const;
		virtual const char *GetListItem(uint32 dwIndex) const;
		virtual uint32 AddItem(const char *szText);

		virtual void ClearList();

		virtual int GetSelected() const;
		virtual void SetSelected(int iIndex);

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual uint32 AddItem(const char *szText, int iIcon);
		virtual int GetIcon(uint32 dwIndex) const;
		virtual void RemoveItem(uint32 dwIndex);

		virtual void SetSpriteBank(ISpriteBank *pBank);

		virtual void SetAutoScrollEnable(bool bScroll);
		virtual bool IsAutoScrollEnable() const;

		virtual void UpdateAbsolutePosition();

		virtual void SetItemOverrideColor(uint32 dwIndex, const Core::Color32& cColor);
		virtual void SetItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType, const Core::Color32& cColor);
		virtual void ClearItemOverrideColor(uint32 dwIndex);
		virtual void ClearItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType);
		virtual bool HasItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType) const;
		virtual Core::Color32 GetItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType) const;
		virtual Core::Color32 GetItemDefaultColor(LISTBOX_COLOR ColorType) const;

		virtual void SetItem(uint32 dwIndex, const char *szText, int iIcon);
		virtual int InsertItem(uint32 dwIndex, const char *szText, int iIcon);

		virtual void SwapItems(uint32 dwIndex1, uint32 dwIndex2);

		virtual void SetMoveOverSelect(bool bMove) { m_bMoveOverSelect=bMove; };

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("ListBox")
		MEMDEBUG_CLASSSIZE(sizeof(ListBox))

	protected:

		struct ListItem
		{
			ListItem() : iIcon(-1) {}

			std::string strText;
			int iIcon;

			struct ListItemOverrideColor
			{
				ListItemOverrideColor() : bUse(false) {}
				bool bUse;
				Core::Color32 cColor;
			};
			ListItemOverrideColor OverrideColors[LBC_COUNT];
		};

		void RecalculateItemHeight();
		void SelectNew(int ypos, bool bOnlyHover=false);
		void RecalculateScrollPos();

		// extracted that function to avoid copy&paste code
		void RecalculateItemWidth(int icon);

		std::vector<ListItem> m_Items;
		int m_iSelected;
		int m_iItemHeight,m_iTotalItemHeight;
		int m_iItemsIconWidth;
		IFont *m_pFont;
		ISpriteBank *m_pIconBank;
		IScrollBar *m_pScrollBar;
		bool m_bSelecting;
		bool m_bDrawBack;
		bool m_bMoveOverSelect;
		uint32 m_dwSelectTime;
		bool m_bAutoScroll;
		std::string m_strKeyBuffer;
		uint32 m_dwLastKeyTime;
		bool m_bHighLightWhenNotFocused;
};

}

#endif