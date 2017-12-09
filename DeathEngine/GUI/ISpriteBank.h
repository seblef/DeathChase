
#ifndef _ISPRITEBANK_H_

#define _ISPRITEBANK_H_

#include "../Core/IReferenceCounter.h"
#include "../Core/Rect.h"
#include "../Core/Handles.h"
#include "../Core/Color.h"
#include <vector>

namespace GUI
{

struct SpriteFrame
{
	uint16 wTextureNum;
	uint16 wRectNum;

};

struct Sprite
{
	Sprite() : Frames(), dwFrameTime(0) {};
	std::vector<SpriteFrame> Frames;
	uint32 dwFrameTime;

};

class ISpriteBank : public Core::IReferenceCounter
{
	public:

		virtual ~ISpriteBank() {};

		virtual std::vector<Core::Rect>& GetPositions()=0;
		virtual std::vector<Sprite>& GetSprites()=0;

		virtual uint32 GetTextureCount() const=0;

		virtual Core::HTexture GetTexture(const uint32 dwIndex) const=0;
		virtual void AddTexture(Core::HTexture hTex)=0;
		virtual void SetTexture(uint32 dwIndex, Core::HTexture hTex)=0;

		virtual void DrawSprite(uint32 dwIndex, const Core::Vector2i& vPos,
			const Core::Rect *pClip=0, const Core::Color32& cColor=Core::Color32(255,255,255,255),
			uint32 dwStartTime=0, uint32 dwCurrentTime=0, bool bLoop=true, bool bCenter=false)=0;

};

}

#endif