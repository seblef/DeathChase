
#ifndef _IIMAGE_H_

#define _IIMAGE_H_

#include "IElement.h"
#include "../Core/Handles.h"
#include "../Core/Color.h"

namespace GUI
{

class IImage  : public IElement
{
	public:

		IImage(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_IMAGE,pEnvironment,pParent,pUser,dwID,rect) {};
		virtual ~IImage() {};

		  virtual void SetImage(Core::HTexture hTex)=0;
		  virtual void SetColor(Core::Color32 cColor)=0;

		  virtual void SetImageScale(bool bScale)=0;
		  virtual bool IsImageScaled() const=0;

		  virtual void SetUseAlphaChannel(bool bUse)=0;
		  virtual bool IsAlphaChannelUsed() const=0;
};

}

#endif