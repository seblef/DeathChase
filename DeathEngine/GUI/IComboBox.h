
#ifndef _ICOMBOBOX_H_

#define _ICOMBOBOX_H_

#include "IElement.h"

namespace GUI
{

class IComboBox : public IElement
{
	public:

		IComboBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_COMBOBOX,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IComboBox() {}

		virtual uint32 GetItemCount() const=0;
		virtual const char *GetItem(uint32 dwIndex) const=0;
		virtual uint32 AddItem(const char *szText)=0;
		virtual void RemoveItem(uint32 dwIndex)=0;

		virtual void ClearList()=0;
		virtual int GetSelected() const=0;
		virtual void SetSelected(int iIndex)=0;
};

}

#endif