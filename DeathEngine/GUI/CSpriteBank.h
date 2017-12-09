
#ifndef _SPRITEBANK_H_

#define _SPRITEBANK_H_

#include "ISpriteBank.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class IEnvironment;

class SpriteBank : public ISpriteBank
{
	public:

		SpriteBank(IEnvironment *pEnvironment);
		virtual ~SpriteBank();

		virtual std::vector<Core::Rect>& GetPositions();
		virtual std::vector<Sprite>& GetSprites();

		virtual uint32 GetTextureCount() const;
		virtual Core::HTexture GetTexture(const uint32 dwIndex) const;
		virtual void AddTexture(Core::HTexture hTex);
		virtual void SetTexture(uint32 dwIndex, Core::HTexture hTex);

		virtual void DrawSprite(uint32 dwIndex, const Core::Vector2i& vPos, const Core::Rect *pClip=0,
			const Core::Color32& cColor=Core::Color32(255,255,255,255), uint32 dwStartTime=0,
			uint32 dwCurrentTime=0, bool bLoop=true, bool bCenter=false);

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("SpriteBank")
		MEMDEBUG_CLASSSIZE(sizeof(SpriteBank))

	protected:

		std::vector<Sprite> m_Sprites;
		std::vector<Core::Rect> m_Rects;
		std::vector<Core::HTexture> m_Textures;

		IEnvironment *m_pEnvironment;

};

}

#endif