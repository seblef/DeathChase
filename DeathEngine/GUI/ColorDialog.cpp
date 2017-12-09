
#include "ColorDialog.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IButton.h"
#include "IStaticText.h"
#include "IFont.h"
#include "ISpriteBank.h"
#include "../Core/TextureMgr.h"

namespace GUI
{

const int CSD_WIDTH=350;
const int CSD_HEIGHT=300;

struct Template
{
	const char *pre;
	const char *init;
	const char *pos;
	int x,y;
	int rangedown,rangeup;
};

static const Template Templates[]=
{
	{ "A:", "0", 0,20,165, 0, 255 },
	{ "R:", "0", 0,20,205, 0, 255 },
	{ "G:", "0", 0,20,230, 0, 255 },
	{ "B:", "0", 0,20,255, 0, 255 },
	{ "H:", "0", "°",180,205, 0, 360 },
	{ "S:", "0", "%",180,230, 0, 100 },
	{ "L:", "0", "%",180,255, 0, 100 },
};

ColorDialog::ColorDialog(const char *szTitle, IEnvironment *pEnvironment,
						 IElement *pParent,  IEventReceiver *pUser, uint32 dwID) :
	IColorDialog(pEnvironment,pParent,pUser,dwID,Core::Rect((pParent->GetAbsolutePosition().GetWidth() - CSD_WIDTH) / 2,
													  (pParent->GetAbsolutePosition().GetHeight() - CSD_HEIGHT) / 2,
													  (pParent->GetAbsolutePosition().GetWidth()-CSD_WIDTH) / 2 + CSD_WIDTH,
													  (pParent->GetAbsolutePosition().GetHeight()-CSD_HEIGHT) / 2 + CSD_HEIGHT)),
		m_bDragging(false)
{
	m_strText=szTitle;

	ISkin *skin=m_pEnvironment->GetSkin();
	Core::Rect rec;

	int buttonw=skin->GetSize(DS_WINDOWBUTTONWIDTH);
	int posx=m_RelativeRect.GetWidth() - buttonw - 4;

	m_pClose=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw), this, 0,
		INVALID_ID,"",skin ? skin->GetDefaultText(DT_WINDOWCLOSE) : "Close");
	if(skin && skin->GetSpriteBank())
	{
		m_pClose->SetSpriteBank(skin->GetSpriteBank());
		m_pClose->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWCLOSE),skin->GetColor(DC_WINDOWSYMBOL));
		m_pClose->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWCLOSE),skin->GetColor(DC_WINDOWSYMBOL));
	}
	m_pClose->SetSubElement(true);
	m_pClose->SetTabStop(false);
	m_pClose->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pOK=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth() - 80,30,
											   m_RelativeRect.GetWidth() - 10,50),
	   this, 0, INVALID_ID, skin ? skin->GetDefaultText(DT_MSGBOXOK) : "OK");
	m_pOK->SetSubElement(true);
	m_pOK->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pCancel=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth()-80,55,
												   m_RelativeRect.GetWidth()-10,75),
	   this,0,INVALID_ID, skin ? skin->GetDefaultText(DT_MSGBOXCANCEL) : "Annuler");
	m_pCancel->SetSubElement(true);
	m_pCancel->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	Core::Rect r;
	m_ColorRing.hTexture=g_TextureMgr.GetTexture("#colorring");
	if(m_ColorRing.hTexture.IsNull())
		BuildColorRing(Core::Vector2i(128,128),1,m_pEnvironment->GetSkin()->GetColor(DC_3DSHADOW).dwRGB);

	r.m_vUpperLeft.x=20;
	r.m_vUpperLeft.y=20;
	m_ColorRing.pControl=m_pEnvironment->AddImage(m_ColorRing.hTexture,r.m_vUpperLeft,true,this);
	m_ColorRing.pControl->SetSubElement(true);

	uint32 i;
	for(i=0;i!=sizeof(Templates) / sizeof(Template);i++)
	{
		if(Templates[i].pre)
		{
			r.m_vUpperLeft.x=Templates[i].x;
			r.m_vUpperLeft.y=Templates[i].y;
			r.m_vLowerRight.x=r.m_vUpperLeft.x + 15;
			r.m_vLowerRight.y=r.m_vUpperLeft.y + 20;
			IElement *t=m_pEnvironment->AddStaticText(Templates[i].pre,r,false,false,this);
			t->SetSubElement(true);
		}

		if(Templates[i].pos)
		{
			r.m_vUpperLeft.x=Templates[i].x + 52;
			r.m_vUpperLeft.y=Templates[i].y;
			r.m_vLowerRight.x=r.m_vUpperLeft.x + 15;
			r.m_vLowerRight.y=r.m_vUpperLeft.y + 20;
			IElement *t=m_pEnvironment->AddStaticText(Templates[i].pos,r,false,false,this);
			t->SetSubElement(true);
		}

		BatteryItem item;
		r.m_vUpperLeft.x=Templates[i].x + 15;
		r.m_vUpperLeft.y=Templates[i].y;
		r.m_vLowerRight.x=r.m_vUpperLeft.x + 35;
		r.m_vLowerRight.y=r.m_vUpperLeft.y + 20;

		item.pEdit=m_pEnvironment->AddEditBox(Templates[i].init,r,true,this);
		item.pEdit->SetSubElement(true);

		r.m_vUpperLeft.x=Templates[i].x + 70;
		r.m_vUpperLeft.y=Templates[i].y + 4;
		r.m_vLowerRight.x=r.m_vUpperLeft.x + 60;
		r.m_vLowerRight.y=r.m_vUpperLeft.y + 12;

		item.pScroll=m_pEnvironment->AddScrollBar(true,r,this);
		item.pScroll->SetSubElement(true);
		item.pScroll->SetMax(Templates[i].rangeup - Templates[i].rangedown);
		item.pScroll->SetSmallStep(1);

		m_Battery.push_back(item);
	}

	BringToFront(m_pCancel);
	BringToFront(m_pOK);
}

ColorDialog::~ColorDialog()
{
	RemoveChild(m_pClose,true);
	RemoveChild(m_pOK,true);
	RemoveChild(m_pCancel,true);

	uint32 i;
	for(i=0;i<m_Battery.size();i++)
	{
		RemoveChild(m_Battery[i].pEdit,true);
		RemoveChild(m_Battery[i].pScroll,true);
	}

	RemoveChild(m_ColorRing.pControl,true);
}

void ColorDialog::Reset(const char *szTitle, IEnvironment *pEnvironment,
						 IElement *pParent, IEventReceiver *pUser, uint32 dwID)
{
	IElement::Reset(pEnvironment,pParent,pUser,dwID,Core::Rect((pParent->GetAbsolutePosition().GetWidth() - CSD_WIDTH) / 2,
													  (pParent->GetAbsolutePosition().GetHeight() - CSD_HEIGHT) / 2,
													  (pParent->GetAbsolutePosition().GetWidth()-CSD_WIDTH) / 2 + CSD_WIDTH,
													  (pParent->GetAbsolutePosition().GetHeight()-CSD_HEIGHT) / 2 + CSD_HEIGHT));
	m_bDragging=false;

	m_strText=szTitle;

	ISkin *skin=m_pEnvironment->GetSkin();
	Core::Rect rec;

	int buttonw=skin->GetSize(DS_WINDOWBUTTONWIDTH);
	int posx=m_RelativeRect.GetWidth() - buttonw - 4;

	m_pClose=m_pEnvironment->AddButton(Core::Rect(posx,3,posx+buttonw,3+buttonw), this,0,
		INVALID_ID,"",skin ? skin->GetDefaultText(DT_WINDOWCLOSE) : "Close");
	if(skin && skin->GetSpriteBank())
	{
		m_pClose->SetSpriteBank(skin->GetSpriteBank());
		m_pClose->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWCLOSE),skin->GetColor(DC_WINDOWSYMBOL));
		m_pClose->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWCLOSE),skin->GetColor(DC_WINDOWSYMBOL));
	}
	m_pClose->SetSubElement(true);
	m_pClose->SetTabStop(false);
	m_pClose->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pOK=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth() - 80,30,
											   m_RelativeRect.GetWidth() - 10,50),
	   this,0, INVALID_ID, skin ? skin->GetDefaultText(DT_MSGBOXOK) : "OK");
	m_pOK->SetSubElement(true);
	m_pOK->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pCancel=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth()-80,55,
												   m_RelativeRect.GetWidth()-10,75),
	   this,0,INVALID_ID, skin ? skin->GetDefaultText(DT_MSGBOXCANCEL) : "Annuler");
	m_pCancel->SetSubElement(true);
	m_pCancel->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	Core::Rect r;
	m_ColorRing.hTexture=g_TextureMgr.GetTexture("#colorring");
	if(m_ColorRing.hTexture.IsNull())
		BuildColorRing(Core::Vector2i(128,128),1,m_pEnvironment->GetSkin()->GetColor(DC_3DSHADOW).dwRGB);

	r.m_vUpperLeft.x=20;
	r.m_vUpperLeft.y=20;
	m_ColorRing.pControl=m_pEnvironment->AddImage(m_ColorRing.hTexture,r.m_vUpperLeft,true,this);
	m_ColorRing.pControl->SetSubElement(true);

	uint32 i;
	for(i=0;i!=sizeof(Templates) / sizeof(Template);i++)
	{
		if(Templates[i].pre)
		{
			r.m_vUpperLeft.x=Templates[i].x;
			r.m_vUpperLeft.y=Templates[i].y;
			r.m_vLowerRight.x=r.m_vUpperLeft.x + 15;
			r.m_vLowerRight.y=r.m_vUpperLeft.y + 20;
			IElement *t=m_pEnvironment->AddStaticText(Templates[i].pre,r,false,false,this);
			t->SetSubElement(true);
		}

		if(Templates[i].pos)
		{
			r.m_vUpperLeft.x=Templates[i].x + 52;
			r.m_vUpperLeft.y=Templates[i].y;
			r.m_vLowerRight.x=r.m_vUpperLeft.x + 15;
			r.m_vLowerRight.y=r.m_vUpperLeft.y + 20;
			IElement *t=m_pEnvironment->AddStaticText(Templates[i].pos,r,false,false,this);
			t->SetSubElement(true);
		}

		BatteryItem item;
		r.m_vUpperLeft.x=Templates[i].x + 15;
		r.m_vUpperLeft.y=Templates[i].y;
		r.m_vLowerRight.x=r.m_vUpperLeft.x + 35;
		r.m_vLowerRight.y=r.m_vUpperLeft.y + 20;

		item.pEdit=m_pEnvironment->AddEditBox(Templates[i].init,r,true,this);
		item.pEdit->SetSubElement(true);

		r.m_vUpperLeft.x=Templates[i].x + 70;
		r.m_vUpperLeft.y=Templates[i].y + 4;
		r.m_vLowerRight.x=r.m_vUpperLeft.x + 60;
		r.m_vLowerRight.y=r.m_vUpperLeft.y + 12;

		item.pScroll=m_pEnvironment->AddScrollBar(true,r,this);
		item.pScroll->SetSubElement(true);
		item.pScroll->SetMax(Templates[i].rangeup - Templates[i].rangedown);
		item.pScroll->SetSmallStep(1);

		m_Battery.push_back(item);
	}

	BringToFront(m_pCancel);
	BringToFront(m_pOK);
}

void ColorDialog::Clear()
{
	RemoveChild(m_pClose,true);
	RemoveChild(m_pOK,true);
	RemoveChild(m_pCancel,true);

	m_pClose=0;
	m_pOK=0;
	m_pCancel=0;

	uint32 i;
	for(i=0;i<m_Battery.size();i++)
	{
		RemoveChild(m_Battery[i].pEdit,true);
		RemoveChild(m_Battery[i].pScroll,true);
	}
	m_Battery.clear();

	RemoveChild(m_ColorRing.pControl,true);

	IElement::Clear();
}

void ColorDialog::BuildColorRing(const Core::Vector2i &dim, int supersample,
								 const uint32 bordercolor)
{
	Core::Vector2i d(dim.x * supersample,dim.y * supersample);

	Core::HTexture hRaw=g_TextureMgr.CreateEmptyTexture(d.x,d.y,D3DFMT_A8R8G8B8,"#colorring");
	if(hRaw.IsNull())
		return;

	IDirect3DTexture9 *pTex=g_TextureMgr.GetTexture(hRaw);
	if(!pTex)
	{
		g_TextureMgr.ReleaseTexture(hRaw);
		return;
	}

	D3DLOCKED_RECT rect;
	if(pTex->LockRect(0,&rect,0,D3DLOCK_DISCARD)!=D3D_OK)
	{
		g_TextureMgr.ReleaseTexture(hRaw);
		return;
	}

	memset32(rect.pBits,0x00808080,d.x*d.y*4);

	byte *data=(byte *)rect.pBits;
	uint32 pitch=rect.Pitch;

	int radiusout=(d.x / 2) - 4;
	const int fullr2=radiusout * radiusout;

	Core::ColorHSL hsl;
	Core::Color32 rgb;

	rgb.dwRGB=0xff000000;
	hsl.luminance=0.5f;
	hsl.saturation=1.0f;

	Core::Vector2i p;
	uint32 *dst;
	for(p.y=-radiusout;p.y<=radiusout;p.y++)
	{
		int y2=p.y*p.y;

		dst=(uint32 *)((byte *)data + ((4 + p.y * radiusout) * pitch) + (4 << 2));

		for(p.x=-radiusout;p.x<=radiusout;p.x++,dst++)
		{
			int r2=y2 + (p.x * p.x);
			int testa=r2-fullr2;

			if(testa < 0)
			{
				float r=(float)sqrtf((float)r2);
				float t=1.0f / r;
				float xn=(float)-p.x * t;

				hsl.hue=acosf(xn);
				if(p.y > 0)
					hsl.hue=(2.0f * PI)-hsl.hue;

				hsl.hue-=PI / 2.0f;
				float rtest=r / (float)radiusout;

				if(rtest >= 0.5f)
				{
					hsl.luminance=0.5f;
					hsl.saturation=1.0f;
					hsl.ToRGB(rgb);
					*dst=rgb.dwRGB;
				}

				if(rtest >= 0.5f && rtest <= 0.55f)
				{
					uint32 alpha=(int)((rtest - 0.5f) * (255.0f / 0.05f));
					*dst=*dst & 0x00ffffff | (alpha << 24);
				}

				if(rtest >= 0.95f)
				{
					uint32 alpha=(int)((rtest - 0.95f) * (255.0f / 0.05f));
					alpha=255 - alpha;
					*dst=*dst & 0x00ffffff | (alpha << 24);
				}
			}
		}
	}

	pTex->UnlockRect(0);
}

bool ColorDialog::OnEvent(const Event &_Event)
{
	switch(_Event.EventType)
	{
		case ET_GUI:
			switch(_Event.sGUI.EventType)
			{
				case GE_SCROLLBARCHANGED:
					{
						uint32 i;
						for(i=0;i<m_Battery.size();i++)
						{
							if(_Event.sGUI.pCaller==m_Battery[i].pScroll)
							{
								int pos=m_Battery[i].pScroll->GetPos();
								int value=Templates[i].rangedown + pos;
								std::string s;
								s+=value;
								m_Battery[i].pEdit->SetText(s.c_str());
							}
						}
						return true;
					}

				case GE_FOCUSLOST:
					m_bDragging=false;
					break;

				case GE_BUTTONCLICKED:
					if(_Event.sGUI.pCaller==m_pClose ||
					   _Event.sGUI.pCaller==m_pCancel)
					{
						SendCancelEvent();
						Remove();
						return true;
					}
					else if(_Event.sGUI.pCaller==m_pOK)
					{
						SendSelectedEvent();
						Remove();
						return true;
					}
					break;

				default:
					break;
			}
			break;

		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_LMOUSEDOWN:
					m_vDragStart.x=(int)_Event.sMouse.dwX;
					m_vDragStart.y=(int)_Event.sMouse.dwY;
					m_bDragging=true;
					m_pEnvironment->SetFocus(this);
					return true;

				case ME_LMOUSEUP:
					m_bDragging=false;
					m_pEnvironment->RemoveFocus(this);
					return true;

				case ME_MOVED:
					if(m_bDragging)
					{
						if(m_pParent)
							if((int)_Event.sMouse.dwX < m_pParent->GetAbsolutePosition().m_vUpperLeft.x + 1 ||
							   (int)_Event.sMouse.dwY < m_pParent->GetAbsolutePosition().m_vUpperLeft.y + 1 ||
							   (int)_Event.sMouse.dwX > m_pParent->GetAbsolutePosition().m_vLowerRight.x - 1 ||
							   (int)_Event.sMouse.dwY > m_pParent->GetAbsolutePosition().m_vLowerRight.y - 1)
							   return true;

						Move(Core::Vector2i((int)_Event.sMouse.dwX - m_vDragStart.x,(int)_Event.sMouse.dwY - m_vDragStart.y));
						m_vDragStart.x=(int)_Event.sMouse.dwX;
						m_vDragStart.y=(int)_Event.sMouse.dwY;
						return true;
					}

				default:
					break;
			}

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void ColorDialog::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();

	Core::Rect rect(m_AbsoluteRect);
	rect=skin->Draw3DWindowBackground(this,true,skin->GetColor(DC_ACTIVEBORDER),rect,&m_AbsoluteClippingRect);

	if(m_strText.size())
	{
		rect.m_vUpperLeft.x+=2;
		rect.m_vLowerRight.x-=skin->GetSize(DS_WINDOWBUTTONWIDTH) + 5;

		IFont *font=skin->GetFont(DF_WINDOW);
		if(font)
			font->Draw(m_strText.c_str(),rect,skin->GetColor(DC_ACTIVECAPTION),false,true,&m_AbsoluteClippingRect);
	}

	IElement::Draw();
}

void ColorDialog::SendSelectedEvent()
{
	Event e;
	e.EventType=ET_GUI;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;
	e.sGUI.EventType=GE_FILESELECTED;
	SendEvent(e);
}

void ColorDialog::SendCancelEvent()
{
	Event e;
	e.EventType=ET_GUI;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;
	e.sGUI.EventType=GE_FILECANCELLED;
	SendEvent(e);
}

}