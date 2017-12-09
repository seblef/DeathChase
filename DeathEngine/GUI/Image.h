
#ifndef _IMAGE_H_

#define _IMAGE_H_

#include "IImage.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class Image : public IImage
{
	public:

		Image(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual ~Image();

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual void SetImage(Core::HTexture hTex);
		virtual void SetColor(Core::Color32 cColor);

		virtual void SetImageScale(bool bScale);
		virtual bool IsImageScaled() const;

		virtual void SetUseAlphaChannel(bool bUse);
		virtual bool IsAlphaChannelUsed() const;

		virtual void Draw();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Image")
		MEMDEBUG_CLASSSIZE(sizeof(Image))

	private:

		Core::Color32 m_cColor;
		Core::HTexture m_hTexture;
		bool m_bUseAlphaChannel;
		bool m_bScaleImage;
};

}

#endif
