
#ifndef _ILISTBOX_H_

#define _ILISTBOX_H_

#include "IElement.h"
#include "../Core/Color.h"

namespace GUI
{

class IFont;
class ISpriteBank;

enum LISTBOX_COLOR
{
	LBC_TEXT=0,
	LBC_TEXTHIGHLIGHT,
	LBC_ICON,
	LBC_ICONHIGHLIGHT,

	LBC_COUNT

};

class IListBox : public IElement
{
	public:

		IListBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_LISTBOX,pEnvironment, pParent, pUser, dwID, rect) {};
		virtual ~IListBox() {};

		virtual uint32 GetItemCount() const=0;
		virtual const char *GetListItem(uint32 dwIndex) const=0;
		virtual uint32 AddItem(const char *szText)=0;
		virtual uint32 AddItem(const char *szText, int iIcon)=0;
		virtual void RemoveItem(uint32 dwIndex)=0;

		virtual int GetIcon(uint32 dwIndex) const=0;

		virtual void SetSpriteBank(ISpriteBank *pBank)=0;

		virtual void ClearList()=0;

		virtual int GetSelected() const=0;
		virtual void SetSelected(int iIndex)=0;

		virtual void SetAutoScrollEnable(bool bScroll)=0;
		virtual bool IsAutoScrollEnable() const=0;

		virtual void SetItemOverrideColor(uint32 dwIndex, const Core::Color32& cColor)=0;
		virtual void SetItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType, const Core::Color32& cColor)=0;
		virtual void ClearItemOverrideColor(uint32 dwIndex)=0;
		virtual void ClearItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType)=0;
		virtual bool HasItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType)const =0;
		virtual Core::Color32 GetItemOverrideColor(uint32 dwIndex, LISTBOX_COLOR ColorType) const=0;
		virtual Core::Color32 GetItemDefaultColor(LISTBOX_COLOR ColorType) const=0;

		virtual void SetItem(uint32 dwIndex, const char *szText, int iIcon)=0;
		virtual int InsertItem(uint32 dwIndex, const char *szText, int iIcon)=0;
		virtual void SwapItems(uint32 dwIndex1, uint32 dwIndex2)=0;

		virtual void SetMoveOverSelect(bool bMove)=0;
};

}

#endif

