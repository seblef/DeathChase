
#ifndef _CONTEXTMENU_H_

#define _CONTEXTMENU_H_

#include "IContextMenu.h"
#include <string>
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class ContextMenu : public IContextMenu
{
	public:

		ContextMenu(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect, bool bGetFocus=true, bool bAllowFocus=true);
		virtual ~ContextMenu();

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect, bool bGetFocus=true, bool bAllowFocus=true);
		virtual void Clear();

		virtual uint32 GetItemCount() const;

		virtual uint32 AddItem(const char *szText, int iCommandID,
			bool bEnable, bool bHasSubMenu, bool bChecked);
		virtual void AddSeparator();

		virtual const char *GetItemText(uint32 dwIndex) const;
		virtual void SetItemText(uint32 dwIndex, const char *szText);

		virtual bool IsItemEnabled(uint32 dwIndex) const;
		virtual void SetItemEnabled(uint32 dwIndex, bool bEnable);

		virtual bool IsItemChecked(uint32 dwIndex) const;
		virtual void SetItemChecked(uint32 dwIndex, bool bEnable);

		virtual void RemoveItem(uint32 dwIndex);
		virtual void RemoveAllItems();

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual int GetSelectedItem() const;

		virtual IContextMenu *GetSubMenu(uint32 dwIndex) const;
		virtual void SetSubMenu(uint32 dwIndex, ContextMenu *pMenu);

		virtual void SetVisible(const bool bVisible);

		virtual int GetItemCommandID(uint32 dwIndex) const;
		virtual void SetItemCommandID(uint32 dwIndex, int iID);

		virtual void SetAllowFocus(bool bAllow) { m_bAllowFocus=bAllow; };

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("ContextMenu")
		MEMDEBUG_CLASSSIZE(sizeof(ContextMenu))

	protected:

		void CloseAllSubMenus();
		bool HasOpenSubMenu() const;

		struct Item
		{
			Item() : strText(), pSubMenu(0) {};

			std::string strText;
			bool bIsSeparator;
			bool bEnable;
			bool bChecked;
			Core::Vector2i vDim;
			int iPosY;
			ContextMenu *pSubMenu;
			int iCommandID;
		};

		virtual void RecalculateSize();
		virtual bool Highlight(const Core::Vector2i& p, bool bCanOpenSubMenu);
		virtual uint32 SendClick(const Core::Vector2i& p);
		virtual Core::Rect GetHRect(const Item& i, const Core::Rect& absolute) const;
		virtual Core::Rect GetRect(const Item& i, const Core::Rect& absolute) const;
		void SetEventParent(IElement *pParent);

		int m_iHighLighted;
		std::vector<Item> m_Items;
		Core::Vector2i m_vPos;
		uint32 m_dwChangeTime;
		IElement *m_pEventParent;
		bool m_bAllowFocus;
};

}

#endif
