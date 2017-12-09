
#ifndef _IBUTTON_H_

#define _IBUTTON_H_

#include "IElement.h"
#include "../Core/Handles.h"
#include "../Core/Color.h"

namespace GUI
{

class IFont;
class ISpriteBank;

enum BUTTONSTATE
{
	BS_UP=0,
	BS_DOWN,
	BS_MOUSEOVER,
	BS_MOUSEOFF,
	BS_FOCUSED,
	BS_NOTFOCUSED,

	BS_COUNT

};

const char * const ButtonStateNames[] =
{
	"buttonUp",
	"buttonDown",
	"buttonMouseOver",
	"buttonMouseOff",
	"buttonFocused",
	"buttonNotFocused",
	0,
	0,
};

class IButton : public IElement
{
	public:

		IButton(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_BUTTON,pEnvironment,pParent,pUser,dwID,rect) {};
	  virtual ~IButton() {};

	  virtual void SetOverrideFont(IFont *pFont=0)=0;
	  
	  virtual void SetImage(Core::HTexture hTex)=0;
	  virtual void SetImage(Core::HTexture hTex, const Core::Rect& pos)=0;
	  virtual void SetPressedImage(Core::HTexture hTex)=0;
	  virtual void SetPressedImage(Core::HTexture hTex, const Core::Rect& pos)=0;

	  virtual void SetSpriteBank(ISpriteBank *pBank)=0;
	  virtual void SetSprite(BUTTONSTATE _State, int iIndex,
		  Core::Color32 cColor=Core::Color32(255,255,255,255), bool bLoop=false)=0;

	  virtual void SetIsPushButton(bool bPush)=0;
	  virtual bool IsPushButton() const=0;

	  virtual void SetPressed(bool bPressed)=0;
	  virtual bool IsPressed() const=0;

	  virtual void SetUseAlphaChannel(bool bUseAlpha)=0;
	  virtual bool IsAlphaChannelUsed() const=0;

	  virtual void SetDrawBorder(bool bBorder)=0;
	  virtual bool IsDrawingBorder() const=0;
};

}

#endif

