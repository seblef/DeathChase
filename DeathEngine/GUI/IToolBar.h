
#ifndef _ITOOLBAR_H_

#define _ITOOLBAR_H_

#include "IElement.h"
#include "../Core/Handles.h"

namespace GUI
{

class IButton;

class IToolBar : public IElement
{
	public:

		IToolBar(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_TOOLBAR,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IToolBar() {};

		virtual IButton *AddButton(uint32 dwID=INVALID_ID, const char *szText=0,
			const char *szToolTipText=0, Core::HTexture hTex=Core::HTexture(),
			Core::HTexture hPressedTex=Core::HTexture(),
			bool bIsPush=false, bool bUseAlphaChannel=false)=0;
};

}

#endif