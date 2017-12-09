
#include "StaticText.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IFont.h"
#include "../Core/Renderer2D.h"

namespace GUI
{

StaticText::StaticText(const char *szText,bool bBorder,IEnvironment *pEnvironment,
					   IElement *pParent, IEventReceiver *pUser,
					   uint32 dwID, const Core::Rect& rect,
					   bool bBackground) :
	IStaticText(pEnvironment,pParent,pUser,dwID,rect), m_bBorder(bBorder),
		m_HAlign(AL_UPPERLEFT),m_VAlign(AL_UPPERLEFT),
		m_bOverrideColor(false),m_bWordWrap(false),m_bBackground(bBackground),
		m_cOverrideColor(Core::Color32(101,255,255,255)),m_cBGColor(Core::Color32(101,210,210,210)),
		m_pOverrideFont(0),m_pLastBreakFont(0)
{
	m_strText=szText;
	if(pEnvironment && pEnvironment->GetSkin())
		m_cBGColor=pEnvironment->GetSkin()->GetColor(DC_3DFACE);
}

StaticText::~StaticText()
{
	if(m_pOverrideFont)
		m_pOverrideFont->Drop();
}

void StaticText::Reset(const char *szText,bool bBorder,IEnvironment *pEnvironment,
					   IElement *pParent, IEventReceiver *pUser,
					   uint32 dwID, const Core::Rect& rect,
					   bool bBackground)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,rect);

	m_bBorder=bBorder;
	m_bBackground=bBackground;

	m_strText=szText;
	if(pEnvironment && pEnvironment->GetSkin())
		m_cBGColor=pEnvironment->GetSkin()->GetColor(DC_3DFACE);
}

void StaticText::Clear()
{
	if(m_pOverrideFont)
		m_pOverrideFont->Drop();

	m_HAlign=m_VAlign=AL_UPPERLEFT;
	m_bOverrideColor=false;
	m_bWordWrap=false;
	m_cOverrideColor=Core::Color32(101,255,255,255);
	m_cBGColor=Core::Color32(101,210,210,210);
	m_pOverrideFont=0;
	m_pLastBreakFont=0;
	m_BrokenText.clear();

	IElement::Clear();
}

void StaticText::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	if(!skin)
		return;

	Core::Rect framerect(m_AbsoluteRect);
	framerect.ClipAgainst(m_AbsoluteClippingRect);

	if(m_bBackground)
		g_Renderer2D.DrawRectangle(framerect.m_vUpperLeft.x,
								 framerect.m_vUpperLeft.y,
								 framerect.m_vLowerRight.x,
								 framerect.m_vLowerRight.y,m_cBGColor);

	framerect=m_AbsoluteRect;

	if(m_bBorder)
	{
		skin->Draw3DSunkenPane(this,Core::Color32(0,0,0,0),true,false,framerect,&m_AbsoluteClippingRect);
		framerect.m_vUpperLeft.x+=skin->GetSize(DS_TEXTDISTANCEX);
	}

	if(m_strText.size())
	{
		IFont *font=m_pOverrideFont;
		if(!m_pOverrideFont)
			font=skin->GetFont();

		if(font)
		{
			if(!m_bWordWrap)
			{
				if(m_VAlign==AL_LOWERRIGHT)
				{
					framerect.m_vUpperLeft.y=framerect.m_vLowerRight.y -
						font->GetDimension("A").y - font->GetKerningHeight();
				}
				if(m_HAlign==AL_LOWERRIGHT)
				{
					framerect.m_vUpperLeft.x=framerect.m_vLowerRight.x -
						font->GetDimension(m_strText.c_str()).x;
				}

				font->Draw(m_strText.c_str(),framerect,
					m_bOverrideColor ? m_cOverrideColor : skin->GetColor(m_bIsEnable ? DC_BUTTONTEXT : DC_GRAYTEXT),
					m_HAlign==AL_CENTER,m_VAlign==AL_CENTER,&m_AbsoluteClippingRect);
			}
			else
			{
				if(font!=m_pLastBreakFont)
					BreakText();

				Core::Rect r(framerect);
				int height=font->GetDimension("A").y + font->GetKerningHeight();
				int totalheight=height * m_BrokenText.size();

				if(m_VAlign==AL_CENTER)
					r.m_vUpperLeft.y=r.GetCenter().y - (totalheight / 2);
				else if(m_VAlign==AL_LOWERRIGHT)
					r.m_vUpperLeft.y=r.m_vLowerRight.y - totalheight;

				uint32 i;
				for(i=0; i<m_BrokenText.size();i++)
				{
					if(m_HAlign==AL_LOWERRIGHT)
					{
						r.m_vUpperLeft.x=framerect.m_vLowerRight.x -
							font->GetDimension(m_BrokenText[i].c_str()).x;
					}

					font->Draw(m_BrokenText[i].c_str(),r,
						m_bOverrideColor ? m_cOverrideColor : skin->GetColor(m_bIsEnable ? DC_BUTTONTEXT : DC_GRAYTEXT),
						m_HAlign==AL_CENTER,false,&m_AbsoluteClippingRect);

					r.m_vLowerRight.y+=height;
					r.m_vUpperLeft.y+=height;
				}
			}
		}
	}

	IElement::Draw();
}

void StaticText::SetOverrideFont(IFont *font)
{
	if(m_pOverrideFont)
		m_pOverrideFont->Drop();

	m_pOverrideFont=font;

	if(m_pOverrideFont)
		m_pOverrideFont->Grab();

	BreakText();
}

IFont *StaticText::GetOverrideFont() const
{
	return m_pOverrideFont;
}

void StaticText::SetOverrideColor(Core::Color32 cColor)
{
	m_cOverrideColor=cColor;
	m_bOverrideColor=true;
}

void StaticText::SetBackgroundColor(Core::Color32 cColor)
{
	m_cBGColor=cColor;
	m_bBackground=true;
}

void StaticText::SetDrawBackground(bool bDraw)
{
	m_bBackground=bDraw;
}

void StaticText::SetDrawBorder(bool bDraw)
{
	m_bBorder=bDraw;
}

void StaticText::SetTextAlignment(ALIGNMENT Horizontal, ALIGNMENT Vertical)
{
	m_HAlign=Horizontal;
	m_VAlign=Vertical;
}

const Core::Color32& StaticText::GetOverrideColor() const
{
	return m_cOverrideColor;
}

void StaticText::EnableOverrideColor(bool bEnable)
{
	m_bOverrideColor=bEnable;
}

bool StaticText::IsOverrideColorEnabled() const
{
	return m_bOverrideColor;
}

void StaticText::SetWordWrap(bool bEnable)
{
	m_bWordWrap=bEnable;
	BreakText();
}

bool StaticText::IsWordWrapEnabled() const
{
	return m_bWordWrap;
}

void StaticText::BreakText()
{
	ISkin *skin=m_pEnvironment->GetSkin();

	if(!m_bWordWrap || !skin)
		return;

	m_BrokenText.clear();

	IFont *font=m_pOverrideFont;
	if(!m_pOverrideFont)
		font=skin->GetFont();

	if(!font)
		return;

	m_pLastBreakFont=font;

	std::string line,word,whitespace;
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

		if(c==' ' || c==0 || i==(size-1))
		{
			if(word.size())
			{
				int whitelgth=font->GetDimension(whitespace.c_str()).x;
				int wordlgth=font->GetDimension(word.c_str()).x;

				if(length + wordlgth + whitelgth > elwidth)
				{
					length=wordlgth;
					m_BrokenText.push_back(line);
					line=word;
				}
				else
				{
					line+=whitespace;
					line+=word;
					length+=whitelgth + wordlgth;
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
}

void StaticText::SetText(const char *szText)
{
	IElement::SetText(szText);
	BreakText();
}

void StaticText::UpdateAbsolutePosition()
{
	IElement::UpdateAbsolutePosition();
	BreakText();
}

int StaticText::GetTextHeight() const
{
	ISkin *skin=m_pEnvironment->GetSkin();

	if(!skin)
		return 0;

	IFont *font=m_pOverrideFont;
	if(!m_pOverrideFont)
		font=skin->GetFont();

	if(!font)
		return 0;

	int height=font->GetDimension("A").y + font->GetKerningHeight();

	if(m_bWordWrap)
		height*=m_BrokenText.size();

	return height;
}

int StaticText::GetTextWidth() const
{
	IFont *font=m_pOverrideFont;
	if(!m_pOverrideFont)
	{
		ISkin *skin=m_pEnvironment->GetSkin();
		if(skin)
			font=skin->GetFont();
	}

	if(!font)
		return 0;

	if(m_bWordWrap)
	{
		int widest=0;
		uint32 line;

		for(line=0;line < m_BrokenText.size();line++)
		{
			int width=font->GetDimension(m_BrokenText[line].c_str()).x;
			if(width > widest)
				widest=width;
		}
		
		return widest;
	}
	else
		return font->GetDimension(m_strText.c_str()).x;
}

}
