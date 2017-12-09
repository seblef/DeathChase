
#include "Environment.h"
#include "Skin.h"
#include "Button.h"
#include "KeyButton.h"
#include "Window.h"
#include "ScrollBar.h"
#include "Font.h"
#include "CSpriteBank.h"
#include "Image.h"
#include "CheckBox.h"
#include "ListBox.h"
#include "FileDialog.h"
#include "ImageFileDialog.h"
#include "ColorDialog.h"
#include "StaticText.h"
#include "EditBox.h"
#include "SpinBox.h"
#include "InOutFader.h"
#include "MessageBox.h"
#include "ModalScreen.h"
#include "TabControl.h"
#include "ContextMenu.h"
#include "ComboBox.h"
#include "Menu.h"
#include "ToolBar.h"
#include "../Core/Renderer.h"
#include "../Core/TextureMgr.h"
#include <dinput.h>


namespace GUI
{

Environment::Environment() : IEnvironment(),
		m_pHovered(0),m_pFocus(0),m_vLastHoveredPos(0,0),m_pCurrentSkin(0),
		m_pUserReceiver(0)

{
	int i;
	for(i=0;i<ET_COUNT;i++)
		m_pAllocators[i]=new Core::TMemoryAllocator<IElement>(ElementTypeNames[i]);

	ISkin *skin=new Skin(ST_WINDOWSMETALLIC);
	SetSkin(skin);

	m_ToolTip.dwLastTime=0;
	m_ToolTip.dwLaunchTime=1000;
	m_ToolTip.pElement=AddStaticText("",Core::Rect(0,0,10,10),true,true,0,0,true);
	m_ToolTip.pElement->SetSubElement(true);
	m_ToolTip.pElement->SetVisible(false);
	m_ToolTip.pElement->SetTabStop(false);
	m_ToolTip.pElement->SetOverrideColor(GetSkin()->GetColor(DC_TOOLTIP));
	m_ToolTip.pElement->SetBackgroundColor(GetSkin()->GetColor(DC_TOOLTIPBACKGROUND));
	m_ToolTip.pElement->SetDrawBackground(true);
	m_ToolTip.pElement->SetOverrideFont(GetSkin()->GetFont(DF_TOOLTIP));

	m_pEnvironment=this;
	SetTabGroup(true);
}

Environment::~Environment()
{
	if(m_pHovered && m_pHovered!=this)
		m_pHovered=0;

	if(m_pFocus)
		m_pFocus=0;

	if(m_ToolTip.pElement)
	{
		RemoveChild(m_ToolTip.pElement,true);
		m_ToolTip.pElement=0;
	}

	if(m_pCurrentSkin)
	{
		delete m_pCurrentSkin;
		m_pCurrentSkin=0;
	}

	uint32 i;
	for(i=0;i<m_Fonts.size();i++)
		m_Fonts[i].pFont->Drop();

	for(i=0;i<ET_COUNT;i++)
		if(m_pAllocators[i])
		{
			delete m_pAllocators[i];
			m_pAllocators[i]=0;
		}
}

void Environment::DrawAll()
{
	Core::Vector2i dim(g_Renderer.GetScreenSize());
	if(m_AbsoluteRect.m_vLowerRight.x != dim.x ||
	   m_AbsoluteRect.m_vLowerRight.y != dim.y)
	{
		m_DesiredRect.m_vLowerRight.x=g_Renderer.GetScreenSize().x;
		m_DesiredRect.m_vLowerRight.y=g_Renderer.GetScreenSize().y;
		m_AbsoluteClippingRect=m_DesiredRect;
		m_AbsoluteRect=m_DesiredRect;
		UpdateAbsolutePosition();
	}

	if(m_ToolTip.pElement)
		BringToFront(m_ToolTip.pElement);

	Draw();
	OnPostRender(timeGetTime());
}

bool Environment::SetFocus(IElement *element)
{
	if(m_pFocus==element)
		return false;

	if(element==this)
		element=0;

	IElement *currentfocus=0;
	if(m_pFocus)
	{
		currentfocus=m_pFocus;
		Event e;
		e.EventType=ET_GUI;
		e.sGUI.pCaller=m_pFocus;
		e.sGUI.pElement=element;
		e.sGUI.EventType=GE_FOCUSLOST;
		if(m_pFocus->OnEvent(e))
			return false;

		currentfocus=0;
	}

	if(element)
	{
		currentfocus=m_pFocus;

		Event e;
		e.EventType=ET_GUI;
		e.sGUI.pCaller=element;
		e.sGUI.pElement=m_pFocus;
		e.sGUI.EventType=GE_FOCUSED;
		if(element->OnEvent(e))
			return false;
	}

	m_pFocus=element;
	return true;
}

IElement *Environment::GetFocus() const
{
	return m_pFocus;
}

bool Environment::RemoveFocus(IElement *element)
{
	if(m_pFocus && m_pFocus==element)
	{
		Event e;
		e.EventType=ET_GUI;
		e.sGUI.pCaller=m_pFocus;
		e.sGUI.pElement=0;
		e.sGUI.EventType=GE_FOCUSLOST;
		if(m_pFocus->OnEvent(e))
			return false;
	}

	if(m_pFocus)
		m_pFocus=0;

	return true;
}

bool Environment::HasFocus(IElement *element) const
{
	return (element==m_pFocus);
}

void Environment::Clear()
{
	if(m_pFocus)
		m_pFocus=0;

	if(m_pHovered && m_pHovered!=this)
		m_pHovered=0;

	const std::list<IElement *>& children=GetRootElement()->GetChildren();
	while(!children.empty())
		RemoveChild(children.back(),true);
}

bool Environment::OnEvent(const GUI::Event &_Event)
{
	if(m_pUserReceiver && (_Event.EventType!=ET_GUI || _Event.sGUI.pCaller!=this))
		return m_pUserReceiver->OnEvent(_Event);

	return false;
}

void Environment::OnPostRender(uint32 dwTime)
{
	if(dwTime - m_ToolTip.dwLastTime >= m_ToolTip.dwLaunchTime &&
	   m_pHovered && m_pHovered!=this &&
	   m_ToolTip.pElement->IsVisible()!=true &&
	   m_pHovered!=m_ToolTip.pElement &&
	   m_pHovered->GetToolTipText() &&
	   GetSkin() && GetSkin()->GetFont(DF_TOOLTIP))
	{
		Core::Rect pos;
		pos.m_vUpperLeft=m_vLastHoveredPos;
		Core::Vector2i dim(GetSkin()->GetFont(DF_TOOLTIP)->GetDimension(m_pHovered->GetToolTipText()));
		dim.x+=GetSkin()->GetSize(DS_TEXTDISTANCEX) * 2;
		dim.y+=GetSkin()->GetSize(DS_TEXTDISTANCEY) * 2;

		pos.m_vUpperLeft.y-=dim.y+1;
		pos.m_vLowerRight.y=pos.m_vUpperLeft.y + dim.y -1;
		pos.m_vLowerRight.x=pos.m_vUpperLeft.x + dim.x;

		pos.ConstrainTo(GetAbsolutePosition());

		m_ToolTip.pElement->SetRelativePosition(pos);
		m_ToolTip.pElement->SetText(m_pHovered->GetToolTipText());

		m_ToolTip.pElement->SetVisible(!m_ToolTip.pElement->IsTextEmpty());

		int textheight=m_ToolTip.pElement->GetTextHeight();
		pos=m_ToolTip.pElement->GetRelativePosition();
		pos.m_vLowerRight.y=pos.m_vUpperLeft.y+textheight;
		m_ToolTip.pElement->SetRelativePosition(pos);
	}

	IElement::OnPostRender(dwTime);
}

void Environment::UpdateHoveredElement(Core::Vector2i pos)
{
	IElement *lasthovered=m_pHovered;
	m_vLastHoveredPos=pos;

	m_pHovered=GetElementFromPoint(pos);

	if(m_pHovered)
	{
		uint32 now=timeGetTime();

		if(m_pHovered!=lasthovered)
		{
			Event e;
			e.EventType=ET_GUI;

			if(lasthovered)
			{
				e.sGUI.pCaller=lasthovered;
				e.sGUI.EventType=GE_LEFT;
				e.sGUI.pElement=0;
				lasthovered->OnEvent(e);
			}

			if(m_ToolTip.pElement)
			{
				m_ToolTip.pElement->SetVisible(false);
				m_ToolTip.dwLastTime+=500;
			}
			else
			{
				if(now - m_ToolTip.dwLastTime < m_ToolTip.dwLastTime)
					m_ToolTip.dwLastTime+=500;
				else
					m_ToolTip.dwLastTime=now;
			}

			e.sGUI.pCaller=m_pHovered;
			e.sGUI.EventType=GE_HOVERED;
			e.sGUI.pElement=0;
			m_pHovered->OnEvent(e);
		}
	}
}

void Environment::SetUserEventReceiver(GUI::IEventReceiver *pEvr)
{
	m_pUserReceiver=pEvr;
}

bool Environment::PostEventFromUser(const Event& e)
{
	switch(e.EventType)
	{
		case ET_GUI:
			break;

		case ET_MOUSE:
			UpdateHoveredElement(Core::Vector2i((int)e.sMouse.dwX,(int)e.sMouse.dwY));

			if(e.sMouse.EventType==ME_LMOUSEDOWN)
				if((m_pHovered && m_pHovered!=m_pFocus) || !m_pFocus)
					SetFocus(m_pHovered);

			if(m_pFocus && m_pFocus->OnEvent(e))
				return true;

			if(!m_pFocus && m_pHovered)
				return m_pHovered->OnEvent(e);

			break;

		case ET_KEY:
			{
				if(m_pFocus)
					m_pFocus->OnEvent(e);
			}
			break;

		default:
			break;
	}

	return false;
}

ISkin *Environment::GetSkin() const
{
	return m_pCurrentSkin;
}

void Environment::SetSkin(GUI::ISkin *pSkin)
{
	if(m_pCurrentSkin)
		delete m_pCurrentSkin;

	m_pCurrentSkin=pSkin;
}

ISkin *Environment::LoadSkin(const char *szName)
{
	ISkin *skin=new Skin(ST_WINDOWSMETALLIC);

/*	if(!skin->Load(szName))
	{
		delete skin;
		return 0;
	}
*/
	Font *font=(Font *)GetBuiltInFont();

	if(font)
	{
		ISpriteBank *bank=font->GetSpriteBank();
		skin->SetFont(font);
		skin->SetSpriteBank(bank);
	}

	return skin;
}

IButton *Environment::AddButton(const Core::Rect &rect, IElement *pParent, IEventReceiver *pUser,
								uint32 dwID, const char *szText, const char *szToolTipText)
{
	IButton *button=(IButton *)m_pAllocators[ET_BUTTON]->GetFree();

	if(button)
		((Button *)button)->Reset(this,pParent ? pParent : this, pUser, dwID,rect);
	else
	{
		button=new Button(this,pParent ? pParent : this, pUser, dwID, rect);
		m_pAllocators[ET_BUTTON]->RegisterNew((IElement *)button);
	}

	if(szText) button->SetText(szText);
	if(szToolTipText) button->SetToolTipText(szToolTipText);

	return button;
}

IKeyButton *Environment::AddKeyButton(const Core::Rect &rect, IElement *pParent, IEventReceiver *pUser,
								uint32 dwID, const char *szText, const char *szToolTipText)
{
	IKeyButton *button=(IKeyButton *)m_pAllocators[ET_KEYBUTTON]->GetFree();

	if(button)
		((Button *)button)->Reset(this,pParent ? pParent : this, pUser, dwID,rect);
	else
	{
		button=new KeyButton(this,pParent ? pParent : this, pUser, dwID, rect);
		m_pAllocators[ET_KEYBUTTON]->RegisterNew((IElement *)button);
	}

	if(szText) button->SetText(szText);
	if(szToolTipText) button->SetToolTipText(szToolTipText);

	return button;
}

IWindow *Environment::AddWindow(const Core::Rect &rect, bool bModal,
							   const char *szText, IElement *pParent,
							    IEventReceiver *pUser, uint32 dwID)
{
	pParent=pParent ? pParent : this;

	if(bModal)
	{
		IElement *pFree=m_pAllocators[ET_MODALSCREEN]->GetFree();
		if(pFree)
		{
			((ModalScreen *)pFree)->Reset(this,pParent,pUser,INVALID_ID);
			pParent=pFree;
		}
		else
		{
			pParent=new ModalScreen(this,pParent,pUser,dwID);
			m_pAllocators[ET_MODALSCREEN]->RegisterNew(pParent);
		}
	}

	IWindow *win=(IWindow *)m_pAllocators[ET_WINDOW]->GetFree();
	if(win)
		((Window *)win)->Reset(this,pParent,pUser,dwID,rect);
	else
	{
		win=new Window(this,pParent,pUser,dwID,rect);
		m_pAllocators[ET_WINDOW]->RegisterNew((IElement *)win);
	}

	if(szText)
		win->SetText(szText);

	return win;
}

IElement *Environment::AddModalScreen(IElement *pParent, IEventReceiver *pUser)
{
	pParent=pParent ? pParent : this;

	IElement *win=m_pAllocators[ET_MODALSCREEN]->GetFree();
	if(win)
		((ModalScreen *)win)->Reset(this,pParent,pUser,INVALID_ID);
	else
	{
		win=new ModalScreen(this,pParent,pUser,INVALID_ID);
		m_pAllocators[ET_MODALSCREEN]->RegisterNew(win);
	}

	return win;
}

IWindow *Environment::AddMessageBox(const char *szCaption, const char *szText, bool bModal,
									uint32 dwFlags, IElement *pParent,  IEventReceiver *pUser,
									uint32 dwID)
{
	if(!m_pCurrentSkin)
		return 0;

	pParent=pParent ? pParent : this;

	Core::Rect rect;
	Core::Vector2i screendim, msgboxdim;

	screendim.x=pParent->GetAbsolutePosition().GetWidth();
	screendim.y=pParent->GetAbsolutePosition().GetHeight();
	msgboxdim.x=m_pCurrentSkin->GetSize(DS_MESSAGEBOXWIDTH);
	msgboxdim.y=m_pCurrentSkin->GetSize(DS_MESSAGEBOXHEIGHT);

	rect.m_vUpperLeft.x=(screendim.x - msgboxdim.x) / 2;
	rect.m_vUpperLeft.y=(screendim.y - msgboxdim.y) / 2;
	rect.m_vLowerRight.x=rect.m_vUpperLeft.x + msgboxdim.x;
	rect.m_vLowerRight.y=rect.m_vUpperLeft.y + msgboxdim.y;

	if(bModal)
	{
		IElement *pFree=m_pAllocators[ET_MODALSCREEN]->GetFree();
		if(pFree)
		{
			((ModalScreen *)pFree)->Reset(this,pParent,pUser,INVALID_ID);
			pParent=pFree;
		}
		else
		{
			pParent=new ModalScreen(this,pParent,pUser,INVALID_ID);
			m_pAllocators[ET_MODALSCREEN]->RegisterNew(pParent);
		}
	}

	IWindow *win=(IWindow *)m_pAllocators[ET_MESSAGEBOX]->GetFree();
	if(win)
		((CMessageBox *)win)->Reset(this,szCaption,szText,dwFlags,pParent,pUser,dwID,rect);
	else
	{
		win=new CMessageBox(this,szCaption,szText,dwFlags,pParent,pUser,dwID,rect);
		m_pAllocators[ET_MESSAGEBOX]->RegisterNew((IElement *)win);
	}

	return win;
}

IScrollBar *Environment::AddScrollBar(bool bHorizontal, const Core::Rect &rect,
									  IElement *pParent,  IEventReceiver *pUser, uint32 dwID)
{
	IScrollBar *bar=(IScrollBar *)m_pAllocators[ET_SCROLLBAR]->GetFree();
	if(bar)
		((ScrollBar *)bar)->Reset(bHorizontal,this,pParent ? pParent : this, pUser, dwID,rect);
	else
	{
		bar=new ScrollBar(bHorizontal,this,pParent ? pParent : this, pUser, dwID,rect);
		m_pAllocators[ET_SCROLLBAR]->RegisterNew((IElement *)bar);
	}

	return bar;
}

IImage *Environment::AddImage(Core::HTexture hTex, Core::Vector2i pos,
							  bool bUseAlpha, IElement *pParent,
							   IEventReceiver *pUser, uint32 dwID, const char *szText)
{
	Core::Vector2i sz(0,0);
	if(!hTex.IsNull())
		g_TextureMgr.GetTextureSize(hTex,sz.x,sz.y);

	IImage *img=(IImage *)m_pAllocators[ET_IMAGE]->GetFree();
	if(img)
		((Image *)img)->Reset(this, pParent ? pParent : this, pUser, dwID, Core::Rect(pos,pos + sz));
	else
	{
		img=new Image(this, pParent ? pParent : this, pUser, dwID, Core::Rect(pos,pos + sz));
		m_pAllocators[ET_IMAGE]->RegisterNew((IElement *)img);
	}

	if(szText) img->SetText(szText);
	if(bUseAlpha) img->SetUseAlphaChannel(true);
	if(!hTex.IsNull()) img->SetImage(hTex);

	return img;
}

IImage *Environment::AddImage(const Core::Rect &rect, IElement *pParent,
							   IEventReceiver *pUser, uint32 dwID, const char *szText)
{
	IImage *img=(IImage *)m_pAllocators[ET_IMAGE]->GetFree();
	if(img)
		((Image *)img)->Reset(this, pParent ? pParent : this, pUser, dwID, rect);
	else
	{
		img=new Image(this, pParent ? pParent : this, pUser, dwID, rect);
		m_pAllocators[ET_IMAGE]->RegisterNew((IElement *)img);
	}

	if(szText) img->SetText(szText);

	return img;
}

ICheckBox *Environment::AddCheckBox(bool bChecked, const Core::Rect &rect,
									IElement *pParent,  IEventReceiver *pUser, 
									uint32 dwID, const char *szText)
{
	ICheckBox *b=(ICheckBox *)m_pAllocators[ET_CHECKBOX]->GetFree();
	if(b)
		((CheckBox *)b)->Reset(bChecked,this,pParent ? pParent : this, pUser, dwID, rect);
	else
	{
		b=new CheckBox(bChecked,this,pParent ? pParent : this, pUser, dwID, rect);
		m_pAllocators[ET_CHECKBOX]->RegisterNew((IElement *)b);
	}

	if(szText) b->SetText(szText);
	return b;
}

IListBox *Environment::AddListBox(const Core::Rect &rect, IElement *pParent,  IEventReceiver *pUser,
								  uint32 dwID, bool bDrawBackground)
{
	IListBox *b=(IListBox *)m_pAllocators[ET_LISTBOX]->GetFree();
	if(b)
		((ListBox *)b)->Reset(this,pParent ? pParent : this,pUser,dwID,rect,true,bDrawBackground,false);
	else
	{
		b=new ListBox(this,pParent ? pParent : this,pUser,dwID,rect,true,bDrawBackground,false);
		m_pAllocators[ET_LISTBOX]->RegisterNew((IElement *)b);
	}

	if(m_pCurrentSkin && m_pCurrentSkin->GetSpriteBank())
		b->SetSpriteBank(m_pCurrentSkin->GetSpriteBank());

	return b;
}

IFileDialog *Environment::AddFileDialog(const char *szTitle, bool bModal, bool bImageDialog,
										IElement *pParent,  IEventReceiver *pUser, uint32 dwID)
{
	pParent=pParent ? pParent : this;

	if(bModal)
	{
		IElement *pFree=m_pAllocators[ET_MODALSCREEN]->GetFree();
		if(pFree)
		{
			((ModalScreen *)pFree)->Reset(this,pParent,pUser,INVALID_ID);
			pParent=pFree;
		}
		else
		{
			pParent=new ModalScreen(this,pParent,pUser,INVALID_ID);
			m_pAllocators[ET_MODALSCREEN]->RegisterNew(pParent);
		}
	}

	IFileDialog *d;

	if(bImageDialog)
	{
		d=(IFileDialog *)m_pAllocators[ET_IMAGEFILEDIALOG]->GetFree();
		if(d)
			((ImageFileDialog *)d)->Reset(szTitle,this,pParent,pUser,dwID);
		else
		{
			d=new ImageFileDialog(szTitle,this,pParent,pUser,dwID);
			m_pAllocators[ET_IMAGEFILEDIALOG]->RegisterNew((IElement *)d);
		}
	}
	else
	{
		d=(IFileDialog *)m_pAllocators[ET_FILEDIALOG]->GetFree();
		if(d)
			((FileDialog *)d)->Reset(szTitle,this,pParent,pUser,dwID);
		else
		{
			d=new FileDialog(szTitle,this,pParent,pUser,dwID);
			m_pAllocators[ET_FILEDIALOG]->RegisterNew((IElement *)d);
		}
	}

	return d;
}

IColorDialog *Environment::AddColorDialog(const char *szTitle, bool bModal,
										  IElement *pParent,  IEventReceiver *pUser, 
										  uint32 dwID)
{
	pParent=pParent ? pParent : this;

	if(bModal)
	{
		IElement *pFree=m_pAllocators[ET_MODALSCREEN]->GetFree();
		if(pFree)
		{
			((ModalScreen *)pFree)->Reset(this,pParent,pUser,INVALID_ID);
			pParent=pFree;
		}
		else
		{
			pParent=new ModalScreen(this,pParent,pUser,INVALID_ID);
			m_pAllocators[ET_MODALSCREEN]->RegisterNew(pParent);
		}
	}

	IColorDialog *d=(IColorDialog *)m_pAllocators[ET_COLORDIALOG]->GetFree();
	if(d)
		((ColorDialog *)d)->Reset(szTitle,this,pParent,pUser,dwID);
	else
	{
		d=new ColorDialog(szTitle,this,pParent,pUser,dwID);
		m_pAllocators[ET_COLORDIALOG]->RegisterNew((IElement *)d);
	}

	return d;
}

IStaticText *Environment::AddStaticText(const char *szText, const Core::Rect &rect,
										bool bBorder, bool bWordWrap,
										IElement *pParent,
										 IEventReceiver *pUser, uint32 dwID, bool bDrawBackground)
{
	IStaticText *d=(IStaticText *)m_pAllocators[ET_STATICTEXT]->GetFree();
	if(d)
		((StaticText *)d)->Reset(szText,bBorder,this,pParent ? pParent : this, pUser,
			dwID, rect, bDrawBackground);
	else
	{
		d=new StaticText(szText,bBorder,this,pParent ? pParent : this, pUser,
			dwID, rect, bDrawBackground);
		m_pAllocators[ET_STATICTEXT]->RegisterNew((IElement *)d);
	}

	d->SetWordWrap(bWordWrap);

	return d;
}

IEditBox *Environment::AddEditBox(const char *szText, const Core::Rect &rect,
								  bool bBorder, IElement *pParent,  IEventReceiver *pUser,
								  uint32 dwID)
{
	IEditBox *d=(IEditBox *)m_pAllocators[ET_EDITBOX]->GetFree();
	if(d)
		((EditBox *)d)->Reset(szText,bBorder,this,pParent ? pParent : this, pUser, dwID, rect);
	else
	{
		d=new EditBox(szText,bBorder,this,pParent ? pParent : this, pUser,dwID, rect);
		m_pAllocators[ET_EDITBOX]->RegisterNew((IElement *)d);
	}

	return d;
}

ISpinBox *Environment::AddSpinBox(const char *szText, const Core::Rect &rect,
								  IElement *pParent,  IEventReceiver *pUser, uint32 dwID)
{
	ISpinBox *d=(ISpinBox *)m_pAllocators[ET_SPINBOX]->GetFree();
	if(d)
		((SpinBox *)d)->Reset(szText,this,pParent ? pParent : this, pUser, dwID, rect);
	else
	{
		d=new SpinBox(szText,this,pParent ? pParent : this, pUser, dwID, rect);
		m_pAllocators[ET_SPINBOX]->RegisterNew((IElement *)d);
	}

	return d;
}

ITabControl *Environment::AddTabControl(const Core::Rect &rect, IElement *pParent,  IEventReceiver *pUser,
										bool bFillBackground, bool bBorder, uint32 dwID)
{
	ITabControl *t=(ITabControl *)m_pAllocators[ET_TABCONTROL]->GetFree();
	if(t)
		((TabControl *)t)->Reset(this,pParent ? pParent : this, pUser, rect,bFillBackground,bBorder,dwID);
	else
	{
		t=new TabControl(this,pParent ? pParent : this, pUser, rect,bFillBackground,bBorder,dwID);
		m_pAllocators[ET_TABCONTROL]->RegisterNew((IElement *)t);
	}

	return t;
}

ITab *Environment::AddTab(const Core::Rect &rect, IElement *pParent,  IEventReceiver *pUser,
						  uint32 dwID)
{
	ITab *t=(ITab *)m_pAllocators[ET_TAB]->GetFree();
	if(t)
		((Tab *)t)->Reset(-1,this,pParent ? pParent : this, pUser, rect, dwID);
	else
	{
		t=new Tab(-1,this,pParent ? pParent : this, pUser, rect, dwID);
		m_pAllocators[ET_TAB]->RegisterNew((IElement *)t);
	}

	return t;
}

IContextMenu *Environment::AddContextMenu(const Core::Rect &rect, IElement *pParent,
										   IEventReceiver *pUser, uint32 dwID)
{
	IContextMenu *c=(IContextMenu *)m_pAllocators[ET_CONTEXTMENU]->GetFree();
	if(c)
		((ContextMenu *)c)->Reset(this,pParent ? pParent : this, pUser, dwID, rect, true);
	else
	{
		c=new ContextMenu(this,pParent ? pParent : this, pUser, dwID, rect, true);
		m_pAllocators[ET_CONTEXTMENU]->RegisterNew((IElement *)c);
	}

	return c;
}

IContextMenu *Environment::AddMenu(IElement *pParent,  IEventReceiver *pUser, uint32 dwID)
{
	if(!pParent) pParent=this;

	IContextMenu *c=(IContextMenu *)m_pAllocators[ET_MENU]->GetFree();
	if(c)
		((Menu *)c)->Reset(this,pParent,pUser,dwID,Core::Rect(0,0,pParent->GetAbsolutePosition().GetWidth(),pParent->GetAbsolutePosition().GetHeight()));
	else
	{
		c=new Menu(this,pParent,pUser,dwID,Core::Rect(0,0,pParent->GetAbsolutePosition().GetWidth(),pParent->GetAbsolutePosition().GetHeight()));
		m_pAllocators[ET_MENU]->RegisterNew((IElement *)c);
	}

	c->SetMinSize(Core::Vector2i(1,1));

	return c;
}

IToolBar *Environment::AddToolBar(IElement *pParent,  IEventReceiver *pUser, uint32 dwID)
{
	if(!pParent) pParent=this;

	IToolBar *b=(IToolBar *)m_pAllocators[ET_TOOLBAR]->GetFree();
	if(b)
		((ToolBar *)b)->Reset(this,pParent,pUser,dwID,Core::Rect(0,0,10,10));
	else
	{
		b=new ToolBar(this,pParent,pUser,dwID,Core::Rect(0,0,10,10));
		m_pAllocators[ET_TOOLBAR]->RegisterNew((IElement *)b);
	}

	return b;
}

IInOutFader *Environment::AddInOutFader(const Core::Rect *rect, IElement *pParent,
										 IEventReceiver *pUser,uint32 dwID)
{
	Core::Rect r;

	if(rect) r=*rect;
	else
		r=Core::Rect(g_Renderer.GetViewportPos(),g_Renderer.GetViewportPos()+g_Renderer.GetViewportSize());

	if(!m_pParent) m_pParent=this;

	IInOutFader *fader=(IInOutFader *)m_pAllocators[ET_INOUTFADER]->GetFree();
	if(fader)
		((InOutFader *)fader)->Reset(this,pParent,pUser,dwID,r);
	else
	{
		fader=new InOutFader(this,pParent,pUser,dwID,r);
		m_pAllocators[ET_INOUTFADER]->RegisterNew((IElement *)fader);
	}

	return fader;
}

IComboBox *Environment::AddComboBox(const Core::Rect &rect, IElement *pParent,
									 IEventReceiver *pUser, uint32 dwID)
{
	IComboBox *t=(IComboBox *)m_pAllocators[ET_COMBOBOX]->GetFree();
	if(t)
		((ComboBox *)t)->Reset(this, pParent ? pParent : this, pUser, dwID, rect);
	else
	{
		t=new ComboBox(this, pParent ? pParent : this, pUser, dwID, rect);
		m_pAllocators[ET_COMBOBOX]->RegisterNew((IElement *)t);
	}

	return t;
}

IFont *Environment::GetFont(const char *szFileName)
{
	SFont f;
	IFont *ifont=0;

	if(!szFileName)
		f.strFileName="";
	else
		f.strFileName=szFileName;

	std::vector<SFont>::iterator it;
	for(it=m_Fonts.begin();it!=m_Fonts.end();it++)
		if(it->strFileName==f.strFileName)
			return it->pFont;

	Font *font=new Font(this,f.strFileName.c_str());
	ifont=(IFont *)font;
	if(!font->Load(f.strFileName.c_str()))
	{
		g_Output->AddMsg("x Failed loading font %s\n",f.strFileName.c_str());
		font->Drop();
		return 0;
	}

	f.pFont=ifont;
	m_Fonts.push_back(f);

	return ifont;
}

ISpriteBank *Environment::GetSpriteBank(const char *szFileName)
{
	SSpriteBank b;
	
	if(!szFileName)
		b.strFileName="";
	else
		b.strFileName=szFileName;

	std::vector<SSpriteBank>::iterator it;
	for(it=m_Banks.begin();it!=m_Banks.end();it++)
		if(it->strFileName==b.strFileName)
			return it->pBank;

	return 0;
}

ISpriteBank *Environment::AddEmptySpriteBank(const char *szName)
{
	SSpriteBank b;

	if(!szName)
		b.strFileName="";
	else
		b.strFileName=szName;

	std::vector<SSpriteBank>::iterator it;
	for(it=m_Banks.begin();it!=m_Banks.end();it++)
		if(it->strFileName==b.strFileName)
			return it->pBank;

	b.pBank=new SpriteBank(this);
	m_Banks.push_back(b);

	return b.pBank;
}

IFont *Environment::GetBuiltInFont()
{
	if(m_Fonts.empty())
	{
		Font *pFont=new Font(this,"standard.bmp");
		
		if(pFont->Load("standard.bmp"))
		{
			SFont sf;
			sf.strFileName="standard.bmp";
			sf.pFont=pFont;
			m_Fonts.push_back(sf);
			return pFont;
		}
		else
		{
			g_Output->AddMsg("x Failed loading font standard.bmp\n");
			return 0;
		}
	}

	return m_Fonts[0].pFont;
}

IElement *Environment::GetRootElement()
{
	return this;
}

IElement *Environment::GetNextElement(bool bReverse, bool bGroup)
{
	IElement *startpos=m_pFocus ? m_pFocus->GetTabGroup() : 0;
	int startorder=-1;

	if(bGroup && startpos)
		startorder=startpos->GetTabOrder();
	else if(!bGroup && m_pFocus && !m_pFocus->IsTabGroup())
	{
		startorder=m_pFocus->GetTabOrder();
		if(startorder==-1)
		{
			IElement *el=m_pFocus;
			while(el && el->GetParent() && startorder==-1)
			{
				el=el->GetParent();
				startorder=el->GetTabOrder();
			}
		}
	}

	if(bGroup || !startpos)
		startpos=this;

	IElement *closest=0;
	IElement *first;

	startpos->GetNextElement(startorder,bReverse,bGroup,first,closest);

	if(closest) return closest;
	else if(first) return first;
	else if(bGroup) return this;
	else return 0;
}

bool Environment::ReleaseElement(IElement *pElement)
{
	if(m_pFocus==pElement) m_pFocus=0;
	if(m_pHovered==pElement) m_pHovered=0;

	if(!pElement)
		return false;

	if(pElement->GetParent()==this)
	{
		std::list<IElement *>::iterator i;
		for(i=m_Children.begin();i!=m_Children.end();i++)
			if((*i)==pElement)
			{
				m_Children.erase(i);
				break;
			}
	}

	if(pElement->GetType() >= 0 && pElement->GetType() < ET_COUNT)
	{
		m_pAllocators[(int)pElement->GetType()]->Delete(pElement);
		return true;
	}
	else
	{
	//	delete pElement;
		return false;
	}
}

}