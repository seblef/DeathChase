
#ifndef _ISKIN_H_

#define _ISKIN_H_

#include "../Core/Color.h"
#include "../Core/Rect.h"

namespace GUI
{

class IFont;
class ISpriteBank;
class IElement;

enum SKINTYPE
{
	ST_WINDOWSCLASSIC,
	ST_WINDOWSMETALLIC,
	ST_UNKNOWN

};

const char * const SkinTypeNames[]=
{
	"windowsClassic",
	"windowsMetallic",
	"burning",
	"unknown",
	0,
};

enum DEFAULTCOLOR
{
	DC_3DDARKSHADOW=0,
	DC_3DSHADOW,
	DC_3DFACE,
	DC_3DHIGHLIGHT,
	DC_3DLIGHT,
	DC_ACTIVEBORDER,
	DC_ACTIVECAPTION,
	DC_APPWORKSPACE,
	DC_BUTTONTEXT,
	DC_GRAYTEXT,
	DC_HIGHLIGHT,
	DC_HIGHLIGHTTEXT,
	DC_INACTIVEBORDER,
	DC_INACTIVECAPTION,
	DC_TOOLTIP,
	DC_TOOLTIPBACKGROUND,
	DC_SCROLLBAR,
	DC_WINDOW,
	DC_WINDOWSYMBOL,
	DC_ICON_,
	DC_ICONHIGHLIGHT,

	DC_COUNT
};

const char* const SkinColorNames[]=
{
	"3DDarkShadow",
	"3DShadow",
	"3DFace",
	"3DHighlight",
	"3DLight",
	"ActiveBorder",
	"ActiveCaption",
	"AppWorkspace",
	"ButtonText",
	"GrayText",
	"Highlight",
	"HighlightText",
	"InactiveBorder",
	"InactiveCaption",
	"ToolTip",
	"ToolTipBackground",
	"ScrollBar",
	"Window",
	"WindowSymbol",
	"Icon",
	"IconHighlight",
	0,
};

enum DEFAULTSIZE
{
	DS_SCROLLBARSIZE=0,
	DS_MENUHEIGHT,
	DS_WINDOWBUTTONWIDTH,
	DS_CHECKBOXWIDTH,
	DS_MESSAGEBOXWIDTH,
	DS_MESSAGEBOXHEIGHT,
	DS_BUTTONWIDTH,
	DS_BUTTONHEIGHT,
	DS_TEXTDISTANCEX,
	DS_TEXTDISTANCEY,

	DS_COUNT
};

const char* const SkinSizeNames[]=
{
	"ScrollBarSize",
	"MenuHeight",
	"WindowButtonWidth",
	"CheckBoxWidth",
	"MessageBoxWidth",
	"MessageBoxHeight",
	"ButtonWidth",
	"ButtonHeight",
	"TextDistanceX",
	"TextDistanceY",
	0,
};

enum DEFAULTTEXT
{
	DT_MSGBOXOK=0,
	DT_MSGBOXCANCEL,
	DT_MSGBOXYES,
	DT_MSGBOXNO,
	DT_WINDOWCLOSE,
	DT_WINDOWMAXIMIZE,
	DT_WINDOWMINIMIZE,
	DT_WINDOWRESTORE,

	DT_COUNT
};

const char* const SkinTextNames[]=
{
	"MessageBoxOkay",
	"MessageBoxCancel",
	"MessageBoxYes",
	"MessageBoxNo",
	"WindowButtonClose",
	"WindowButtonMaximize",
	"WindowButtonMinimize",
	"WindowButtonRestore",
	0,
};

enum DEFAULTICON
{
	DI_WINDOWMAXIMIZE=0,
	DI_WINDOWRESTORE,
	DI_WINDOWCLOSE,
	DI_WINDOWMINIMIZE,
	DI_WINDOWRESIZE,
	DI_CURSORUP,
	DI_CURSORDOWN,
	DI_CURSORLEFT,
	DI_CURSORRIGHT,
	DI_MENUMORE,
	DI_CHECKBOXCHECKED,
	DI_DROPDOWN,
	DI_SMALLCURSORUP,
	DI_SMALLCURSORDOWN,
	DI_RADIOBUTTONCHECKED,
	DI_MORELEFT,
	DI_MORERIGHT,
	DI_MOREUP,
	DI_MOREDOWN,
	DI_EXPAND,
	DI_COLLAPSE,
	DI_FILE,
	DI_DIRECTORY,

	DI_COUNT
};

const char* const SkinIconNames[]=
{
	"windowMaximize",
	"windowRestore",
	"windowClose",
	"windowMinimize",
	"windowResize", 
	"cursorUp",
	"cursorDown",
	"cursorLeft",
	"cursorRight",
	"menuMore",
	"checkBoxChecked",
	"dropDown",
	"smallCursorUp",
	"smallCursorDown",
	"radioButtonChecked",
	"moreLeft",
	"moreRight",
	"moreUp",
	"moreDown",
	"expand",
	"collapse",
	"file",
	"directory",
	0
};

enum DEFAULTFONT
{
	DF_DEFAULT=0,
	DF_BUTTON,
	DF_WINDOW,
	DF_MENU,
	DF_TOOLTIP,
	
	DF_COUNT

};

const char* const SkinFontNames[]=
{
	"defaultFont",
	"buttonFont",
	"windowFont",
	"menuFont",
	"tooltipFont",
	0
};

class ISkin
{
	public:

		virtual ~ISkin() {};

		virtual Core::Color32 GetColor(DEFAULTCOLOR _Color) const=0;
		virtual void SetColor(DEFAULTCOLOR _Color, Core::Color32 cColor)=0;

		virtual int GetSize(DEFAULTSIZE size) const=0;
		virtual void SetSize(DEFAULTSIZE size, int iSize)=0;

		virtual const char* GetDefaultText(DEFAULTTEXT text) const=0;
		virtual void SetDefaultText(DEFAULTTEXT text, const char *szText)=0;

		virtual IFont *GetFont(DEFAULTFONT font=DF_DEFAULT) const=0;
		virtual void SetFont(IFont *pFont, DEFAULTFONT font=DF_DEFAULT)=0;

		virtual ISpriteBank *GetSpriteBank() const=0;
		virtual void SetSpriteBank(ISpriteBank *pBank)=0;

		virtual uint32 GetIcon(DEFAULTICON icon) const=0;
		virtual void SetIcon(DEFAULTICON icon, uint32 iIndex)=0;

		virtual void Draw3DButtonPaneStandard(IElement *pElement,
			const Core::Rect& rect,
			const Core::Rect *pClip=0)=0;
		virtual void Draw3DButtonPanePressed(IElement *pElement,
			const Core::Rect& rect,
			const Core::Rect *pClip=0)=0;

		virtual void Draw3DSunkenPane(IElement *pElement,
			Core::Color32 bgColor, bool bFlat, bool bFillBackGround,
			const Core::Rect& rect,
			const Core::Rect *pClip=0)=0;

		virtual Core::Rect Draw3DWindowBackground(IElement *pElement,
			bool bDrawTitleBar, Core::Color32 cTitleBarColor,
			Core::Rect rect, bool bMinimized,
			const Core::Rect *pClip=0)=0;

		virtual void Draw3DMenuPane(IElement *pElement,
			const Core::Rect& rect,
			const Core::Rect *pClip=0)=0;

		virtual void Draw3DToolBar(IElement *pElement,
			const Core::Rect& rect,
			const Core::Rect *pClip=0)=0;

		virtual void Draw3DTabButton(IElement *pElement, bool bActive,
			const Core::Rect& rect,
			const Core::Rect *pClip=0)=0;

		virtual void Draw3DTabBody(IElement *pElement,
			bool bBorder, bool bBackground,
			const Core::Rect& rect,
			const Core::Rect *pClip=0)=0;

		virtual void DrawIcon(IElement *pElement, DEFAULTICON icon,
			const Core::Vector2i& vPos,
			uint32 dwStartTime=0, uint32 dwCurrentTime=0,
			bool bLoop=false, const Core::Rect *pClip=0)=0;

		virtual void Draw2DRectangle(IElement *pElement,
			const Core::Color32& cColor,
			const Core::Rect& pos,
			const Core::Rect *pClip=0)=0;

		virtual SKINTYPE GetType() const { return ST_UNKNOWN; };
		virtual bool Load(const char *szName)=0;
};

}

#endif