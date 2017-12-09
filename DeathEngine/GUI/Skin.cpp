
#include "Skin.h"
#include "IFont.h"
#include "ISpriteBank.h"
#include "../Core/Renderer2D.h"
#include "../Core/ScriptFile.h"

namespace GUI
{

Skin::Skin(SKINTYPE _Type) : m_pBank(0), m_Type(_Type)
{
	if(m_Type==ST_WINDOWSCLASSIC || m_Type==ST_WINDOWSMETALLIC)
	{
		m_cColors[DC_3DDARKSHADOW]   = Core::Color32(200,50,50,50);
		m_cColors[DC_3DSHADOW]        = Core::Color32(200,130,130,130);
		m_cColors[DC_3DFACE]          = Core::Color32(200,210,210,210);
		m_cColors[DC_3DHIGHLIGHT]    = Core::Color32(200,255,255,255);
		m_cColors[DC_3DLIGHT]         =	Core::Color32(200,210,210,210);
		m_cColors[DC_ACTIVEBORDER]    = Core::Color32(200,16,14,115);
		m_cColors[DC_ACTIVECAPTION]   = Core::Color32(200,255,255,255);
		m_cColors[DC_APPWORKSPACE]    = Core::Color32(200,100,100,100);
		m_cColors[DC_BUTTONTEXT]      = Core::Color32(240,10,10,10);
		m_cColors[DC_GRAYTEXT]        = Core::Color32(240,130,130,130);
		m_cColors[DC_HIGHLIGHT]       = Core::Color32(200,8,36,107);
		m_cColors[DC_HIGHLIGHTTEXT]  = Core::Color32(240,255,255,255);
		m_cColors[DC_INACTIVEBORDER]  = Core::Color32(200,165,165,165);
		m_cColors[DC_INACTIVECAPTION] = Core::Color32(200,210,210,210);
		m_cColors[DC_TOOLTIP]          = Core::Color32(200,0,0,0);
		m_cColors[DC_TOOLTIPBACKGROUND]= Core::Color32(200,255,255,225);
		m_cColors[DC_SCROLLBAR]        = Core::Color32(200,230,230,230);
		m_cColors[DC_WINDOW]           = Core::Color32(200,255,255,255);
		m_cColors[DC_WINDOWSYMBOL]    = Core::Color32(200,10,10,10);
		m_cColors[GUI::DC_ICON_]             = Core::Color32(200,255,255,255);
		m_cColors[DC_ICONHIGHLIGHT]  = Core::Color32(200,8,36,107);

		m_iSizes[DS_SCROLLBARSIZE] = 14;
		m_iSizes[DS_MENUHEIGHT] = 30;
		m_iSizes[DS_WINDOWBUTTONWIDTH] = 15;
		m_iSizes[DS_CHECKBOXWIDTH] = 18;
		m_iSizes[DS_MESSAGEBOXWIDTH] = 500;
		m_iSizes[DS_MESSAGEBOXHEIGHT] = 200;
		m_iSizes[DS_BUTTONWIDTH] = 80;
		m_iSizes[DS_BUTTONHEIGHT] = 20;
	
		m_iSizes[DS_TEXTDISTANCEX] = 2;
		m_iSizes[DS_TEXTDISTANCEY] = 0;
	}
	else
	{
		m_cColors[DC_3DDARKSHADOW] =	0x60767982;
		m_cColors[DC_3DFACE]		=	0xc0cbd2d9;		// tab background
		m_cColors[DC_3DSHADOW]		=	0x50e4e8f1;		// tab background, and left-top highlight
		m_cColors[DC_3DHIGHLIGHT]	=	0x40c7ccdc;	
		m_cColors[DC_3DLIGHT]		=	0x802e313a;
		m_cColors[DC_ACTIVEBORDER]	=	0x80404040;		// window title
		m_cColors[DC_ACTIVECAPTION] =	0xf0d0d0d0;
		m_cColors[DC_APPWORKSPACE]	=	0xc0646464;		// unused
		m_cColors[DC_BUTTONTEXT]	=	0xd0161616;
		m_cColors[DC_GRAYTEXT]		=	0x3c141414;
		m_cColors[DC_HIGHLIGHT]		=	0x6c606060;
		m_cColors[DC_HIGHLIGHTTEXT]=	0xd0e0e0e0;
		m_cColors[DC_INACTIVEBORDER]=	0xf0a5a5a5;
		m_cColors[DC_INACTIVECAPTION]=	0xf0d2d2d2;
		m_cColors[DC_TOOLTIP]		=	0xf00f2033;
		m_cColors[DC_TOOLTIPBACKGROUND]=0xc0cbd2d9;
		m_cColors[DC_SCROLLBAR]		=	0xf0e0e0e0;
		m_cColors[DC_WINDOW]			=	0xf0f0f0f0;
		m_cColors[DC_WINDOWSYMBOL]	=	0xd0161616;
		m_cColors[GUI::DC_ICON_]			=	0xd0161616;
		m_cColors[DC_ICONHIGHLIGHT]=	0xd0606060;

		m_iSizes[DS_SCROLLBARSIZE] = 14;
		m_iSizes[DS_MENUHEIGHT] = 48;
		m_iSizes[DS_WINDOWBUTTONWIDTH] = 15;
		m_iSizes[DS_CHECKBOXWIDTH] = 18;
		m_iSizes[DS_MESSAGEBOXWIDTH] = 500;
		m_iSizes[DS_MESSAGEBOXHEIGHT] = 200;
		m_iSizes[DS_BUTTONWIDTH] = 80;
		m_iSizes[DS_BUTTONHEIGHT] = 20;

		m_iSizes[DS_TEXTDISTANCEX] = 3;
		m_iSizes[DS_TEXTDISTANCEY] = 2;
	}

	m_strTexts[DT_MSGBOXOK] = "OK";
	m_strTexts[DT_MSGBOXCANCEL] = "Annuler";
	m_strTexts[DT_MSGBOXYES] = "Oui";
	m_strTexts[DT_MSGBOXNO] = "Non";
	m_strTexts[DT_WINDOWCLOSE] = "Fermer";
	m_strTexts[DT_WINDOWRESTORE] = "Restaurer";
	m_strTexts[DT_WINDOWMINIMIZE] = "Minimiser";
	m_strTexts[DT_WINDOWMAXIMIZE] = "Maximiser";

	Load("standard");

	m_dwIcons[DI_WINDOWMAXIMIZE] = 225;
	m_dwIcons[DI_WINDOWRESTORE] = 226;
	m_dwIcons[DI_WINDOWCLOSE] = 227;
	m_dwIcons[DI_WINDOWMINIMIZE] = 228;
	m_dwIcons[DI_CURSORUP] = 229;
	m_dwIcons[DI_CURSORDOWN] = 230;
	m_dwIcons[DI_CURSORLEFT] = 231;
	m_dwIcons[DI_CURSORRIGHT] = 232;
	m_dwIcons[DI_MENUMORE] = 232;
	m_dwIcons[DI_CHECKBOXCHECKED] = 233;
	m_dwIcons[DI_DROPDOWN] = 234;
	m_dwIcons[DI_SMALLCURSORUP] = 235;
	m_dwIcons[DI_SMALLCURSORDOWN] = 236;
	m_dwIcons[DI_RADIOBUTTONCHECKED] = 237;
	m_dwIcons[DI_MORELEFT] = 238;
	m_dwIcons[DI_MORERIGHT] = 239;
	m_dwIcons[DI_MOREUP] = 240;
	m_dwIcons[DI_MOREDOWN] = 241;
	m_dwIcons[DI_WINDOWRESIZE] = 242;
	m_dwIcons[DI_EXPAND] = 243;
	m_dwIcons[DI_COLLAPSE] = 244;

	m_dwIcons[DI_FILE] = 245;
	m_dwIcons[DI_DIRECTORY] = 246;

	for (uint32 i=0; i<DF_COUNT; ++i)
		m_pFonts[i] = 0;

	m_bUseGradient = m_Type == ST_WINDOWSMETALLIC;
}

Skin::~Skin()
{
	uint32 i;
	for(i=0;i<DF_COUNT;i++)
		if(m_pFonts[i])
			m_pFonts[i]->Drop();

	if(m_pBank)
		m_pBank->Drop();
}

Core::Color32 Skin::GetColor(DEFAULTCOLOR color) const
{
	if ((uint32)color < DC_COUNT)
		return m_cColors[color];
	else
		return Core::Color32();
}

void Skin::SetColor(DEFAULTCOLOR which, Core::Color32 newColor)
{
	if ((uint32)which < DC_COUNT)
		m_cColors[which] = newColor;
}

int Skin::GetSize(DEFAULTSIZE size) const
{
	if ((uint32)size < DS_COUNT)
		return m_iSizes[size];
	else
		return 0;
}

void Skin::SetSize(DEFAULTSIZE which, int size)
{
	if ((uint32)which < DS_COUNT)
		m_iSizes[which] = size;
}

IFont* Skin::GetFont(DEFAULTFONT which) const
{
	if (((uint32)which < DF_COUNT) && m_pFonts[which])
		return m_pFonts[which];
	else
		return m_pFonts[DF_DEFAULT];
}

void Skin::SetFont(IFont* font,DEFAULTFONT which)
{
	if ((uint32)which >= DS_COUNT)
		return;

	if (m_pFonts[which])
		m_pFonts[which]->Drop();

	m_pFonts[which] = font;

	if (m_pFonts[which])
		m_pFonts[which]->Grab();
}

ISpriteBank* Skin::GetSpriteBank() const
{
	return m_pBank;
}

void Skin::SetSpriteBank(ISpriteBank* bank)
{
	if (m_pBank)
		m_pBank->Drop();

	if (bank)
		bank->Grab();

	m_pBank = bank;
}

uint32 Skin::GetIcon(DEFAULTICON icon) const
{
	if ((uint32)icon < DI_COUNT)
		return m_dwIcons[icon];
	else
		return 0;
}

void Skin::SetIcon(DEFAULTICON icon, uint32 index)
{
	if ((uint32)icon < DI_COUNT)
		m_dwIcons[icon] = index;
}

const char* Skin::GetDefaultText(DEFAULTTEXT text) const
{
	if ((uint32)text < DT_COUNT)
		return m_strTexts[text].c_str();
	else
		return m_strTexts[0].c_str();
}

void Skin::SetDefaultText(DEFAULTTEXT which, const char* newText)
{
	if ((uint32)which < DT_COUNT)
		m_strTexts[which] = newText;
}

void Skin::Draw3DButtonPaneStandard(IElement* element,
					const Core::Rect& r,
					const Core::Rect* clip)
{
	Core::Rect rect = r;

	g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW),rect,clip);

	rect.m_vLowerRight.x -= 1;
	rect.m_vLowerRight.y -= 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, clip);

	rect.m_vUpperLeft.x += 1;
	rect.m_vUpperLeft.y += 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

	rect.m_vLowerRight.x -= 1;
	rect.m_vLowerRight.y -= 1;

	if (!m_bUseGradient)
	{
		g_Renderer2D.DrawRectangle(GetColor(DC_3DFACE), rect, clip);
	}
	else
	{
		const Core::Color32 c1(GetColor(DC_3DFACE));
		const Core::Color32 c2(c1.GetInterpolated(GetColor(DC_3DDARKSHADOW), 0.4f));
		g_Renderer2D.DrawRectangle(rect, c1, c1, c2, c2, clip);
	}
}

void Skin::Draw3DButtonPanePressed(IElement* element,
					const Core::Rect& r,
					const Core::Rect* clip)
{
	Core::Rect rect = r;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, clip);

	rect.m_vLowerRight.x -= 1;
	rect.m_vLowerRight.y -= 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW), rect, clip);

	rect.m_vUpperLeft.x += 1;
	rect.m_vUpperLeft.y += 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

	rect.m_vUpperLeft.x += 1;
	rect.m_vUpperLeft.y += 1;

	if (!m_bUseGradient)
	{
		g_Renderer2D.DrawRectangle(GetColor(DC_3DFACE), rect, clip);
	}
	else
	{
		const Core::Color32 c1(GetColor(DC_3DFACE));
		const Core::Color32 c2(c1.GetInterpolated(GetColor(DC_3DDARKSHADOW), 0.4f));
		g_Renderer2D.DrawRectangle(rect, c1, c1, c2, c2, clip);
	}
}

void Skin::Draw3DSunkenPane(IElement* element, Core::Color32 bgcolor,
				bool flat, bool fillBackGround,
				const Core::Rect& r,
				const Core::Rect* clip)
{
	Core::Rect rect(r);

	if (flat)
	{
		// draw flat sunken pane
		if (fillBackGround)
			g_Renderer2D.DrawRectangle(bgcolor, rect, clip);

		rect.m_vLowerRight.y = rect.m_vUpperLeft.y + 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

		rect.m_vLowerRight.y = r.m_vLowerRight.y;
		rect.m_vLowerRight.x = rect.m_vUpperLeft.x + 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

		rect = r;
		rect.m_vUpperLeft.x = rect.m_vLowerRight.x - 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, clip);

		rect = r;
		rect.m_vUpperLeft.y = r.m_vLowerRight.y - 1;
		rect.m_vLowerRight.y = r.m_vLowerRight.y;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, clip);
	}
	else
	{
		// draw deep sunken pane
		if (fillBackGround)
			g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, clip);

		rect.m_vLowerRight.x -= 1;
		rect.m_vLowerRight.y -= 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

		rect.m_vUpperLeft.x += 1;
		rect.m_vUpperLeft.y += 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DLIGHT), rect, clip);

		rect.m_vLowerRight.x -= 1;
		rect.m_vLowerRight.y -= 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW), rect, clip);

		rect.m_vUpperLeft.x += 1;
		rect.m_vUpperLeft.y += 1;
		g_Renderer2D.DrawRectangle(bgcolor, rect, clip);
	}
}

Core::Rect Skin::Draw3DWindowBackground(IElement* element,
				bool drawTitleBar, Core::Color32 titleBarColor,
				Core::Rect r, bool bMinimized,
				const Core::Rect* cl)
{

	if(bMinimized)
		r.m_vLowerRight.y = r.m_vUpperLeft.y + GetSize(DS_WINDOWBUTTONWIDTH) + 6;

	Core::Rect rect(r);

	rect.m_vLowerRight.y = rect.m_vUpperLeft.y + 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, cl);

	rect.m_vLowerRight.y = r.m_vLowerRight.y;
	rect.m_vLowerRight.x = rect.m_vUpperLeft.x + 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, cl);

	rect.m_vUpperLeft.x = r.m_vLowerRight.x - 1;
	rect.m_vLowerRight.x = r.m_vLowerRight.x;
	rect.m_vUpperLeft.y = r.m_vUpperLeft.y;
	rect.m_vLowerRight.y = r.m_vLowerRight.y;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW), rect, cl);

	rect.m_vUpperLeft.x -= 1;
	rect.m_vLowerRight.x -= 1;
	rect.m_vUpperLeft.y += 1;
	rect.m_vLowerRight.y -= 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, cl);

	rect.m_vUpperLeft.x = r.m_vUpperLeft.x;
	rect.m_vUpperLeft.y = r.m_vLowerRight.y - 1;
	rect.m_vLowerRight.y = r.m_vLowerRight.y;
	rect.m_vLowerRight.x = r.m_vLowerRight.x;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW), rect, cl);

	rect.m_vUpperLeft.x += 1;
	rect.m_vLowerRight.x -= 1;
	rect.m_vUpperLeft.y -= 1;
	rect.m_vLowerRight.y -= 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, cl);

	rect = r;
	rect.m_vUpperLeft.x +=1;
	rect.m_vUpperLeft.y +=1;
	rect.m_vLowerRight.x -= 2;
	rect.m_vLowerRight.y -= 2;

	if (!m_bUseGradient)
	{
		g_Renderer2D.DrawRectangle(GetColor(DC_3DFACE), rect, cl);
	}
	else
	{
		const Core::Color32 c2(GetColor(DC_3DSHADOW));
		const Core::Color32 c1(GetColor(DC_3DFACE));
		g_Renderer2D.DrawRectangle(rect, c1, c1, c1, c2, cl);
	}

	rect = r;
	rect.m_vUpperLeft.x += 2;
	rect.m_vUpperLeft.y += 2;
	rect.m_vLowerRight.x -= 2;
	rect.m_vLowerRight.y = rect.m_vUpperLeft.y + GetSize(DS_WINDOWBUTTONWIDTH) + 2;

	if (drawTitleBar)
	{
		const Core::Color32 c(titleBarColor.GetInterpolated(Core::Color32(255,0,0,0), 0.2f));
		g_Renderer2D.DrawRectangle(rect, titleBarColor, c, titleBarColor, c, cl);
	}

	return rect;
}

void Skin::Draw3DMenuPane(IElement* element,
			const Core::Rect& r, const Core::Rect* clip)
{
	Core::Rect rect(r);

	rect.m_vLowerRight.y = rect.m_vUpperLeft.y + 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, clip);

	rect.m_vLowerRight.y = r.m_vLowerRight.y;
	rect.m_vLowerRight.x = rect.m_vUpperLeft.x + 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), rect, clip);

	rect.m_vUpperLeft.x = r.m_vLowerRight.x - 1;
	rect.m_vLowerRight.x = r.m_vLowerRight.x;
	rect.m_vUpperLeft.y = r.m_vUpperLeft.y;
	rect.m_vLowerRight.y = r.m_vLowerRight.y;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW), rect, clip);

	rect.m_vUpperLeft.x -= 1;
	rect.m_vLowerRight.x -= 1;
	rect.m_vUpperLeft.y += 1;
	rect.m_vLowerRight.y -= 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

	rect.m_vUpperLeft.x = r.m_vUpperLeft.x;
	rect.m_vUpperLeft.y = r.m_vLowerRight.y - 1;
	rect.m_vLowerRight.y = r.m_vLowerRight.y;
	rect.m_vLowerRight.x = r.m_vLowerRight.x;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW), rect, clip);

	rect.m_vUpperLeft.x += 1;
	rect.m_vLowerRight.x -= 1;
	rect.m_vUpperLeft.y -= 1;
	rect.m_vLowerRight.y -= 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

	rect = r;
	rect.m_vUpperLeft.x +=1;
	rect.m_vUpperLeft.y +=1;
	rect.m_vLowerRight.x -= 2;
	rect.m_vLowerRight.y -= 2;

	if (!m_bUseGradient)
		g_Renderer2D.DrawRectangle(GetColor(DC_3DFACE), rect, clip);
	else
	{
		const Core::Color32 c1(GetColor(DC_3DFACE));
		const Core::Color32 c2(GetColor(DC_3DSHADOW));
		g_Renderer2D.DrawRectangle(rect, c1, c1, c2, c2, clip);
	}
}

void Skin::Draw3DToolBar(IElement* element,
				const Core::Rect& r,
				const Core::Rect* clip)
{
	Core::Rect rect(r);

	rect.m_vUpperLeft.x = r.m_vUpperLeft.x;
	rect.m_vUpperLeft.y = r.m_vLowerRight.y - 1;
	rect.m_vLowerRight.y = r.m_vLowerRight.y;
	rect.m_vLowerRight.x = r.m_vLowerRight.x;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), rect, clip);

	rect = r;
	rect.m_vLowerRight.y -= 1;

	if (!m_bUseGradient)
	{
		g_Renderer2D.DrawRectangle(GetColor(DC_3DFACE), rect, clip);
	}
	else
	{
		const Core::Color32 c1(GetColor(DC_3DFACE));
		const Core::Color32 c2(GetColor(DC_3DSHADOW));
		g_Renderer2D.DrawRectangle(rect, c1, c1, c2, c2, clip);
	}
}

void Skin::Draw3DTabButton(IElement* element, bool active,
				const Core::Rect& frameRect,
				const Core::Rect* clip)
{
	Core::Rect tr(frameRect);

	tr.m_vLowerRight.x -= 2;
	tr.m_vLowerRight.y = tr.m_vUpperLeft.y + 1;
	tr.m_vUpperLeft.x += 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), tr, clip);

	tr = frameRect;
	tr.m_vLowerRight.x = tr.m_vUpperLeft.x + 1;
	tr.m_vUpperLeft.y += 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), tr, clip);

	tr = frameRect;
	tr.m_vUpperLeft.x += 1;
	tr.m_vUpperLeft.y += 1;
	tr.m_vLowerRight.x -= 2;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DFACE), tr, clip);

	tr.m_vLowerRight.x += 1;
	tr.m_vUpperLeft.x = tr.m_vLowerRight.x - 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), tr, clip);

	tr.m_vLowerRight.x += 1;
	tr.m_vUpperLeft.x += 1;
	tr.m_vUpperLeft.y += 1;
	g_Renderer2D.DrawRectangle(GetColor(DC_3DDARKSHADOW), tr, clip);
}

void Skin::Draw3DTabBody(IElement* element, bool border, bool background,
	const Core::Rect& rect, const Core::Rect* clip)
{
	Core::Rect tr(rect);

	if (border)
	{
		tr.m_vUpperLeft.y += GetSize(DS_BUTTONHEIGHT) + 2;
		tr.m_vLowerRight.x = tr.m_vUpperLeft.x + 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DHIGHLIGHT), tr, clip);

		tr.m_vUpperLeft.x = rect.m_vLowerRight.x - 1;
		tr.m_vLowerRight.x = tr.m_vUpperLeft.x + 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), tr, clip);

		tr = rect;
		tr.m_vUpperLeft.y = tr.m_vLowerRight.y - 1;
		g_Renderer2D.DrawRectangle(GetColor(DC_3DSHADOW), tr, clip);
	}

	if (background)
	{
		tr = rect;
		tr.m_vUpperLeft.y += GetSize(DS_BUTTONHEIGHT) + 2;
		tr.m_vLowerRight.x -= 1;
		tr.m_vUpperLeft.x += 1;
		tr.m_vLowerRight.y -= 1;

		if (!m_bUseGradient)
			g_Renderer2D.DrawRectangle(GetColor(DC_3DFACE), tr, clip);
		else
		{
			const Core::Color32 c1(GetColor(DC_3DFACE));
			const Core::Color32 c2(GetColor(DC_3DSHADOW));
			g_Renderer2D.DrawRectangle(tr, c1, c1, c2, c2, clip);
		}
	}
}

void Skin::DrawIcon(IElement* element, DEFAULTICON icon,
					const Core::Vector2i& position,
			uint32 starttime, uint32 currenttime, 
			bool loop, const Core::Rect* clip)
{
	if (!m_pBank)
		return;

	m_pBank->DrawSprite(m_dwIcons[icon], position, clip, 
			Core::Color32(255,0,0,0), starttime, currenttime, loop, true);
}

SKINTYPE Skin::GetType() const
{
	return m_Type;
}

void Skin::Draw2DRectangle(IElement* element,
						   const Core::Color32 &color, const Core::Rect& pos,
		const Core::Rect* clip)
{
	g_Renderer2D.DrawRectangle(color, pos, clip);
}

bool Skin::Load(const char *szName)
{
	MYASSERT(szName,"Mauvais nom de skin",true);

	char szTemp[1024]="GUI\\Templates\\";
	strcat(szTemp,szName);
	strcat(szTemp,".skin");

	Core::ScriptFile file;
	if(!file.Open(szTemp))
	{
		g_Output->AddMsg("x Impossible d'ouvrir le fichier %s\n",szTemp);
		return false;
	}

	const char *szToken=file.GetToken();
	while(szToken)
	{
		bool found=false;

		int i;
		for(i=0;i<DC_COUNT;i++)
		{
			if(strcmp(SkinColorNames[i],szToken)==0)
			{
				Core::Color32 color;
				if(file.ParseColor(color))
					m_cColors[i]=color;
				else
				{
					g_Output->AddMsg("x Fin de fichier inattendue dans la skin %s\n",szName);
					return false;
				}

				found=true;
			}
		}

		if(!found)
		{
			for(i=0;i<DS_COUNT;i++)
			{
				if(strcmp(SkinSizeNames[i],szToken)==0)
				{
					szToken=file.GetToken();
					if(szToken)
						m_iSizes[i]=atoi(szToken);
					else
					{
						g_Output->AddMsg("x Fin de fichier inattendue dans la skin %s\n",szName);
						return false;
					}

					found=true;
				}
			}
		}

		if(!found)
		{
			for(i=0;i<DT_COUNT;i++)
			{
				if(strcmp(SkinTextNames[i],szToken)==0)
				{
					szToken=file.GetToken();
					if(szToken)
						m_strTexts[i]=szToken;
					else
					{
						g_Output->AddMsg("x Fin de fichier inattendue dans la skin %s\n",szName);
						return false;
					}

					found=true;
				}
			}
		}
		
		if(!found)
		{
			g_Output->AddMsg("x Token %s inconnu dans la skin %s\n",szToken,szName);
			return false;
		}

		szToken=file.GetToken();
	}

	file.Close();
	return true;
}

}
