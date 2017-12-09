
#ifndef _SKIN_H_

#define _SKIN_H_

#include "../Common.h"
#include "ISkin.h"
#include <string>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class Skin : public ISkin
{
	public:

		Skin(SKINTYPE _Type);
		virtual ~Skin();

		virtual Core::Color32 GetColor(DEFAULTCOLOR dc) const;
		virtual void SetColor(DEFAULTCOLOR dc, Core::Color32 c);

		virtual int GetSize(DEFAULTSIZE ds) const;
		virtual void SetSize(DEFAULTSIZE ds, int s);

		virtual IFont *GetFont(DEFAULTFONT df=DF_DEFAULT) const;
		virtual void SetFont(IFont *f, DEFAULTFONT df);

		virtual ISpriteBank *GetSpriteBank() const;
		virtual void SetSpriteBank(ISpriteBank *sb);

		virtual uint32 GetIcon(DEFAULTICON di) const;
		virtual void SetIcon(DEFAULTICON di, uint32 i);

		virtual const char *GetDefaultText(DEFAULTTEXT dt) const;
		virtual void SetDefaultText(DEFAULTTEXT dt, const char *t);

		virtual void Draw3DButtonPaneStandard(IElement *element,
			const Core::Rect& rect,	const Core::Rect *clip=0);
		virtual void Draw3DButtonPanePressed(IElement *element,
			const Core::Rect& rect, const Core::Rect *clip=0);
		virtual void Draw3DSunkenPane(IElement *element,
			Core::Color32 bgcolor, bool flat, bool fillbackground,
			const Core::Rect& rect, const Core::Rect *clip=0);
		virtual Core::Rect Draw3DWindowBackground(IElement *element,
			bool drawtitlebar, Core::Color32 titlebarcolor,
			Core::Rect rect, bool bMinimized, const Core::Rect *clip=0);
		virtual void Draw3DMenuPane(IElement *element,
			const Core::Rect& rect, const Core::Rect *clip=0);
		virtual void Draw3DToolBar(IElement *element,
			const Core::Rect& rect, const Core::Rect *clip=0);
		virtual void Draw3DTabButton(IElement *element, bool active,
			const Core::Rect& rect, const Core::Rect *clip=0);
		virtual void Draw3DTabBody(IElement *element, bool border,
			bool background, const Core::Rect& rect, const Core::Rect *clip=0);
		virtual void DrawIcon(IElement *element, DEFAULTICON icon,
			const Core::Vector2i& pos, uint32 starttime=0, uint32 currenttime=0,
			bool loop=false, const Core::Rect *clip=0);
		virtual void Draw2DRectangle(IElement *element, const Core::Color32& color,
			const Core::Rect& rect, const Core::Rect *clip=0);

		virtual SKINTYPE GetType() const;

		virtual bool Load(const char *szName);

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Skin")
		MEMDEBUG_CLASSSIZE(sizeof(Skin))

	private:

		Core::Color32 m_cColors[DC_COUNT];
		int m_iSizes[DS_COUNT];
		uint32 m_dwIcons[DI_COUNT];
		IFont *m_pFonts[DF_COUNT];
		ISpriteBank *m_pBank;
		std::string m_strTexts[DT_COUNT];
		bool m_bUseGradient;

		SKINTYPE m_Type;
};

}

#endif
