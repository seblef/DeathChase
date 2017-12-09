
#ifndef _IMULTILISTBOX_H_
#define _IMULTILISTBOX_H_

#include "IElement.h"
#include <vector>
#include "IFont.h"

namespace GUI
{

class IFont;

typedef std::vector<std::string> MultiListBoxVector;

class IMultiListBox : public IElement
{
	public:

		IMultiListBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_MULTILISTBOX,pEnvironment, pParent, pUser, dwID, rect) {};
		virtual ~IMultiListBox() {};

		virtual uint32 AddItem(MultiListBoxVector *pItem)=0;

		virtual void ClearList()=0;

		virtual void SetAutoScrollEnable(bool bScroll)=0;
		virtual bool IsAutoScrollEnable() const=0;

};

}

#endif

