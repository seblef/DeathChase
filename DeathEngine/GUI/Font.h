
#ifndef _FONT_H_

#define _FONT_H_

#include "IFont.h"
#include "../Core/BMPImage.h"
#include "../Core/TMap.h"
#include <vector>
#include "../Dev/MemDebugger.h"

namespace GUI
{

class IEnvironment;
class ISpriteBank;

class Font : public IFont
{
	public:

		Font(IEnvironment *pEnvironment, const char *szFileName);
		virtual ~Font();

		bool Load(const char *szFileName);

		virtual void Draw(const char *szText, const Core::Rect& pos,
			Core::Color32 cColor, bool hcenter=false, bool vcenter=false,
			const Core::Rect *pClip=0);

		virtual Core::Vector2i GetDimension(const char *szText) const;
		virtual int GetCharacterFromPos(const char *szText, int px) const;

		virtual FONTTYPE GetType() const { return FT_BITMAP; };

		virtual void SetKerningWidth(int iKerning);
		virtual void SetKerningHeight(int iKerning);

		virtual int GetKerningWidth(const char *szThisLetter=0, const char *szPreviousLetter=0) const;
		virtual int GetKerningHeight() const;

		virtual ISpriteBank *GetSpriteBank() const;
		virtual int GetSpriteNoFromChar(const char *c) const;

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Font")
		MEMDEBUG_CLASSSIZE(sizeof(Font))

	private:
		
		struct FontArea
		{
			FontArea(): iUnderhang(0), iOverhang(0), iWidth(0), iSpriteNo(0) {}

			int iUnderhang;
			int iOverhang;
			int iWidth;
			int iSpriteNo;

		};

		bool LoadTexture(Core::BMPImage *pImage, const char *szName);

		void ReadPosition32bit(uint32 *pBuffer, int iWidth, int iHeight, int& iLowerRight);

		int GetAreaFromCharacter(const char c) const;
		void SetMaxHeight();

		std::vector<FontArea> m_Areas;
		Core::TMap<char, int> m_CharacterMap;

		ISpriteBank *m_pSpriteBank;
		IEnvironment *m_pEnvironment;

		uint32 m_dwWrongCharacter;
		int m_iMaxHeight;
		int m_iGlobalKerningWidth, m_iGlobalKerningHeight;

};

}

#endif