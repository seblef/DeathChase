
#include "ModalScreen.h"
#include "IEnvironment.h"
#include "ISkin.h"
#include <windows.h>

namespace GUI
{

ModalScreen::ModalScreen(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
						 uint32 dwID) : 
	IElement(ET_MODALSCREEN, pEnvironment, pParent, pUser, dwID, pParent->GetAbsolutePosition()),
		m_dwMouseDownTime(0)
{
	SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
	SetTabGroup(true);
}

void ModalScreen::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
						uint32 dwID)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,pParent->GetAbsolutePosition());

	SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);
	SetTabGroup(true);
}

void ModalScreen::Clear()
{
	IElement::Clear();
}

bool ModalScreen::OnEvent(const Event& e)
{
	switch(e.EventType)
	{
		case ET_GUI:
			switch(e.sGUI.EventType)
			{
				case GE_FOCUSED:
					if(e.sGUI.pCaller!=this && !IsMyChild(e.sGUI.pCaller))
						m_pEnvironment->SetFocus(this);
					return false;

				case GE_FOCUSLOST:
					if(!(IsMyChild(e.sGUI.pElement) || e.sGUI.pElement==this))
					{
						m_dwMouseDownTime=timeGetTime();
						return true;
					}
					else
					{
						if(m_pUserReceiver) m_pUserReceiver->OnEvent(e);
						return IElement::OnEvent(e);
					}

				case GE_CLOSED:
					if(m_pUserReceiver) m_pUserReceiver->OnEvent(e);
					return IElement::OnEvent(e);

				default:
					break;
			}
			break;

		case ET_MOUSE:
			if(e.sMouse.EventType==ME_LMOUSEDOWN)
				m_dwMouseDownTime=timeGetTime();

		default:
			break;
	}

	IElement::OnEvent(e);

	return true;
}

void ModalScreen::Draw()
{
	ISkin *skin=m_pEnvironment->GetSkin();

	if(!skin)
		return;

	uint32 now=timeGetTime();
	if(now - m_dwMouseDownTime < 300 && (now / 70)%2)
	{
		std::list<IElement*>::iterator it=m_Children.begin();
		Core::Rect r;
		Core::Color32 c(m_pEnvironment->GetSkin()->GetColor(DC_3DHIGHLIGHT));

		for(;it!=m_Children.end();it++)
		{
			r=(*it)->GetAbsolutePosition();
			r.m_vLowerRight.x+=1;
			r.m_vLowerRight.y+=1;
			r.m_vUpperLeft.x-=1;
			r.m_vUpperLeft.y-=1;

			skin->Draw2DRectangle(this,c,r,&m_AbsoluteClippingRect);
		}
	}

	IElement::Draw();
}

void ModalScreen::RemoveChild(IElement *pChild, bool bDelete)
{
	IElement::RemoveChild(pChild,bDelete);

	if(m_Children.empty() && m_pParent)
		m_pParent->RemoveChild(this,true);
}

void ModalScreen::AddChild(IElement *pChild)
{
	IElement::AddChild(pChild);
	m_pEnvironment->SetFocus(pChild);
}

void ModalScreen::UpdateAbsolutePosition()
{
	Core::Rect parentrect(0,0,0,0);

	if(m_pParent)
	{
		parentrect=m_pParent->GetAbsolutePosition();
		m_RelativeRect.m_vUpperLeft.x=0;
		m_RelativeRect.m_vUpperLeft.y=0;
		m_RelativeRect.m_vLowerRight.x=parentrect.GetWidth();
		m_RelativeRect.m_vLowerRight.y=parentrect.GetHeight();
	}

	IElement::UpdateAbsolutePosition();
}

}
