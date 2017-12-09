
#include "ComboBox.h"
#include "IEnvironment.h"
#include "ISkin.h"
#include "IFont.h"
#include "IButton.h"
#include "ListBox.h"
#include <dinput.h>

namespace GUI
{

ComboBox::ComboBox(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
				   uint32 dwID, Core::Rect rect) :
	IComboBox(pEnvironment,pParent,pUser,dwID,rect),
		m_pListButton(0),m_pListBox(0),m_iSelected(-1),m_bHasFocus(false),m_pLastFocus(0)
{
	ISkin *skin=m_pEnvironment->GetSkin();

	int width=15;
	if(skin)
		width=skin->GetSize(DS_WINDOWBUTTONWIDTH);

	Core::Rect r;
	r.m_vUpperLeft.x=rect.GetWidth() - width - 2;
	r.m_vLowerRight.x=rect.GetWidth() - 2;

	r.m_vUpperLeft.y=2;
	r.m_vLowerRight.y=rect.GetHeight() - 2;

	m_pListButton=pEnvironment->AddButton(r,this,0,-1,"");
	if(skin && skin->GetSpriteBank())
	{
		m_pListButton->SetSpriteBank(skin->GetSpriteBank());
		m_pListButton->SetSprite(BS_UP,skin->GetIcon(DI_CURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
		m_pListButton->SetSprite(BS_DOWN,skin->GetIcon(DI_CURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
	}
	m_pListButton->SetSubElement(true);
	m_pListButton->SetTabStop(false);

	SetNoClip(true);

	SetTabStop(true);
	SetTabOrder(-1);
}

void ComboBox::Reset(IEnvironment *pEnvironment, IElement *pParent,IEventReceiver *pUser,
				   uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	ISkin *skin=m_pEnvironment->GetSkin();

	int width=15;
	if(skin)
		width=skin->GetSize(DS_WINDOWBUTTONWIDTH);

	Core::Rect r;
	r.m_vUpperLeft.x=rect.GetWidth() - width - 2;
	r.m_vLowerRight.x=rect.GetWidth() - 2;

	r.m_vUpperLeft.y=2;
	r.m_vLowerRight.y=rect.GetHeight() - 2;

	m_pListButton=pEnvironment->AddButton(r,this,0,-1,"");
	if(skin && skin->GetSpriteBank())
	{
		m_pListButton->SetSpriteBank(skin->GetSpriteBank());
		m_pListButton->SetSprite(BS_UP,skin->GetIcon(DI_CURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
		m_pListButton->SetSprite(BS_DOWN,skin->GetIcon(DI_CURSORDOWN),skin->GetColor(DC_WINDOWSYMBOL));
	}
	m_pListButton->SetSubElement(true);
	m_pListButton->SetTabStop(false);

	SetNoClip(true);

	SetTabStop(true);
	SetTabOrder(-1);
}

void ComboBox::Clear()
{
	m_Items.clear();

	m_pListButton=0;
	m_pListBox=0;
	m_iSelected=-1;
	m_bHasFocus=false;
	m_pLastFocus=0;

	IElement::Clear();
}

uint32 ComboBox::GetItemCount() const
{
	return m_Items.size();
}

const char *ComboBox::GetItem(uint32 index) const
{
	if(index >= m_Items.size())
		return 0;

	return m_Items[index].c_str();
}

void ComboBox::RemoveItem(uint32 index)
{
	if(index >= m_Items.size())
		return;

	if(m_iSelected==(int)index)
		m_iSelected=-1;

	m_Items.erase(m_Items.begin() + index);
}

const char *ComboBox::GetText() const
{
	if(m_iSelected > -1)
		return GetItem(m_iSelected);
	else
		return 0;
}

uint32 ComboBox::AddItem(const char *szText)
{
	m_Items.push_back(std::string(szText));

	if(m_iSelected==-1)
		m_iSelected=0;

	return m_Items.size() -1;
}

void ComboBox::ClearList()
{
	m_Items.clear();
	m_iSelected=-1;
}

int ComboBox::GetSelected() const
{
	return m_iSelected;
}

void ComboBox::SetSelected(int index)
{
	if(index < -1 || index >= (int)m_Items.size())
		return;

	m_iSelected=index;
}

void ComboBox::UpdateAbsolutePosition()
{
	IElement::UpdateAbsolutePosition();

	const int width=m_pEnvironment->GetSkin()->GetSize(DS_WINDOWBUTTONWIDTH);

	if(m_pListBox)
		m_pListBox->SetRelativePosition(Core::Rect(m_RelativeRect.GetWidth() - width -2, 2,
			m_RelativeRect.GetWidth() -2, m_RelativeRect.GetHeight() -2));
}

bool ComboBox::OnEvent(const Event& _Event)
{
	switch(_Event.EventType)
	{
		case ET_KEY:
			if(m_pListBox && _Event.sKey.bPressed && _Event.sKey.cDXChar==DIK_ESCAPE)
			{
				OpenCloseMenu();
				return true;
			}
			else if(_Event.sKey.cDXChar==DIK_RETURN || _Event.sKey.cDXChar==DIK_SPACE)
			{
				if(!_Event.sKey.bPressed)
					OpenCloseMenu();

				m_pListButton->SetPressed(m_pListBox==0);
				return true;
			}
			else if(_Event.sKey.bPressed)
			{
				int oldselected=m_iSelected;
				bool absorb=true;

				switch(_Event.sKey.cDXChar)
				{
					case DIK_DOWN:
						m_iSelected+=1;
						break;
					case DIK_UP:
						m_iSelected-=1;
						break;
					case DIK_HOME:
					case DIK_PRIOR:
						m_iSelected=0;
						break;
					case DIK_END:
					case DIK_NEXT:
						m_iSelected=(int)m_Items.size()-1;
						break;
					default:
						absorb=false;
						break;
				}

				if(m_iSelected < 0) m_iSelected=0;
				if(m_iSelected >= (int)m_Items.size()) m_iSelected=(int)m_Items.size()-1;

				if(m_iSelected!=oldselected)
					SendSelectionChangedEvent();

				if(absorb)
					return true;
			}
			break;

		case ET_GUI:
			switch(_Event.sGUI.EventType)
			{
				case GE_FOCUSLOST:
					if(m_pListBox &&
						(m_pEnvironment->HasFocus(m_pListBox) || m_pListBox->IsMyChild(_Event.sGUI.pCaller)) &&
						_Event.sGUI.pElement!=this &&
						_Event.sGUI.pElement!=m_pListButton &&
						_Event.sGUI.pElement!=m_pListBox &&
						!m_pListBox->IsMyChild(_Event.sGUI.pElement))
					{
						OpenCloseMenu();
					}
					break;
				case GE_BUTTONCLICKED:
					if(_Event.sGUI.pCaller==m_pListButton)
					{
						OpenCloseMenu();
						return true;
					}
					break;
				case GE_LISTBOXSELECTEDAGAIN:
				case GE_LISTBOXCHANGED:
					if(_Event.sGUI.pCaller==m_pListBox)
					{
						m_iSelected=m_pListBox->GetSelected();
						if(m_iSelected < 0 || m_iSelected >= (int)m_Items.size())
							m_iSelected=-1;
						
						OpenCloseMenu();
						SendSelectionChangedEvent();
					}
					return true;
				default:
					break;
			}
			break;

		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_LMOUSEDOWN:
				{
					Core::Vector2i p((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY);

					if(m_pListBox && m_pListBox->IsPointInside(p) && m_pListBox->OnEvent(_Event))
						return true;

					return true;
				}
				case ME_LMOUSEUP:
				{
					Core::Vector2i p((int)_Event.sMouse.dwX,(int)_Event.sMouse.dwY);

					if(m_pListBox && m_pListBox->GetAbsolutePosition().IsPointInside(p) &&
						m_pListBox->OnEvent(_Event))
						return true;
					else
					{
						OpenCloseMenu();
						return true;
					}
				}
				break;

				case ME_MOUSEWHEEL:
					{
						int oldselected=m_iSelected;
						m_iSelected+=(_Event.sMouse.fWheel < 0.0f) ? 1 : -1;

						if(m_iSelected < 0) m_iSelected=0;
						if(m_iSelected >= (int)m_Items.size()) m_iSelected=(int)m_Items.size()-1;

						if(m_iSelected!=oldselected)
							SendSelectionChangedEvent();
					}
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void ComboBox::SendSelectionChangedEvent()
{
	Event e;
	e.EventType=ET_GUI;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;
	e.sGUI.EventType=GE_COMBOBOXCHANGED;
	SendEvent(e);
}

void ComboBox::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	IElement *currentfocus=m_pEnvironment->GetFocus();

	if(currentfocus!=m_pLastFocus)
	{
		m_bHasFocus=currentfocus==this || IsMyChild(currentfocus);
		m_pLastFocus=currentfocus;
	}

	Core::Rect framerect(m_AbsoluteRect);
	
	skin->Draw3DSunkenPane(this,skin->GetColor(DC_3DHIGHLIGHT),
		true,true,framerect,&m_AbsoluteClippingRect);

	if(m_iSelected!=-1)
	{
		framerect=m_AbsoluteRect;
		framerect.m_vUpperLeft.x+=2;
		framerect.m_vUpperLeft.y+=2;
		framerect.m_vLowerRight.x-=m_pListButton->GetAbsolutePosition().GetWidth() + 2;
		framerect.m_vLowerRight.y-=2;
		if(m_bHasFocus)
			skin->Draw2DRectangle(this,skin->GetColor(DC_HIGHLIGHT),framerect,&m_AbsoluteClippingRect);

		IFont *font=skin->GetFont();
		if(font)
			font->Draw(m_Items[m_iSelected].c_str(),framerect,
				skin->GetColor(m_bHasFocus ? DC_HIGHLIGHTTEXT : DC_BUTTONTEXT),
				false,true,&m_AbsoluteClippingRect);
	}

	IElement::Draw();
}

void ComboBox::OpenCloseMenu()
{
	if(m_pListBox)
	{
		m_pEnvironment->SetFocus(this);
		RemoveChild(m_pListBox,true);
		m_pListBox=0;
	}
	else
	{
		if(m_pParent) m_pParent->BringToFront(this);

		ISkin *skin=m_pEnvironment->GetSkin();
		int h=m_Items.size();

		if(h > 5) h=5;
		if(h==0) h=1;

		IFont *font=skin->GetFont();
		if(font)
			h*=(font->GetDimension("A").y + 4);

		Core::Rect r(0,m_AbsoluteRect.GetHeight(),
			m_AbsoluteRect.GetWidth(),m_AbsoluteRect.GetHeight() +h);

		m_pListBox=m_pEnvironment->AddListBox(r,this,0,INVALID_ID,true);
		m_pListBox->SetNoClip(true);
		m_pListBox->SetMoveOverSelect(true);
		m_pListBox->SetSubElement(true);
		m_pListBox->UpdateAbsolutePosition();

		int i;
		for(i=0;i<(int)m_Items.size();i++)
			m_pListBox->AddItem(m_Items[i].c_str());

		m_pListBox->SetSelected(m_iSelected);
		m_pEnvironment->SetFocus(m_pListBox);
	}
}

}
