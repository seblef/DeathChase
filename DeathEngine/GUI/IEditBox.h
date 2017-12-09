
#ifndef _IEDITBOX_H_

#define _IEDITBOX_H_

#include "IElement.h"
#include "../Core/Color.h"

namespace GUI
{

class IFont;

class IEditBox : public IElement
{
	public:

		IEditBox(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,uint32 dwID, Core::Rect rect) :
		  IElement(ET_EDITBOX,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IEditBox() {};

		virtual void SetOverrideFont(IFont *pFont=0)=0;
		virtual void SetOverrideColor(Core::Color32 cColor)=0;

		virtual void EnableOverrideColor(bool bEnable)=0;

		virtual void SetDrawBorder(bool bBorder)=0;
		virtual void SetTextAlignment(ALIGNMENT Horizontal, ALIGNMENT Vertical)=0;
	
		virtual void SetWordWrap(bool bEnable)=0;
		virtual bool IsWordWrapEnabled() const=0;

		virtual void SetMultiLine(bool bEnable)=0;
		virtual bool IsMultiLineEnabled() const=0;

		virtual void SetAutoScroll(bool bEnable)=0;
		virtual bool IsAutoScrollEnabled() const=0;

		virtual void SetPasswordBox(bool bPasswordBox, char cPasswordChar='*')=0;
		virtual bool IsPasswordBox() const=0;

		virtual Core::Vector2i GetTextDimension()=0;

		virtual void SetMax(uint32 dwMax)=0;
		virtual uint32 GetMax() const=0;
};

}

#endif