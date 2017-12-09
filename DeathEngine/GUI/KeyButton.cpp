
#include "KeyButton.h"
#include "ISkin.h"
#include "IFont.h"
#include "IEnvironment.h"
#include "../Core/Renderer2D.h"
#include "../Core/Input.h"

namespace GUI
{

KeyButton::KeyButton(IEnvironment *pEnvironment,
			   IElement *pParent, IEventReceiver *pUser,
			   uint32 dwID, Core::Rect rect, bool bNoClip) :
	IKeyButton(pEnvironment,pParent,pUser,dwID,rect), m_bPressed(false),m_byKey(0)
{
	SetTabStop(true);
	SetTabOrder(-1);
}

KeyButton::~KeyButton()
{
}

void KeyButton::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
				   uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser, dwID,rect);

	m_bPressed=false;
	m_byKey=0;

	SetTabStop(true);
	SetTabOrder(-1);
}

void KeyButton::Clear()
{
	IElement::Clear();
}

bool KeyButton::OnEvent(const Event& _Event)
{
	if(!m_bIsEnable)
		return m_pParent ? m_pParent->OnEvent(_Event) : false;

	switch(_Event.EventType)
	{
		case ET_KEY:
			if(m_bPressed && _Event.sKey.bPressed)
			{
				SetKey((byte)_Event.sKey.cDXChar);
				m_bPressed=false;

				Event e;
				e.EventType=ET_GUI;
				e.sGUI.EventType=GE_KEYSELECTED;
				e.sGUI.pCaller=this;
				e.sGUI.pElement=0;
				SendEvent(e);

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
				if(m_pEnvironment->HasFocus(this) &&
					!m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY)))
				{
					m_pEnvironment->RemoveFocus(this);
					return false;
				}

				m_pEnvironment->SetFocus(this);
				m_bPressed=!m_bPressed;
				return true;
			}
			break;

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void KeyButton::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	IFont *font=skin->GetFont();

	Core::Rect rect=m_AbsoluteRect;
	Core::Vector2i spritepos(m_AbsoluteRect.GetCenter());

	if(!m_bPressed)
		skin->Draw3DButtonPaneStandard(this,rect,&m_AbsoluteClippingRect);
	else
		skin->Draw3DButtonPanePressed(this,rect,&m_AbsoluteClippingRect);

	if(m_strText.size())
	{
		rect=m_AbsoluteRect;
		if(m_bPressed)
			rect.m_vUpperLeft.y+=2;

		if(font)
			font->Draw(m_strText.c_str(),rect,
			skin->GetColor(m_bIsEnable ? DC_BUTTONTEXT : DC_GRAYTEXT),true,true,&m_AbsoluteClippingRect);
	}

	IElement::Draw();
}

void KeyButton::SetKey(const byte byKey)
{
	m_byKey=byKey;
	SetText(g_Input.GetKeyName(byKey));
}


}