
#include "EditBox.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IFont.h"
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

namespace GUI
{

EditBox::EditBox(const char *szText, bool bBorder,IEnvironment *pEnvironment,
				 IElement *pParent, IEventReceiver *pUser,uint32 dwID, const Core::Rect &rect) :
	IEditBox(pEnvironment,pParent,pUser,dwID,rect), m_bMouseMarking(false),
		m_bBorder(bBorder),m_bOverrideColorEnabled(false),m_iMarkBegin(0),m_iMarkEnd(0),
		m_cOverrideColor(Core::Color32(101,255,255,255)),
		m_pOverrideFont(0),m_pLastBreakFont(0), m_iCursorPos(0), m_iHScrollPos(0),
		m_iVScrollPos(0),m_dwMax(0),m_cPasswordChar('*'),
		m_HAlign(AL_UPPERLEFT),m_VAlign(AL_CENTER),
		m_bWordWrap(true),m_bMultiLine(false),m_bPasswordBox(false),m_bAutoScroll(true)
{
	m_strText=szText;

	SetTabStop(true);
	SetTabOrder(-1);

	BreakText();
}

EditBox::~EditBox()
{
	if(m_pOverrideFont)
		m_pOverrideFont->Drop();
}

void EditBox::Reset(const char *szText, bool bBorder,IEnvironment *pEnvironment,
				 IElement *pParent, IEventReceiver *pUser,uint32 dwID, const Core::Rect &rect)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_bBorder=bBorder;
	m_strText=szText;

	SetTabStop(true);
	SetTabOrder(-1);

	BreakText();
}

void EditBox::Clear()
{
	if(m_pOverrideFont)
		m_pOverrideFont->Drop();

	m_bMouseMarking=false;
	m_bOverrideColorEnabled=false;
	m_iMarkBegin=m_iMarkEnd=0;
	m_cOverrideColor=Core::Color32(101,255,255,255);
	m_pOverrideFont=0;
	m_pLastBreakFont=0;
	m_iCursorPos=0;
	m_iHScrollPos=m_iVScrollPos=0;
	m_dwMax=0;
	m_cPasswordChar='*';
	m_HAlign=AL_UPPERLEFT;
	m_VAlign=AL_CENTER;
	m_BrokenText.clear();
	m_BrokenTextPositions.clear();
	m_bMultiLine=false;
	m_bWordWrap=false;
	m_bPasswordBox=false;
	m_bAutoScroll=true;

	IElement::Clear();
}

void EditBox::SetOverrideFont(IFont *pFont)
{
	if(m_pOverrideFont)
		m_pOverrideFont->Drop();

	m_pOverrideFont=pFont;

	if(m_pOverrideFont)
		m_pOverrideFont->Grab();

	BreakText();
}

void EditBox::SetOverrideColor(Core::Color32 cColor)
{
	m_cOverrideColor=cColor;
	m_bOverrideColorEnabled=true;
}

void EditBox::SetDrawBorder(bool bBorder)
{
	m_bBorder=bBorder;
}

void EditBox::EnableOverrideColor(bool bEnable)
{
	m_bOverrideColorEnabled=bEnable;
}

void EditBox::SetWordWrap(bool bEnable)
{
	m_bWordWrap=bEnable;
	BreakText();
}

void EditBox::UpdateAbsolutePosition()
{
	IElement::UpdateAbsolutePosition();
	BreakText();
}

bool EditBox::IsWordWrapEnabled() const
{
	return m_bWordWrap;
}

void EditBox::SetMultiLine(bool bEnable)
{
	m_bMultiLine=bEnable;
}

bool EditBox::IsMultiLineEnabled() const
{
	return m_bMultiLine;
}

void EditBox::SetPasswordBox(bool bPasswordBox, char cPasswordChar)
{
	m_bPasswordBox=bPasswordBox;
	if(m_bPasswordBox)
	{
		m_cPasswordChar=cPasswordChar;
		SetMultiLine(false);
		SetWordWrap(false);
		m_BrokenText.clear();
	}
}

bool EditBox::IsPasswordBox() const
{
	return m_bPasswordBox;
}

void EditBox::SetTextAlignment(ALIGNMENT Horizontal, ALIGNMENT Vertical)
{
	m_HAlign=Horizontal;
	m_VAlign=Vertical;
}

bool EditBox::OnEvent(const Event& _Event)
{
	switch(_Event.EventType)
	{
		case ET_GUI:
			if(_Event.sGUI.EventType==GE_FOCUSLOST)
			{
				if(_Event.sGUI.pCaller==this)
				{
					m_bMouseMarking=false;
					m_iMarkBegin=0;
					m_iMarkEnd=0;
				}
			}
			break;

		case ET_KEY:
			if(ProcessKey(_Event))
				return true;
			break;

		case ET_MOUSE:
			if(ProcessMouse(_Event))
				return true;
			break;

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

bool EditBox::ProcessKey(const Event &e)
{
	if(!e.sKey.bPressed)
		return false;

	bool textchanged=false;

	if(e.sKey.bControl)
	{
		switch(e.sKey.cDXChar)
		{
			case DIK_A:
				m_iMarkBegin=0;
				m_iMarkEnd=m_strText.size();
				break;

			case DIK_HOME:
				if(e.sKey.bShift)
				{
					m_iMarkEnd=m_iCursorPos;
					m_iMarkBegin=0;
					m_iCursorPos=0;
				}
				else
				{
					m_iCursorPos=0;
					m_iMarkBegin=0;
					m_iMarkEnd=0;
				}
				break;

			case DIK_END:
				if(e.sKey.bShift)
				{
					m_iMarkBegin=m_iCursorPos;
					m_iMarkEnd=m_strText.size();
					m_iCursorPos=0;
				}
				else
				{
					m_iCursorPos=m_strText.size();
					m_iMarkBegin=0;
					m_iMarkEnd=0;
				}
				break;

			default:
				return false;
		}
	}

	if(!e.sKey.bControl)
	{


		switch(e.sKey.cDXChar)
		{
			case DIK_END:
				{
					int p=m_strText.size();
					if(m_bWordWrap || m_bMultiLine)
					{
						p=GetLineFromPos(m_iCursorPos);
						p=m_BrokenTextPositions[p] + (int)m_BrokenText[p].size();
						if(p > 0 && (m_strText[p-1]=='\r' || m_strText[p-1]=='\n'))
							p-=1;
					}

					if(e.sKey.bShift)
					{
						if(m_iMarkBegin==m_iMarkEnd)
							m_iMarkBegin=m_iCursorPos;

						m_iMarkEnd=p;
					}
					else
					{
						m_iMarkBegin=0;
						m_iMarkEnd=0;
					}

					m_iCursorPos=p;
					m_dwBlinkStartTime=timeGetTime();
				}
				break;

			case DIK_HOME:
				{
					int p=0;
					if(m_bWordWrap || m_bMultiLine)
					{
						p=GetLineFromPos(m_iCursorPos);
						p=m_BrokenTextPositions[p];
					}

					if(e.sKey.bShift)
					{
						if(m_iMarkBegin==m_iMarkEnd)
							m_iMarkBegin=m_iCursorPos;

						m_iMarkEnd=p;
					}
					else
					{
						m_iMarkBegin=0;
						m_iMarkEnd=0;
					}

					m_iCursorPos=p;
					m_dwBlinkStartTime=timeGetTime();
				}
				break;

			case DIK_RETURN:
				if(m_bMultiLine)
					InputChar('\n');
				else
				{
					Event ev;
					ev.EventType=ET_GUI;
					ev.sGUI.pCaller=this;
					ev.sGUI.pElement=0;
					ev.sGUI.EventType=GE_EDITBOXENTER;
					SendEvent(ev);
				}
				break;

			case DIK_LEFT:
				if(e.sKey.bShift)
				{
					if(m_iCursorPos > 0)
					{
						if(m_iMarkBegin==m_iMarkEnd)
							m_iMarkBegin=m_iCursorPos;

						m_iMarkEnd=m_iCursorPos+1;
					}
				}
				else
				{
					m_iMarkBegin=0;
					m_iMarkEnd=0;
				}

				if(m_iCursorPos > 0) m_iCursorPos--;
				m_dwBlinkStartTime=timeGetTime();
				break;

			case DIK_RIGHT:
				if(e.sKey.bShift)
				{
					if(m_strText.size() > (uint32)m_iCursorPos)
					{
						if(m_iMarkBegin==m_iMarkEnd)
							m_iMarkBegin=m_iCursorPos;

						m_iMarkEnd=m_iCursorPos+1;
					}
				}
				else
				{
					m_iMarkBegin=0;
					m_iMarkEnd=0;
				}

				if(m_strText.size() > (uint32)m_iCursorPos) m_iCursorPos++;
				m_dwBlinkStartTime=timeGetTime();
				break;

			case DIK_UP:
				if(m_bMultiLine || (m_bWordWrap && m_BrokenText.size() > 1))
				{
					int lineno=GetLineFromPos(m_iCursorPos);
					int mb=(m_iMarkBegin==m_iMarkEnd) ? m_iCursorPos : (m_iMarkBegin > m_iMarkEnd ? m_iMarkBegin : m_iMarkEnd);
					if(lineno > 0)
					{
						int cp=m_iCursorPos - m_BrokenTextPositions[lineno];
						if((int)m_BrokenText[lineno-1].size() < cp)
							m_iCursorPos=m_BrokenTextPositions[lineno-1] + (int)m_BrokenText[lineno-1].size()-1;
						else
							m_iCursorPos=m_BrokenTextPositions[lineno-1] + cp;
					}

					if(e.sKey.bShift)
					{
						m_iMarkBegin=mb;
						m_iMarkEnd=m_iCursorPos;
					}
					else
					{
						m_iMarkBegin=0;
						m_iMarkEnd=0;
					}
				}
				else
					return false;

				break;

			case DIK_DOWN:
				if(m_bMultiLine || (m_bWordWrap && m_BrokenText.size() > 1))
				{
					int lineno=GetLineFromPos(m_iCursorPos);
					int mb=(m_iMarkBegin==m_iMarkEnd) ? m_iCursorPos : (m_iMarkBegin < m_iMarkEnd ? m_iMarkBegin : m_iMarkEnd);
					if(lineno < (int)m_BrokenText.size()-1)
					{
						int cp=m_iCursorPos - m_BrokenTextPositions[lineno];
						if((int)m_BrokenText[lineno+1].size() < cp)
							m_iCursorPos=m_BrokenTextPositions[lineno+ 1] + m_BrokenText[lineno+1].size()-1;
						else
							m_iCursorPos=m_BrokenTextPositions[lineno+ 1] + cp;
					}

					if(e.sKey.bShift)
					{
						m_iMarkBegin=mb;
						m_iMarkEnd=m_iCursorPos;
					}
					else
					{
						m_iMarkBegin=0;
						m_iMarkEnd=0;
					}
				}
				else
					return false;

				break;

			case DIK_BACK:
				if(!this->m_bIsEnable)
					break;

				if(m_strText.size())
				{
					std::string s;
					if(m_iMarkBegin!=m_iMarkEnd)
					{
						int realmbgn=m_iMarkBegin < m_iMarkEnd ? m_iMarkBegin : m_iMarkEnd;
						int realmend=m_iMarkBegin < m_iMarkEnd ? m_iMarkEnd : m_iMarkBegin;

						s=m_strText.substr(0,realmbgn);
						s.append(m_strText.substr(realmend,m_strText.size()-realmend));
						m_strText=s;

						m_iCursorPos=realmbgn;
					}
					else
					{
						if(m_iCursorPos > 0)
							s=m_strText.substr(0,m_iCursorPos-1);
						else
							s="";

						s.append(m_strText.substr(m_iCursorPos,m_strText.size() - m_iCursorPos));
						m_strText=s;
						m_iCursorPos--;
					}

					if(m_iCursorPos < 0) m_iCursorPos=0;
					m_dwBlinkStartTime=timeGetTime();
					m_iMarkBegin=0;
					m_iMarkEnd=0;
					textchanged=true;
				}
				break;

			case DIK_DELETE:
				if(!this->m_bIsEnable)
					break;

				if(m_strText.size()!=0)
				{
					std::string s;

					if(m_iMarkBegin!=m_iMarkEnd)
					{
						int realmbgn=m_iMarkBegin < m_iMarkEnd ? m_iMarkBegin : m_iMarkEnd;
						int realmend=m_iMarkBegin < m_iMarkEnd ? m_iMarkEnd : m_iMarkBegin;

						s=m_strText.substr(0,realmbgn);
						s.append(m_strText.substr(realmend,m_strText.size()-realmend));
						m_strText=s;

						m_iCursorPos=realmbgn;
					}
					else
					{
						s=m_strText.substr(0,m_iCursorPos);
						s.append(m_strText.substr(m_iCursorPos+1,m_strText.size()-m_iCursorPos-1));
						m_strText=s;
					}

					if(m_iCursorPos > (int)m_strText.size())
						m_iCursorPos=(int)m_strText.size();

					m_dwBlinkStartTime=timeGetTime();
					m_iMarkBegin=0;
					m_iMarkEnd=0;
					textchanged=true;
				}
				break;

			case DIK_ESCAPE:
			case DIK_TAB:
			case DIK_LSHIFT:
			case DIK_RSHIFT:
			case DIK_F1:
			case DIK_F2:
			case DIK_F3:
			case DIK_F4:
			case DIK_F5:
			case DIK_F6:
			case DIK_F7:
			case DIK_F8:
			case DIK_F9:
			case DIK_F10:
			case DIK_F11:
			case DIK_F12:
			case DIK_F14:
				return false;

			default:
				InputChar(e.sKey.cChar);
				break;
		}
	}

	if(textchanged)
		BreakText();

	CalculateScrollPos();

	return true;
}

void EditBox::Draw()
{
	if(!m_bIsVisible)
		return;

	bool focus=m_pEnvironment->HasFocus(this);

	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return;

	m_FrameRect=m_AbsoluteRect;

	if(m_bBorder)
	{
		skin->Draw3DSunkenPane(this,skin->GetColor(DC_WINDOW),
			false,true,m_FrameRect,&m_AbsoluteClippingRect);

		m_FrameRect.m_vUpperLeft.x+=skin->GetSize(DS_TEXTDISTANCEX)+1;
		m_FrameRect.m_vUpperLeft.y+=skin->GetSize(DS_TEXTDISTANCEY)+1;
		m_FrameRect.m_vLowerRight.x-=skin->GetSize(DS_TEXTDISTANCEX)+1;
		m_FrameRect.m_vLowerRight.y-=skin->GetSize(DS_TEXTDISTANCEY)+1;
	}

	Core::Rect localcliprect(m_FrameRect);
	localcliprect.ClipAgainst(m_AbsoluteClippingRect);

	IFont *font=m_pOverrideFont;
	if(!m_pOverrideFont)
		font=skin->GetFont();

	int cursorline=0;
	int charcursorpos=0;

	if(font)
	{
		if(m_pLastBreakFont!=font)
			BreakText();

		std::string *txtline=&m_strText;
		int startpos=0;

		std::string s,s2;

		bool ml=(!m_bPasswordBox && (m_bWordWrap || m_bMultiLine));
		int realmbgn=m_iMarkBegin < m_iMarkEnd ? m_iMarkBegin : m_iMarkEnd;
		int realmend=m_iMarkBegin < m_iMarkEnd ? m_iMarkEnd : m_iMarkBegin;
		int hlinestart=ml ? GetLineFromPos(realmbgn) : 0;
		int hlinecount=ml ? GetLineFromPos(realmend) - hlinestart+1 : 1;
		int linecount=ml ? m_BrokenText.size() : 1;

		bool prevover=m_bOverrideColorEnabled;
		Core::Color32 prevcolor(m_cOverrideColor);

		if(m_strText.size())
		{
			if(!m_bIsEnable && !m_bOverrideColorEnabled)
			{
				m_bOverrideColorEnabled=true;
				m_cOverrideColor=skin->GetColor(DC_GRAYTEXT);
			}

			int i;
			for(i=0;i<linecount;i++)
			{
				SetTextRect(i);

				Core::Rect c(localcliprect);
				c.ClipAgainst(m_CurrentTextRect);
				if(!c.IsValid())
					continue;

				if(m_bPasswordBox)
				{
					if(m_BrokenText.size()!=1)
					{
						m_BrokenText.clear();
						m_BrokenText.push_back(std::string());
					}
					if(m_BrokenText[0].size()!=m_strText.size())
					{
						m_BrokenText[0]=m_strText;
						uint32 q;
						for(q=0;q<m_strText.size();q++)
							m_BrokenText[0][q]=m_cPasswordChar;
					}
					txtline=&m_BrokenText[0];
					startpos=0;
				}
				else
				{
					txtline=ml ? &m_BrokenText[i] : &m_strText;
					startpos=ml ? m_BrokenTextPositions[i] : 0;
				}

				font->Draw(txtline->c_str(),m_CurrentTextRect,
					m_bOverrideColorEnabled ? m_cOverrideColor : skin->GetColor(DC_BUTTONTEXT),
					false,true,&localcliprect);

				if(focus && m_iMarkBegin!=m_iMarkEnd && i>= hlinestart && i< hlinestart + hlinecount)
				{
					int mbegin=0, mend=0;
					int linestartpos=0, lineendpos=txtline->size();

					if(i==hlinestart)
					{
						s=txtline->substr(0,realmbgn - startpos);
						mbegin=font->GetDimension(s.c_str()).x;
						linestartpos=realmbgn - startpos;
					}
					if(i==hlinestart + hlinecount-1)
					{
						s2=txtline->substr(0,realmend - startpos);
						mend=font->GetDimension(s2.c_str()).x;
						lineendpos=(int)s2.size();
					}
					else
						mend=font->GetDimension(txtline->c_str()).x;

					m_CurrentTextRect.m_vUpperLeft.x+=mbegin;
					m_CurrentTextRect.m_vLowerRight.x=m_CurrentTextRect.m_vUpperLeft.x + mend - mbegin;

					skin->Draw2DRectangle(this,skin->GetColor(DC_HIGHLIGHT),m_CurrentTextRect,&localcliprect);

					s=txtline->substr(linestartpos,lineendpos-linestartpos);

					if(s.size())
						font->Draw(s.c_str(),m_CurrentTextRect,
						m_bOverrideColorEnabled ? m_cOverrideColor : skin->GetColor(DC_HIGHLIGHTTEXT),
						false,true,&localcliprect);
				}
			}

			m_bOverrideColorEnabled=prevover;
			m_cOverrideColor=prevcolor;
		}

		if(m_bWordWrap || m_bMultiLine)
		{
			cursorline=GetLineFromPos(m_iCursorPos);
			txtline=&m_BrokenText[cursorline];
			startpos=m_BrokenTextPositions[cursorline];
		}
		s=txtline->substr(0,m_iCursorPos-startpos);
		charcursorpos=font->GetDimension(s.c_str()).x;

		if(focus && (timeGetTime() - m_dwBlinkStartTime) % 700 < 350)
		{
			SetTextRect(cursorline);
			m_CurrentTextRect.m_vUpperLeft.x+=charcursorpos;

			font->Draw("_",m_CurrentTextRect,
				m_bOverrideColorEnabled ? m_cOverrideColor : skin->GetColor(DC_BUTTONTEXT),
				false,true,&localcliprect);
		}
	}
}

void EditBox::SetText(const char *szText)
{
	m_strText=szText;
	m_iCursorPos=0;
	m_iHScrollPos=0;
	m_iMarkBegin=0;
	m_iMarkEnd=0;
	BreakText();
}

void EditBox::SetAutoScroll(bool bEnable)
{
	m_bAutoScroll=bEnable;
}

bool EditBox::IsAutoScrollEnabled() const
{
	return m_bAutoScroll;
}

Core::Vector2i EditBox::GetTextDimension()
{
	Core::Rect ret;

	SetTextRect(0);
	ret=m_CurrentTextRect;
	uint32 i;

	for(i=1;i<m_BrokenText.size();i++)
	{
		SetTextRect(i);
		ret.AddPoint(m_CurrentTextRect.m_vUpperLeft);
		ret.AddPoint(m_CurrentTextRect.m_vLowerRight);
	}

	return ret.GetSize();
}

void EditBox::SetMax(uint32 dwMax)
{
	m_dwMax=dwMax;

	if(m_strText.size() > m_dwMax && m_dwMax!=0)
		m_strText=m_strText.substr(0,m_dwMax);
}

uint32 EditBox::GetMax() const
{
	return m_dwMax;
}

bool EditBox::ProcessMouse(const Event &e)
{
	switch(e.sMouse.EventType)
	{
		case ME_LMOUSEUP:
			if(m_pEnvironment->HasFocus(this))
			{
				m_iCursorPos=GetCursorPos((int)e.sMouse.dwX,(int)e.sMouse.dwY);
				if(m_bMouseMarking)
					m_iMarkEnd=m_iCursorPos;
				m_bMouseMarking=false;
				CalculateScrollPos();
				return true;
			}
			break;

		case ME_MOVED:
			if(m_bMouseMarking)
			{
				m_iCursorPos=GetCursorPos((int)e.sMouse.dwX,(int)e.sMouse.dwY);
				m_iMarkEnd=m_iCursorPos;
				CalculateScrollPos();
				return true;
			}
			break;

		case ME_LMOUSEDOWN:
			if(!m_pEnvironment->HasFocus(this))
			{
				m_dwBlinkStartTime=timeGetTime();
				m_pEnvironment->SetFocus(this);
				m_bMouseMarking=true;
				m_iCursorPos=GetCursorPos((int)e.sMouse.dwX,(int)e.sMouse.dwY);
				m_iMarkBegin=m_iCursorPos;
				m_iMarkEnd=m_iCursorPos;
				CalculateScrollPos();
				return true;
			}
			else
			{
				if(!m_AbsoluteClippingRect.IsPointInside(Core::Vector2i((int)e.sMouse.dwX,(int)e.sMouse.dwY)))
					return false;
				else
				{
					m_iCursorPos=GetCursorPos((int)e.sMouse.dwX,(int)e.sMouse.dwY);

					if(!m_bMouseMarking)
						m_iMarkBegin=m_iCursorPos;

					m_bMouseMarking=true;
					m_iMarkEnd=m_iCursorPos;
					CalculateScrollPos();
					return true;
				}
			}

		default:
			break;
	}

	return false;
}

int EditBox::GetCursorPos(int x, int y)
{
	IFont *font=m_pOverrideFont;
	ISkin *skin=m_pEnvironment->GetSkin();

	if(!m_pOverrideFont)
		font=skin->GetFont();

	uint32 linecount=1;
	if(m_bWordWrap || m_bMultiLine)
		linecount=m_BrokenText.size();

	std::string *txtline=&m_strText;
	int startpos=0;
	x+=3;
	uint32 i;

	for(i=0;i<linecount;i++)
	{
		SetTextRect(i);
		if(i==0 && y< m_CurrentTextRect.m_vUpperLeft.y)
			y=m_CurrentTextRect.m_vUpperLeft.y;

		if(i==linecount-1 && y < m_CurrentTextRect.m_vLowerRight.y)
			y=m_CurrentTextRect.m_vLowerRight.y;

		if(y >= m_CurrentTextRect.m_vUpperLeft.y && y <= m_CurrentTextRect.m_vLowerRight.y)
		{
			txtline=(m_bWordWrap || m_bMultiLine) ? &m_BrokenText[i] : &m_strText;
			startpos=(m_bWordWrap || m_bMultiLine) ? m_BrokenTextPositions[i] : 0;
			break;
		}
	}

	if(x < m_CurrentTextRect.m_vUpperLeft.x)
		x=m_CurrentTextRect.m_vUpperLeft.x;

	int idx=font->GetCharacterFromPos(m_strText.c_str(),x - m_CurrentTextRect.m_vUpperLeft.x);

	if(idx!=-1)
		return idx + startpos;

	return txtline->size() + startpos;
}

void EditBox::BreakText()
{
	ISkin *skin=m_pEnvironment->GetSkin();

	if((!m_bWordWrap && !m_bMultiLine) || !skin)
		return;

	m_BrokenText.clear();
	m_BrokenTextPositions.clear();

	IFont *font=m_pOverrideFont;
	if(!m_pOverrideFont)
		font=skin->GetFont();

	if(!font)
		return;

	m_pLastBreakFont=font;
	
	std::string line;
	std::string word;
	std::string whitespace;
	int lastlinestart=0;
	int size=m_strText.size();
	int length=0;
	int elwidth=m_RelativeRect.GetWidth() - 6;
	char c;
	int i;

	for(i=0;i<size;i++)
	{
		c=m_strText[i];
		bool linebreak=false;

		if(c=='\r')
		{
			linebreak=true;
			c=' ';
			if(m_strText[i+1]=='\n')
			{
				m_strText.erase(i+1);
				size--;
			}
		}
		else if(c=='\n')
		{
			linebreak=true;
			c=' ';
		}

		if(!m_bMultiLine)
			linebreak=false;

		if(c==' ' || c==0 || i==(size-1))
		{
			if(word.size())
			{
				int whitelgth=font->GetDimension(whitespace.c_str()).x;
				int worldlgth=font->GetDimension(word.c_str()).x;

				if(m_bWordWrap && length + worldlgth + whitelgth > elwidth)
				{
					length=worldlgth;
					m_BrokenText.push_back(line);
					m_BrokenTextPositions.push_back(lastlinestart);
					lastlinestart=i-(int)word.size();
					line=word;
				}
				else
				{
					line+=whitespace;
					line+=word;
					length+=whitelgth + worldlgth;
				}

				word="";
				whitespace="";
			}

			whitespace+=c;

			if(linebreak)
			{
				line+=whitespace;
				line+=word;
				m_BrokenText.push_back(line);
				m_BrokenTextPositions.push_back(lastlinestart);
				lastlinestart=i+1;
				line="";
				word="";
				whitespace="";
				length=0;
			}
		}
		else
			word+=c;
	}

	line+=whitespace;
	line+=word;
	m_BrokenText.push_back(line);
	m_BrokenTextPositions.push_back(lastlinestart);
}

void EditBox::SetTextRect(int line)
{
	Core::Vector2i d;
	int linecount=1;

	IFont *font=m_pOverrideFont;
	ISkin *skin=m_pEnvironment->GetSkin();
	if(!m_pOverrideFont)
		font=skin->GetFont();

	if(m_bWordWrap || m_bMultiLine)
	{
		linecount=m_BrokenText.size();
		d=font->GetDimension(m_BrokenText[line].c_str());
	}
	else
	{
		d=font->GetDimension(m_strText.c_str());
		d.y=m_AbsoluteRect.GetHeight();
	}

	d.y+=font->GetKerningHeight();

	switch(m_HAlign)
	{
		case AL_CENTER:
			m_CurrentTextRect.m_vUpperLeft.x=(m_FrameRect.GetWidth() / 2) - (d.x / 2);
			m_CurrentTextRect.m_vLowerRight.x=(m_FrameRect.GetWidth() / 2) + (d.x / 2);
			break;

		case AL_LOWERRIGHT:
			m_CurrentTextRect.m_vUpperLeft.x=m_FrameRect.GetWidth() - d.x;
			m_CurrentTextRect.m_vLowerRight.x=m_FrameRect.GetWidth();
			break;

		default:
			m_CurrentTextRect.m_vUpperLeft.x=0;
			m_CurrentTextRect.m_vLowerRight.x=d.x;
			break;
	}

	switch(m_VAlign)
	{
		case AL_CENTER:
			m_CurrentTextRect.m_vUpperLeft.y=(m_FrameRect.GetHeight() / 2) -
				(linecount * d.y) / 2 + d.y * line;
			break;

		case AL_LOWERRIGHT:
			m_CurrentTextRect.m_vUpperLeft.y=m_FrameRect.GetHeight() -
				linecount*d.y + d.y*line;
			break;

		default:
			m_CurrentTextRect.m_vUpperLeft.y=d.y * line;
			break;
	}
	m_CurrentTextRect.m_vUpperLeft.x-=m_iHScrollPos;
	m_CurrentTextRect.m_vLowerRight.x-=m_iHScrollPos;
	m_CurrentTextRect.m_vUpperLeft.y-=m_iVScrollPos;
	m_CurrentTextRect.m_vLowerRight.y=m_CurrentTextRect.m_vUpperLeft.y + d.y;

	m_CurrentTextRect+=m_FrameRect.m_vUpperLeft;
}

int EditBox::GetLineFromPos(int iPos)
{
	if(!m_bWordWrap && !m_bMultiLine)
		return 0;

	int i=0;
	while(i < (int)m_BrokenTextPositions.size())
	{
		if(m_BrokenTextPositions[i] > iPos)
			return i-1;

		i++;
	}

	return (int)m_BrokenTextPositions.size()-1;
}

void EditBox::InputChar(char c)
{
	if(!m_bIsEnable)
		return;

	if(c!=0)
	{
		if(m_strText.size() < m_dwMax || m_dwMax==0)
		{
			std::string s;
			
			if(m_iMarkBegin!=m_iMarkEnd)
			{
				int realmbgn=m_iMarkBegin < m_iMarkEnd ? m_iMarkBegin : m_iMarkEnd;
				int realmend=m_iMarkBegin < m_iMarkEnd ? m_iMarkEnd : m_iMarkBegin;

				s=m_strText.substr(0,realmbgn);
				s+=c;
				s.append(m_strText.substr(realmend,m_strText.size()-realmend));
				m_strText=s;
				m_iCursorPos=realmbgn+1;
			}
			else
			{
				s=m_strText.substr(0,m_iCursorPos);
				s+=c;
				s.append(m_strText.substr(m_iCursorPos,m_strText.size() - m_iCursorPos));
				m_strText=s;
				m_iCursorPos++;
			}

			m_dwBlinkStartTime=timeGetTime();
			m_iMarkBegin=0;
			m_iMarkEnd=0;
		}
	}
	
	BreakText();
}

void EditBox::CalculateScrollPos()
{
	if(!m_bAutoScroll)
		return;

	int cursline=GetLineFromPos(m_iCursorPos);
	SetTextRect(cursline);

	if(!m_bWordWrap)
	{
		IFont *font=m_pOverrideFont;
		ISkin *skin=m_pEnvironment->GetSkin();
		if(!m_pOverrideFont)
			font=skin->GetFont();

		std::string *txtline=m_bMultiLine ? &m_BrokenText[cursline] : &m_strText;
		int cpos=m_bMultiLine ? m_iCursorPos - m_BrokenTextPositions[cursline] : m_iCursorPos;

		int cstart=m_CurrentTextRect.m_vUpperLeft.x + m_iHScrollPos +
			font->GetDimension(txtline->substr(0,cpos).c_str()).x;
		int cend=cstart + font->GetDimension("_ ").x;

		if(m_FrameRect.m_vLowerRight.x < cend)
			m_iHScrollPos=cend-m_FrameRect.m_vLowerRight.x;
		else if(m_FrameRect.m_vUpperLeft.x > cstart)
			m_iHScrollPos=cstart - m_FrameRect.m_vUpperLeft.x;
		else
			m_iHScrollPos=0;
	}

	if(m_FrameRect.m_vLowerRight.y < m_CurrentTextRect.m_vLowerRight.y + m_iVScrollPos)
		m_iVScrollPos=m_CurrentTextRect.m_vLowerRight.y - m_FrameRect.m_vLowerRight.y + m_iVScrollPos;
	else if(m_FrameRect.m_vUpperLeft.y > m_CurrentTextRect.m_vUpperLeft.y + m_iVScrollPos)
		m_iVScrollPos=m_CurrentTextRect.m_vUpperLeft.y - m_FrameRect.m_vUpperLeft.y + m_iVScrollPos;
	else
		m_iVScrollPos=0;
}

}
