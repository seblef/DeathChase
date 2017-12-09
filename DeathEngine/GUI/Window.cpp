
#include "Window.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IButton.h"
#include "IFont.h"

namespace GUI
{

Window::Window(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
			   uint32 dwID, Core::Rect rect) :
	IWindow(pEnvironment,pParent,pUser, dwID,rect), m_bDragging(false), m_bMinimized(false)
{
	ISkin *skin=0;
	if(pEnvironment)
		skin=m_pEnvironment->GetSkin();

	ISpriteBank *sprites=0;
	Core::Color32 color(255,255,255,255);

	int buttonw=15;
	if(skin)
	{
		buttonw=skin->GetSize(DS_WINDOWBUTTONWIDTH);
		sprites=skin->GetSpriteBank();
		color=skin->GetColor(DC_WINDOWSYMBOL);
	}
	int posx=m_RelativeRect.GetWidth() - buttonw - 4;

	m_pClose=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWCLOSE) : "Fermer");
	m_pClose->SetSubElement(true);
	m_pClose->SetTabStop(false);
	m_pClose->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);
	if(sprites)
	{
		m_pClose->SetSpriteBank(sprites);
		m_pClose->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWCLOSE),color);
		m_pClose->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWCLOSE),color);
	}
	posx-=buttonw+2;

	m_pRestore=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWRESTORE) : "Restaurer");
	m_pRestore->SetVisible(true);
	m_pRestore->SetEnable(false);
	m_pRestore->SetSubElement(true);
	m_pRestore->SetTabStop(false);
	m_pRestore->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);
	if(sprites)
	{
		m_pRestore->SetSpriteBank(sprites);
		m_pRestore->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWRESTORE),color);
		m_pRestore->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWRESTORE),color);
	}
	posx-=buttonw+2;

	m_pMin=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWMINIMIZE) : "Minimiser");
	m_pMin->SetVisible(true);
	m_pMin->SetSubElement(true);
	m_pMin->SetTabStop(false);
	m_pMin->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);
	if(sprites)
	{
		m_pMin->SetSpriteBank(sprites);
		m_pMin->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWMINIMIZE),color);
		m_pMin->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWMINIMIZE),color);
	}

	SetTabGroup(true);
	SetTabStop(true);
	SetTabOrder(-1);
}

Window::~Window()
{
	RemoveChild(m_pMin,true);
	RemoveChild(m_pRestore,true);
	RemoveChild(m_pClose,true);
}

void Window::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
				   uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	ISkin *skin=0;
	if(pEnvironment)
		skin=m_pEnvironment->GetSkin();

	ISpriteBank *sprites=0;
	Core::Color32 color(255,255,255,255);

	int buttonw=15;
	if(skin)
	{
		buttonw=skin->GetSize(DS_WINDOWBUTTONWIDTH);
		sprites=skin->GetSpriteBank();
		color=skin->GetColor(DC_WINDOWSYMBOL);
	}
	int posx=m_RelativeRect.GetWidth() - buttonw - 4;

	m_pClose=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWCLOSE) : "Fermer");
	m_pClose->SetSubElement(true);
	m_pClose->SetTabStop(false);
	m_pClose->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);
	if(sprites)
	{
		m_pClose->SetSpriteBank(sprites);
		m_pClose->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWCLOSE),color);
		m_pClose->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWCLOSE),color);
	}
	posx-=buttonw+2;

	m_pRestore=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWRESTORE) : "Restaurer");
	m_pRestore->SetVisible(false);
	m_pRestore->SetSubElement(true);
	m_pRestore->SetTabStop(false);
	m_pRestore->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);
	if(sprites)
	{
		m_pRestore->SetSpriteBank(sprites);
		m_pRestore->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWRESTORE),color);
		m_pRestore->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWRESTORE),color);
	}
	posx-=buttonw+2;

	m_pMin=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWMINIMIZE) : "Minimiser");
	m_pMin->SetVisible(false);
	m_pMin->SetSubElement(true);
	m_pMin->SetTabStop(false);
	m_pMin->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);
	if(sprites)
	{
		m_pMin->SetSpriteBank(sprites);
		m_pMin->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWMINIMIZE),color);
		m_pMin->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWMINIMIZE),color);
	}

	SetTabGroup(true);
	SetTabStop(true);
	SetTabOrder(-1);
}

void Window::Clear()
{
	RemoveChild(m_pMin,true);
	RemoveChild(m_pRestore,true);
	RemoveChild(m_pClose,true);

	m_pMin=0;
	m_pRestore=0;
	m_pClose=0;

	IElement::Clear();
}

bool Window::OnEvent(const Event &_Event)
{
	switch(_Event.EventType)
	{
		case ET_GUI:
			if(_Event.sGUI.EventType==GE_FOCUSLOST)
				m_bDragging=false;
			else if(_Event.sGUI.EventType==GE_FOCUSED)
			{
				if(_Event.sGUI.pCaller==this && m_pParent)
					m_pParent->BringToFront(this);
			}
			else if(_Event.sGUI.EventType==GE_BUTTONCLICKED)
			{
				if(_Event.sGUI.pCaller==m_pClose)
				{
					Event e;
					e.EventType=ET_GUI;
					e.sGUI.pCaller=this;
					e.sGUI.pElement=0;
					e.sGUI.EventType=GE_CLOSED;
					SendEvent(e);

					return true;
				}
				else if(_Event.sGUI.pCaller==m_pMin && !m_bMinimized)
					Minimize();
				else if(_Event.sGUI.pCaller==m_pRestore && m_bMinimized)
					Restore();
			}
			break;

		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_LMOUSEDOWN:
					m_vDragStart.x=(int)_Event.sMouse.dwX;
					m_vDragStart.y=(int)_Event.sMouse.dwY;
					m_bDragging=true;
					if(!m_pEnvironment->HasFocus(this))
					{
						m_pEnvironment->SetFocus(this);
						if(m_pParent)
							m_pParent->BringToFront(this);
					}
					return true;

				case ME_LMOUSEUP:
					m_bDragging=false;
					return true;

				case ME_MOVED:
					if(m_bDragging)
					{
						if(m_pParent)
							if((int)_Event.sMouse.dwX < m_pParent->GetAbsolutePosition().m_vUpperLeft.x+1 ||
							   (int)_Event.sMouse.dwY < m_pParent->GetAbsolutePosition().m_vUpperLeft.y+1 ||
							   (int)_Event.sMouse.dwX > m_pParent->GetAbsolutePosition().m_vLowerRight.x-1 ||
							   (int)_Event.sMouse.dwY > m_pParent->GetAbsolutePosition().m_vLowerRight.y-1)
							   return true;

						Move(Core::Vector2i((int)_Event.sMouse.dwX - m_vDragStart.x,(int)_Event.sMouse.dwY - m_vDragStart.y));

						if(m_pUserReceiver)
						{
							Event e;
							e.EventType=ET_GUI;
							e.sGUI.EventType=GE_MOVED;
							e.sGUI.pCaller=this;
							e.sGUI.pElement=0;
							if(m_pUserReceiver)
								m_pUserReceiver->OnEvent(e);
						}

						m_vDragStart.x=(int)_Event.sMouse.dwX;
						m_vDragStart.y=(int)_Event.sMouse.dwY;
						return true;
					}
					break;
				default:
					break;
			}
		default:
			break;
	}

	return IElement::OnEvent(_Event);
}

void Window::UpdateAbsolutePosition()
{
	IElement::UpdateAbsolutePosition();
}

void Window::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();

	Core::Rect rect(m_AbsoluteRect);
	Core::Rect *clip=&m_AbsoluteClippingRect;

	rect=skin->Draw3DWindowBackground(this,true,skin->GetColor(DC_ACTIVEBORDER),
		m_AbsoluteRect,m_bMinimized,&m_AbsoluteClippingRect);

	if(m_strText.size())
	{
		rect.m_vUpperLeft.x+=skin->GetSize(DS_TEXTDISTANCEX);
		rect.m_vUpperLeft.y+=skin->GetSize(DS_TEXTDISTANCEY);
		rect.m_vLowerRight.x-=skin->GetSize(DS_WINDOWBUTTONWIDTH) + 5;

		IFont *font=skin->GetFont(DF_WINDOW);
		if(font)
			font->Draw(m_strText.c_str(),rect,skin->GetColor(DC_ACTIVECAPTION),false,true,clip);
	}

	IElement::Draw();
}

IButton *Window::GetCloseButton() const
{
	return m_pClose;
}

IButton *Window::GetMaximizeButton() const
{
	return m_pRestore;
}

IButton *Window::GetMinimizeButton() const
{
	return m_pMin;
}

bool Window::IsMinimized() const
{
	return m_bMinimized;
}

void Window::Minimize()
{
	if(m_bMinimized)
		return;

	m_pMin->SetEnable(false);
	m_pRestore->SetEnable(true);

	m_SavedVisible.clear();

	std::list<IElement *>::iterator i;
	for(i=m_Children.begin();i!=m_Children.end();i++)
	{
		m_SavedVisible.push_back((*i)->IsVisible());
		(*i)->SetVisible(false);
	}

	m_pMin->SetVisible(true);
	m_pRestore->SetVisible(true);

	m_bMinimized=true;

	Event e;
	e.EventType=ET_GUI;
	e.sGUI.EventType=GE_MINIMIZE;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;
	SendEvent(e);
}

void Window::Restore()
{
	if(!m_bMinimized)
		return;

	m_pMin->SetEnable(true);
	m_pRestore->SetEnable(false);

	std::list<IElement *>::iterator i;
	std::vector<bool>::iterator b=m_SavedVisible.begin();

	for(i=m_Children.begin();i!=m_Children.end();i++,b++)
		(*i)->SetVisible(*b);

	m_bMinimized=false;

	Event e;
	e.EventType=ET_GUI;
	e.sGUI.EventType=GE_RESTORE;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;
	SendEvent(e);
}

bool Window::IsPointInside(const Core::Vector2i &p) const
{
	if(m_bMinimized)
	{
		Core::Rect r(m_AbsoluteClippingRect);
		r.m_vLowerRight.y = r.m_vUpperLeft.y + m_pEnvironment->GetSkin()->GetSize(DS_WINDOWBUTTONWIDTH) + 6;

		r.ClipAgainst(m_AbsoluteClippingRect);
		return r.IsPointInside(p);
	}
	else
		return m_AbsoluteClippingRect.IsPointInside(p);
}

}