
#include "Menu.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IFont.h"

namespace GUI
{

Menu::Menu(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
		   uint32 dwID, Core::Rect rect) :
	ContextMenu(pEnvironment,pParent,pUser,dwID,rect,false,true)
{
	m_Type=ET_MENU;
	RecalculateSize();
}

void Menu::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
		   uint32 dwID, Core::Rect rect)
{
	ContextMenu::Reset(pEnvironment,pParent,pUser,dwID,rect);
	RecalculateSize();
}

void Menu::Clear()
{
	ContextMenu::Clear();
}

void Menu::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	IFont *font=skin->GetFont(DF_MENU);

	Core::Rect rect(m_AbsoluteRect);

	skin->Draw3DMenuPane(this,rect,&m_AbsoluteClippingRect);
	
	rect=m_AbsoluteRect;
	int i;

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(!m_Items[i].bIsSeparator)
		{
			rect=GetRect(m_Items[i],m_AbsoluteRect);

			if(i==m_iHighLighted && m_Items[i].bEnable)
			{
				skin->Draw3DSunkenPane(this,skin->GetColor(DC_3DDARKSHADOW),
					true,true,rect,&m_AbsoluteClippingRect);
			}

			DEFAULTCOLOR c=DC_BUTTONTEXT;

			if(i==m_iHighLighted)
				c=DC_HIGHLIGHTTEXT;

			if(!m_Items[i].bEnable)
				c=DC_GRAYTEXT;

			if(font)
				font->Draw(m_Items[i].strText.c_str(), rect,
					skin->GetColor(c),true,true,&m_AbsoluteClippingRect);
		}
	}

	IElement::Draw();
}

bool Menu::OnEvent(const Event &_Event)
{
	if(!m_bIsEnable)
		return m_pParent ? m_pParent->OnEvent(_Event) : false;

	switch(_Event.EventType)
	{
		case ET_GUI:
			switch(_Event.sGUI.EventType)
			{
				case GE_FOCUSLOST:
					if(_Event.sGUI.pCaller==this && !IsMyChild(_Event.sGUI.pElement))
					{
						CloseAllSubMenus();
						m_iHighLighted=-1;
					}
					break;

				case GE_FOCUSED:
					if(_Event.sGUI.pCaller==this && m_pParent)
						m_pParent->BringToFront(this);

					break;

				default:
					break;
			}
			break;

		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_LMOUSEDOWN:
					{
					if(!m_pEnvironment->HasFocus(this))
					{
						m_pEnvironment->SetFocus(this);
						if(m_pParent)
							m_pParent->BringToFront(this);
					}

					Core::Vector2i p((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY);
					bool shouldclosesubmenu=HasOpenSubMenu();

					if(!m_AbsoluteClippingRect.IsPointInside(p))
					{
						shouldclosesubmenu=false;
						int t=SendClick(p);
						if((t==0 || t==1) && m_pEnvironment->HasFocus(this))
							m_pEnvironment->RemoveFocus(this);
					}

					Highlight(p,true);
					if(shouldclosesubmenu)
						CloseAllSubMenus();

					return true;
					}

				case ME_MOVED:
					if(m_pEnvironment->HasFocus(this))
						Highlight(Core::Vector2i((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY),HasOpenSubMenu());
					return true;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void Menu::RecalculateSize()
{
	ISkin *skin=m_pEnvironment->GetSkin();
	IFont *font=skin->GetFont(DF_MENU);

	if(!font)
	{
		if(m_pParent && skin)
			m_RelativeRect=Core::Rect(0,0,m_pParent->GetAbsolutePosition().m_vLowerRight.x,
				skin->GetSize(DS_MENUHEIGHT));
		return;
	}

	Core::Rect rect;
	rect.m_vUpperLeft=Core::Vector2i::NullVector;
	int height=font->GetDimension("A").y + 5;
	int width=0;
	int i;

	for(i=0;i<(int)m_Items.size();i++)
	{
		if(m_Items[i].bIsSeparator)
		{
			m_Items[i].vDim.x=0;
			m_Items[i].vDim.y=height;
		}
		else
		{
			m_Items[i].vDim=font->GetDimension(m_Items[i].strText.c_str());
			m_Items[i].vDim.x+=20;
		}

		m_Items[i].iPosY=width;
		width+=m_Items[i].vDim.x;
	}

	if(m_pParent)
		width=m_pParent->GetAbsolutePosition().GetWidth();

	rect.m_vLowerRight.x=width;
	rect.m_vLowerRight.y=height;

	SetRelativePosition(rect);

	for(i=0;i<(int)m_Items.size();i++)
		if(m_Items[i].pSubMenu)
		{
			int w=m_Items[i].pSubMenu->GetAbsolutePosition().GetWidth();
			int h=m_Items[i].pSubMenu->GetAbsolutePosition().GetHeight();

			m_Items[i].pSubMenu->SetRelativePosition(Core::Rect(m_Items[i].iPosY,height,
				m_Items[i].iPosY+w-5, height+h));
		}

	IElement::Draw();
}

Core::Rect Menu::GetHRect(const ContextMenu::Item &i, const Core::Rect &absolute) const
{
	Core::Rect r(absolute);
	r.m_vUpperLeft.x+=i.iPosY;
	r.m_vLowerRight.x=r.m_vUpperLeft.x + i.vDim.x;
	return r;
}

Core::Rect Menu::GetRect(const ContextMenu::Item& i, const Core::Rect& absolute) const
{
	return GetHRect(i,absolute);
}

void Menu::UpdateAbsolutePosition()
{
	if(m_pParent)
		m_DesiredRect.m_vLowerRight.x=m_pParent->GetAbsolutePosition().GetWidth();

	IElement::UpdateAbsolutePosition();
}

}