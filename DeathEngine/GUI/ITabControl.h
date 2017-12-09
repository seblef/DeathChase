
#ifndef _ITABCONTROL_H_

#define _ITABCONTROL_H_

#include "IElement.h"
#include "../Core/Color.h"

namespace GUI
{

class ITab : public IElement
{
	public:

		ITab(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_TAB,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~ITab() {};

		virtual int GetNumber() const=0;

		virtual void SetDrawBackground(bool bDraw=true)=0;
		virtual void SetBackgroundColor(Core::Color32 cColor)=0;
		virtual bool IsDrawingBackground() const=0;
		virtual Core::Color32 GetBackgroundColor() const=0;

};

class ITabControl : public IElement
{
	public:

		ITabControl(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_TABCONTROL,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~ITabControl() {};

		virtual ITab *AddTab(const char *szCaption, int iID=-1)=0;
		virtual int GetTabCount() const=0;
		virtual ITab *GetTab(int iIndex) const=0;

		virtual bool SetActiveTab(int iIndex)=0;
		virtual bool SetActiveTab(IElement *pTab)=0;
		virtual int GetActiveTab() const=0;
};

}

#endif
