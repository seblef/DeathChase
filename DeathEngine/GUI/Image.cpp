
#include "Image.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "../Core/TextureMgr.h"
#include "../Core/Renderer2D.h"

namespace GUI
{

Image::Image(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			 uint32 dwID, Core::Rect rect) :
	IImage(pEnvironment,pParent,pUser,dwID,rect), m_cColor(255,255,255,255),
		m_bUseAlphaChannel(false),m_bScaleImage(false)
{}

Image::~Image()
{
}

void Image::Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			 uint32 dwID, Core::Rect rect)
{
	IElement::Reset(pEnvironment,pParent,pUser, dwID,rect);
}

void Image::Clear()
{
	m_bUseAlphaChannel=false;
	m_cColor=Core::Color32(255,255,255,255);
	m_bScaleImage=false;

	IElement::Clear();
}

void Image::SetImage(Core::HTexture hTex)
{
	m_hTexture=hTex;
}

void Image::SetColor(Core::Color32 cColor)
{
	m_cColor=cColor;
}

void Image::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();

	Core::Rect rect(m_AbsoluteRect);

	if(!m_hTexture.IsNull())
	{
		int x,y;
		g_TextureMgr.GetTextureSize(m_hTexture,x,y);

		g_Renderer2D.Draw2DImage(m_hTexture,m_AbsoluteRect.m_vUpperLeft,
//			Core::Rect(Core::Vector2i::NullVector,m_AbsoluteRect.GetSize()),
			Core::Rect(Core::Vector2i::NullVector,Core::Vector2i(x,y)),
			&m_AbsoluteClippingRect,m_cColor,m_bUseAlphaChannel);
	}
	else
		skin->Draw2DRectangle(this,Core::Color32(255,0,0,0),m_AbsoluteRect,&m_AbsoluteClippingRect);

	IElement::Draw();
}

void Image::SetUseAlphaChannel(bool bUse)
{
	m_bUseAlphaChannel=bUse;
}

void Image::SetImageScale(bool bScale)
{
	m_bScaleImage=bScale;
}

bool Image::IsImageScaled() const
{
	return m_bScaleImage;
}

bool Image::IsAlphaChannelUsed() const
{
	return m_bUseAlphaChannel;
}

}
