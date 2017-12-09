
#ifndef _ICONTEXTMENU_H_

#define _ICONTEXTMENU_H_

#include "IElement.h"

namespace GUI
{

class IContextMenu : public IElement
{
	public:

		IContextMenu(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_CONTEXTMENU,pEnvironment,pParent,pUser,dwID,rect)  {};
		virtual ~IContextMenu() {};

		virtual uint32 GetItemCount() const=0;
		virtual uint32 AddItem(const char *szText, int iCommandID=-1, bool bEnable=true,
			bool bHasSubMenu=false, bool bChecked=false)=0;
		virtual void AddSeparator()=0;
		
		virtual const char *GetItemText(uint32 dwIndex) const=0;
		virtual void SetItemText(uint32 dwIndex, const char *szText)=0;

		virtual bool IsItemEnabled(uint32 dwIndex) const=0;
		virtual void SetItemEnabled(uint32 dwIndex, bool bEnable)=0;

		virtual void SetItemChecked(uint32 dwIndex, bool bEnable)=0;
		virtual bool IsItemChecked(uint32 dwIndex) const=0;

		virtual void RemoveItem(uint32 dwIndex)=0;
		virtual void RemoveAllItems()=0;

		virtual int GetSelectedItem() const=0;
		
		virtual int GetItemCommandID(uint32 dwIndex) const=0;
		virtual void SetItemCommandID(uint32 dwIndex, int iID)=0;

		virtual IContextMenu *GetSubMenu(uint32 dwIndex) const=0;

		virtual void SetAllowFocus(bool bAllow)=0;
};

}

#endif