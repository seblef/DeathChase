
#include "CheckBox.h"
#include "IEnvironment.h"
#include "ISkin.h"
#include "IFont.h"
#include <dinput.h>

namespace GUI
{

CheckBox::CheckBox(bool bChecked, IEnvironment *pEnvironment,IElement *pParent,
				   IEventReceiver *pUser, uint32 dwID, Core::Rect rect) :
	ICheckBox(pEnvironment,pParent,pUser,dwID,rect), m_bPressed(false), m_bChecked(bChecked),
		m_dwCheckTime(0)
{
	SetTabStop(true);
	SetTabOrder(-1);
}

void CheckBox::Reset(bool bChecked, IEnvironment *pEnvironment,
				   IElement *pParent, IEventReceiver *pUser, uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser, dwID,rect);

	m_bPressed=false;
	m_bChecked=bChecked;
	m_dwCheckTime=0;

	SetTabStop(true);
	SetTabOrder(-1);
}

void CheckBox::Clear()
{
	IElement::Clear();
}

bool CheckBox::OnEvent(const Event& _Event)
{
	switch(_Event.EventType)
	{
		case ET_KEY:
			if(_Event.sKey.bPressed &&
				(_Event.sKey.cDXChar==DIK_RETURN ||
				_Event.sKey.cDXChar==DIK_SPACE))
			{
				m_bPressed=true;
				return true;
			}
			else if(m_bPressed && _Event.sKey.bPressed && _Event.sKey.cDXChar==DIK_ESCAPE)
			{
				m_bPressed=false;
				return true;
			}
			else if(!_Event.sKey.bPressed && m_bPressed &&
				(_Event.sKey.cDXChar==DIK_RETURN ||
				_Event.sKey.cDXChar==DIK_SPACE))
			{
				m_bPressed=false;
				Event newEvent;
				newEvent.EventType=ET_GUI;
				newEvent.sGUI.pCaller=this;
				newEvent.sGUI.pElement=0;
				m_bChecked=!m_bChecked;
				newEvent.sGUI.EventType=GE_CHECKBOXCHANGED;
				SendEvent(newEvent);
				
				return true;
			}
			break;

		case ET_GUI:
			if(_Event.sGUI.EventType==GE_FOCUSLOST)
			{
				if(_Event.sGUI.pCaller==this)
					m_bPressed=false;
			}
			break;

		case ET_MOUSE:
			if(_Event.sMouse.EventType==ME_LMOUSEDOWN)
			{
				m_bPressed=true;
				m_dwCheckTime=timeGetTime();
				m_pEnvironment->SetFocus(this);
				return true;
			}
			else if(_Event.sMouse.EventType==ME_LMOUSEUP)
			{
				bool bWasPressed=m_bPressed;
				m_pEnvironment->RemoveFocus(this);
				m_bPressed=false;

				if(bWasPressed && m_pParent)
				{
					if(!m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY)))
					{
						m_bPressed=false;
						return true;
					}

					Event newEvent;
					newEvent.EventType=ET_GUI;
					newEvent.sGUI.pCaller=this;
					newEvent.sGUI.pElement=0;
					m_bChecked=!m_bChecked;
					newEvent.sGUI.EventType=GE_CHECKBOXCHANGED;
					SendEvent(newEvent);
			}
				
				return true;
			}
			break;

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void CheckBox::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	Core::Rect rect(m_AbsoluteRect);

	int height=skin->GetSize(DS_CHECKBOXWIDTH);

	Core::Rect checkrect(m_AbsoluteRect.m_vUpperLeft.x,
		((m_AbsoluteRect.GetHeight() - height) / 2) + m_AbsoluteRect.m_vUpperLeft.y,
		0,0);

	checkrect.m_vLowerRight.x=checkrect.m_vUpperLeft.x + height;
	checkrect.m_vLowerRight.y=checkrect.m_vUpperLeft.y + height;

	skin->Draw3DSunkenPane(this,skin->GetColor(m_bPressed ? DC_3DFACE : DC_ACTIVECAPTION),
		false,true,checkrect,&m_AbsoluteClippingRect);

	if(m_bChecked && m_pEnvironment->GetSkin())
		m_pEnvironment->GetSkin()->DrawIcon(this,DI_CHECKBOXCHECKED,checkrect.GetCenter(),
		m_dwCheckTime,timeGetTime(),false,&m_AbsoluteClippingRect);

	if(m_strText.size())
	{
		checkrect=m_AbsoluteRect;
		checkrect.m_vUpperLeft.x+=height+5;

		IFont *font=skin->GetFont();
		if(font)
			font->Draw(m_strText.c_str(),checkrect,skin->GetColor(DC_BUTTONTEXT),false,true,&m_AbsoluteClippingRect);
	}

	IElement::Draw();
}

void CheckBox::SetChecked(bool bChecked)
{
	m_bChecked=bChecked;
}

bool CheckBox::IsChecked() const
{
	return m_bChecked;
}

}