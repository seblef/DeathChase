
#include "Font.h"
#include "CSpriteBank.h"
#include "IEnvironment.h"
#include "../Core/ColorConverter.h"
#include "../Core/TextureMgr.h"

namespace GUI
{

Font::Font(IEnvironment *pEnvironment, const char *szFileName) :
	m_pSpriteBank(0), m_pEnvironment(pEnvironment),m_dwWrongCharacter(0),
	m_iMaxHeight(0),m_iGlobalKerningWidth(0), m_iGlobalKerningHeight(0)
{
	if(m_pEnvironment)
		m_pSpriteBank=m_pEnvironment->AddEmptySpriteBank(szFileName);
	else
		m_pSpriteBank=new SpriteBank(0);
}

Font::~Font()
{
	if(m_pSpriteBank)
		m_pSpriteBank->Drop();
}

void Font::SetMaxHeight()
{
	m_iMaxHeight=0;
	int t;

	std::vector<Core::Rect>& p=m_pSpriteBank->GetPositions();
	int i;

	for(i=0;i<p.size();i++)
	{
		t=p[i].GetHeight();
		if(t > m_iMaxHeight)
			m_iMaxHeight=t;
	}
}

bool Font::Load(const char *szFileName)
{
	MYASSERT(szFileName,"Bad font filename",true);

	Core::BMPImage Image;

	char szTemp[1024];
	strcpy(szTemp,"GUI\\Fonts\\");
	strcat(szTemp,szFileName);

	if(!Image.Load(szTemp))
	{
		g_Output->AddMsg("x Failed Image.Load(szTemp); szTemp=%s\n",szTemp);
		return false;
	}
	else
		return LoadTexture(&Image,szFileName);
}

bool Font::LoadTexture(Core::BMPImage *pImage, const char *szName)
{
	int iLowerRightPositions=0;

	if(pImage->GetBPP()!=24 || pImage->GetWidth()==0 || pImage->GetHeight()==0)
	{
		g_Output->AddMsg("x Bad font bitmap format: W:%i, H: %i, BPP: %i\n",
			pImage->GetWidth(),pImage->GetHeight(),pImage->GetBPP());
		return false;
	}

	pImage->MirrorY();

	uint32 *pTempBuffer=new uint32[pImage->GetWidth() * pImage->GetHeight()];
	if(!pTempBuffer)
		return false;

	Core::ColorConverter::ConvertR8G8B8ToA8R8G8B8((void *)pImage->GetBuffer(),pTempBuffer,pImage->GetWidth()*pImage->GetHeight());
	ReadPosition32bit(pTempBuffer,pImage->GetWidth(),pImage->GetHeight(),iLowerRightPositions);

	m_dwWrongCharacter=GetAreaFromCharacter(' ');

	if(!iLowerRightPositions || !m_pSpriteBank->GetSprites().size())
		g_Output->AddMsg("The amount of upper corner pixels or lower corner pixels is == 0, font file may be corrupted.\n");
	else if(iLowerRightPositions!=(int)m_pSpriteBank->GetSprites().size())
		g_Output->AddMsg("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.\n");

	bool bRet=(!m_pSpriteBank->GetSprites().empty() && iLowerRightPositions);
	if(bRet)
	{
		Core::HTexture hTex=g_TextureMgr.CreateTexture(pImage->GetWidth(),pImage->GetHeight(),
			D3DFMT_A8R8G8B8,(void *)pTempBuffer,szName);
		g_TextureMgr.CompressTexture(hTex);
		m_pSpriteBank->AddTexture(hTex);
	}
	else
		g_Output->AddMsg("x Erreur lors du chargement de la font %s\n",szName);

	delete[] pTempBuffer;
	SetMaxHeight();

	return bRet;
}

void Font::ReadPosition32bit(uint32 *pBuffer, int iWidth, int iHeight, int& iLowerRight)
{
	if(!pBuffer)
		return;

	const Core::Vector2i vSize(iWidth, iHeight);

	pBuffer[0]|=0xff000000;

	uint32 dwColorTopLeft=pBuffer[0];
	uint32 dwColorLowerRight=*(pBuffer+1);
	uint32 dwColorBackGround=*(pBuffer+2);
	uint32 dwColorBackGroundTransparent=0;

	*(pBuffer+1)=dwColorBackGround;

	Core::Vector2i vPos(0,0);
	for(vPos.y;vPos.y<vSize.y;vPos.y++)
	{
		for(vPos.x=0;vPos.x<vSize.x;vPos.x++)
		{
			if(*pBuffer==dwColorTopLeft)
			{
				*pBuffer=dwColorBackGroundTransparent;
				m_pSpriteBank->GetPositions().push_back(Core::Rect(vPos,vPos));
			}
			else if(*pBuffer==dwColorLowerRight)
			{
				if(m_pSpriteBank->GetPositions().size() <= (uint32)iLowerRight)
				{
					iLowerRight=0;
					return;
				}

				*pBuffer=dwColorBackGroundTransparent;
				m_pSpriteBank->GetPositions()[iLowerRight].m_vLowerRight=vPos;

				SpriteFrame f;
				f.wRectNum=iLowerRight;
				f.wTextureNum=0;

				Sprite s;
				s.Frames.push_back(f);
				s.dwFrameTime=0;
				m_pSpriteBank->GetSprites().push_back(s);

				FontArea a;
				a.iOverhang=0;
				a.iUnderhang=0;
				a.iSpriteNo=iLowerRight;
				a.iWidth=m_pSpriteBank->GetPositions()[iLowerRight].GetWidth();
				m_Areas.push_back(a);

				char ch=(char)(iLowerRight + 32);
				m_CharacterMap.Insert(ch,iLowerRight);

				iLowerRight++;
			}
			else if(*pBuffer==dwColorBackGround)
				*pBuffer=dwColorBackGroundTransparent;

			pBuffer++;
		}
	}
}

Core::Vector2i Font::GetDimension(const char *szText) const
{
	Core::Vector2i vDim(0,0);
	Core::Vector2i vThisLine(0,m_iMaxHeight);

	const char *p;
	for(p=szText;*p;p++)
	{
		bool bLineBreak=false;
		if(*p=='\n')
		{
			vDim.y+=vThisLine.y;
			if(vDim.x < vThisLine.x)
				vDim.x=vThisLine.x;

			vThisLine.x=0;
			continue;
		}

		const FontArea &area=m_Areas[GetAreaFromCharacter(*p)];
		vThisLine.x+=area.iUnderhang;
		vThisLine.x+=area.iWidth + area.iOverhang + m_iGlobalKerningWidth;
	}

	vDim.y+=vThisLine.y;
	if(vDim.x < vThisLine.x)
		vDim.x=vThisLine.x;

	return vDim;
}

void Font::SetKerningWidth(int iKerning)
{
	m_iGlobalKerningWidth=iKerning;
}

int Font::GetKerningWidth(const char *szThisLetter, const char *szPreviousLetter) const
{
	int ret=m_iGlobalKerningWidth;

	if(szThisLetter)
	{
		ret+=m_Areas[GetAreaFromCharacter(*szThisLetter)].iOverhang;
		if(szPreviousLetter)
			ret+=m_Areas[GetAreaFromCharacter(*szPreviousLetter)].iUnderhang;
	}

	return ret;
}

void Font::SetKerningHeight(int iKerning)
{
	m_iGlobalKerningHeight=iKerning;
}

int Font::GetKerningHeight() const
{
	return m_iGlobalKerningHeight;
}

int Font::GetSpriteNoFromChar(const char *c) const
{
	return m_Areas[GetAreaFromCharacter(*c)].iSpriteNo;
}

int Font::GetAreaFromCharacter(const char c) const
{
	Core::TMap<char,int>::Node *n=m_CharacterMap.Find(c);
	if(n)
		return n->GetValue();
	else
		return (int)m_dwWrongCharacter;
}

/*
//! draws an text and clips it to the specified rectangle if wanted
void CGUIFont::draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::dimension2d<s32> textDimension;
	core::position2d<s32> offset = position.UpperLeftCorner;

	if (hcenter || vcenter)
	{
		textDimension = getDimension(text);

		if (hcenter)
			offset.X = ((position.getWidth() - textDimension.Width)>>1) + offset.X;

		if (vcenter)
			offset.Y = ((position.getHeight() - textDimension.Height)>>1) + offset.Y;
	}

	core::array<s32> indices;
	indices.reallocate(core::stringw(text).size());
	u32 n;
	while(*text)
	{
		n = (*text) - 32;
		if ( n > Positions.size())
			n = WrongCharacter;
		indices.push_back(n);
		++text;
	}
	Driver->draw2DImage(Texture, offset, Positions, indices, GlobalKerningWidth, clip, color, true);
}
*/

void Font::Draw(const char *szText, const Core::Rect &position,
				Core::Color32 cColor, bool hcenter, bool vcenter,
				const Core::Rect *pClip)
{
	if(!szText || m_Areas.size()==0)
		return;

	Core::Vector2i vTextDimension;
	Core::Vector2i vOffset(position.m_vUpperLeft);
	Core::Rect pos;

	if(hcenter || vcenter || pClip)
		vTextDimension=GetDimension(szText);

	if(hcenter) vOffset.x=((position.GetWidth() - vTextDimension.x) >> 1) + vOffset.x;
	if(vcenter) vOffset.y=((position.GetHeight()- vTextDimension.y) >> 1) + vOffset.y;

	if(pClip)
	{
		Core::Rect ClippedRect(vOffset,vTextDimension);
		ClippedRect.ClipAgainst(*pClip);
		if(!ClippedRect.IsValid())
			return;
	}

	while(*szText)
	{
		FontArea& area=m_Areas[GetAreaFromCharacter(*szText)];
		vOffset.x+=area.iUnderhang;

		m_pSpriteBank->DrawSprite(area.iSpriteNo,vOffset,pClip,cColor);
		vOffset.x+=area.iWidth + area.iOverhang + m_iGlobalKerningWidth;

		szText++;
	}
}

int Font::GetCharacterFromPos(const char *szText, int px) const
{
	int x=0,idx=0;

	while(szText[idx])
	{
		const FontArea& a=m_Areas[GetAreaFromCharacter(szText[idx])];
		x+=a.iWidth + a.iOverhang + a.iUnderhang;

		if(x >= px)
			return idx;

		idx++;
	}

	return -1;
}

ISpriteBank *Font::GetSpriteBank() const
{
	return m_pSpriteBank;
}

}