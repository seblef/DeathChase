
#include "MessageBox.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IButton.h"
#include "IFont.h"
#include <dinput.h>

namespace GUI
{

CMessageBox::CMessageBox(IEnvironment *pEnvironment, const char *szCaption,
					   const char *szText, int iFlags, IElement *pParent,IEventReceiver *pUser,
					   uint32 dwID, Core::Rect rect) :
	Window(pEnvironment,pParent,pUser,dwID,rect),
		m_pOK(0),m_pCancel(0),m_pYes(0),m_pNo(0),m_pText(0),
		m_iFlags(iFlags),m_strMsgText(szText),m_bPressed(false)
{
	m_Type=ET_MESSAGEBOX;
	m_pEnvironment->SetFocus(0);

	GetMaximizeButton()->Remove();
	GetMinimizeButton()->Remove();

	if(szCaption)
		SetText(szCaption);

	m_pEnvironment->SetFocus(this);
	RefreshControls();
}

CMessageBox::~CMessageBox()
{
	RemoveChild(m_pText,true);
	RemoveChild(m_pOK,true);
	RemoveChild(m_pCancel,true);
	RemoveChild(m_pYes,true);
	RemoveChild(m_pNo,true);
}

void CMessageBox::Reset(IEnvironment *pEnvironment, const char *szCaption, const char *szText,
						int iFlags, IElement *pParent, IEventReceiver *pUser,
						uint32 dwID, Core::Rect rect)
{
	Window::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_iFlags=iFlags;
	m_strMsgText=szText;
	m_bPressed=false;

	m_Type=ET_MESSAGEBOX;

	if(szCaption)
		SetText(szCaption);

	m_pEnvironment->SetFocus(this);
	RefreshControls();
}

void CMessageBox::Clear()
{
	RemoveChild(m_pText,true);
	RemoveChild(m_pOK,true);
	RemoveChild(m_pCancel,true);
	RemoveChild(m_pYes,true);
	RemoveChild(m_pNo,true);

	m_pOK=0;
	m_pCancel=0;
	m_pYes=0;
	m_pNo=0;
	m_pText=0;

	m_pEnvironment->SetFocus(0);

	Window::Clear();
}

void CMessageBox::RefreshControls()
{
	const ISkin *skin=m_pEnvironment->GetSkin();
	IElement *focusme=0;

	const int buttonheight=skin->GetSize(DS_BUTTONHEIGHT);
	const int buttonwidth=skin->GetSize(DS_BUTTONWIDTH);
	const int titleheight=skin->GetSize(DS_WINDOWBUTTONWIDTH) + 2;
	const int buttondistance=skin->GetSize(DS_WINDOWBUTTONWIDTH);

	Core::Vector2i dim(m_AbsoluteClippingRect.GetWidth() - buttonwidth,
		m_AbsoluteClippingRect.GetHeight() - (buttonheight * 3));
	const Core::Vector2i pos((m_AbsoluteClippingRect.GetWidth() - dim.x) / 2,
		buttonheight / 2 + titleheight);

	if(!m_pText)
	{
		m_pText=m_pEnvironment->AddStaticText(m_strMsgText.c_str(),
			Core::Rect(pos,pos+dim),false,false,this);
		m_pText->SetWordWrap(true);
		m_pText->SetSubElement(true);
	}
	else
	{
		m_pText->SetRelativePosition(Core::Rect(pos,pos+dim));
		m_pText->SetText(m_strMsgText.c_str());
	}

	const int textheight=m_pText->GetTextHeight();
	Core::Rect tmp(m_pText->GetRelativePosition());
	tmp.m_vLowerRight.y=tmp.m_vUpperLeft.y + textheight;
	m_pText->SetRelativePosition(tmp);
	dim.y=textheight;

	tmp=GetRelativePosition();
	int msgboxheight=textheight + (int)floorf(2.5f * (float)buttonheight) + titleheight;

	tmp.m_vUpperLeft.y=(m_pParent->GetAbsolutePosition().GetHeight() - msgboxheight) / 2;
	tmp.m_vLowerRight.y=tmp.m_vUpperLeft.y + msgboxheight;

	int countbuttons=0;
	if(m_iFlags & MBF_OK) countbuttons++;
	if(m_iFlags & MBF_CANCEL) countbuttons++;
	if(m_iFlags & MBF_YES) countbuttons++;
	if(m_iFlags & MBF_NO) countbuttons++;

	Core::Rect btnrect;
	btnrect.m_vUpperLeft.y=pos.y + dim.y + buttonheight / 2;
	btnrect.m_vLowerRight.y=btnrect.m_vUpperLeft.y + buttonheight;
	btnrect.m_vUpperLeft.x=(m_AbsoluteClippingRect.GetWidth() -
		((buttonwidth + buttondistance)*countbuttons)) / 2;
	btnrect.m_vLowerRight.x=btnrect.m_vUpperLeft.x + buttonwidth;

	if(m_iFlags & MBF_OK)
	{
		if(!m_pOK)
		{
			m_pOK=m_pEnvironment->AddButton(btnrect,this);
			m_pOK->SetSubElement(true);
		}
		else
			m_pOK->SetRelativePosition(btnrect);

		m_pOK->SetText(skin->GetDefaultText(DT_MSGBOXOK));

		btnrect.m_vLowerRight.x+=buttonwidth + buttondistance;
		btnrect.m_vUpperLeft.x+=buttonwidth + buttondistance;

		focusme=m_pOK;
	}
	else if(m_pOK)
		RemoveChild(m_pOK,true);

	if(m_iFlags & MBF_CANCEL)
	{
		if(!m_pCancel)
		{
			m_pCancel=m_pEnvironment->AddButton(btnrect,this);
			m_pCancel->SetSubElement(true);
		}
		else
			m_pCancel->SetRelativePosition(btnrect);

		m_pCancel->SetText(skin->GetDefaultText(DT_MSGBOXCANCEL));

		btnrect.m_vLowerRight.x+=buttonwidth + buttondistance;
		btnrect.m_vUpperLeft.x+=buttonwidth + buttondistance;

		if(!focusme)
			focusme=m_pCancel;
	}
	else if(m_pCancel)
		RemoveChild(m_pCancel,true);

	if(m_iFlags & MBF_YES)
	{
		if(!m_pYes)
		{
			m_pYes=m_pEnvironment->AddButton(btnrect,this);
			m_pYes->SetSubElement(true);
		}
		else
			m_pYes->SetRelativePosition(btnrect);

		m_pYes->SetText(skin->GetDefaultText(DT_MSGBOXYES));

		btnrect.m_vLowerRight.x+=buttonwidth + buttondistance;
		btnrect.m_vUpperLeft.x+=buttonwidth + buttondistance;

		if(!focusme)
			focusme=m_pYes;
	}
	else if(m_pYes)
		RemoveChild(m_pYes,true);

	if(m_iFlags & MBF_NO)
	{
		if(!m_pNo)
		{
			m_pNo=m_pEnvironment->AddButton(btnrect,this);
			m_pNo->SetSubElement(true);
		}
		else
			m_pNo->SetRelativePosition(btnrect);

		m_pNo->SetText(skin->GetDefaultText(DT_MSGBOXNO));

		btnrect.m_vLowerRight.x+=buttonwidth + buttondistance;
		btnrect.m_vUpperLeft.x+=buttonwidth + buttondistance;

		if(!focusme)
			focusme=m_pNo;
	}
	else if(m_pNo)
		RemoveChild(m_pNo,true);

	if(m_pEnvironment->HasFocus(this) && focusme)
		m_pEnvironment->SetFocus(focusme);
}

bool CMessageBox::OnEvent(const Event& _Event)
{
	Event e;
	e.EventType=ET_GUI;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;

	switch(_Event.EventType)
	{
		case ET_KEY:
			if(_Event.sKey.bPressed)
			{
				switch(_Event.sKey.cDXChar)
				{
					case DIK_RETURN:
						if(m_pOK)
						{
							m_pOK->SetPressed(true);
							m_bPressed=true;
						}
						break;

					case DIK_Y:
						if(m_pYes)
						{
							m_pYes->SetPressed(true);
							m_bPressed=true;
						}
						break;

					case DIK_N:
						if(m_pNo)
						{
							m_pNo->SetPressed(true);
							m_bPressed=true;
						}
						break;

					case DIK_ESCAPE:
						if(m_bPressed)
						{
							if(m_pOK) m_pOK->SetPressed(false);
							if(m_pYes) m_pYes->SetPressed(false);
							if(m_pNo) m_pNo->SetPressed(false);
							m_bPressed=false;
						}
						else if(m_pCancel)
						{
							m_pCancel->SetPressed(true);
							m_bPressed=true;
						}
						else if(m_pClose && m_pClose->IsVisible())
						{
							m_pClose->SetPressed(true);
							m_bPressed=true;
						}
						break;

					default:
						break;
				}
			}
			else if(m_bPressed)
			{
				if(m_pOK && _Event.sKey.cDXChar==DIK_RETURN)
				{
					e.sGUI.EventType=GE_MSGBOXOK;
					m_pParent->OnEvent(e);
					Remove();
					return true;
				}
				else if((m_pCancel || m_pClose) && _Event.sKey.cDXChar==DIK_ESCAPE)
				{
					e.sGUI.EventType=GE_MSGBOXCANCEL;
					m_pParent->OnEvent(e);
					Remove();
					return true;
				}
				else if(m_pYes && _Event.sKey.cDXChar==DIK_Y)
				{
					e.sGUI.EventType=GE_MSGBOXYES;
					m_pParent->OnEvent(e);
					Remove();
				}
				else if(m_pNo && _Event.sKey.cDXChar==DIK_N)
				{
					e.sGUI.EventType=GE_MSGBOXNO;
					m_pParent->OnEvent(e);
					Remove();
					return true;
				}
			}
			break;

		case ET_GUI:
			if(_Event.sGUI.EventType==GE_BUTTONCLICKED)
			{
				if(_Event.sGUI.pCaller==m_pOK)
				{
					e.sGUI.EventType=GE_MSGBOXOK;
					m_pParent->OnEvent(e);
					Remove();
					return true;
				}
				else if(_Event.sGUI.pCaller==m_pCancel ||
						_Event.sGUI.pCaller==m_pClose)
				{
					e.sGUI.EventType=GE_MSGBOXCANCEL;
					m_pParent->OnEvent(e);
					Remove();
					return true;
				}
				else if(_Event.sGUI.pCaller==m_pYes)
				{
					e.sGUI.EventType=GE_MSGBOXYES;
					m_pParent->OnEvent(e);
					Remove();
					return true;
				}
				else if(_Event.sGUI.pCaller==m_pNo)
				{
					e.sGUI.EventType=GE_MSGBOXNO;
					m_pParent->OnEvent(e);
					Remove();
					return true;
				}
			}
			break;

		default:
			break;
	}

	return Window::OnEvent(_Event);
}

}