
#ifndef _BUTTON_H_

#define _BUTTON_H_

#include "IEnvironment.h"
#include "IButton.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class Button : public IButton
{
	public:

		Button(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect, bool bNoClip=false);
	  virtual ~Button();

	  virtual void Reset(IEnvironment *pEnvironment, IElement *pParent,
		  IEventReceiver *pUser, uint32 dwID, Core::Rect rect);
	  virtual void Clear();

	  virtual bool OnEvent(const Event& _Event);
	  virtual void Draw();

	  virtual void SetOverrideFont(IFont *pFont=0);
	  
	  virtual void SetImage(Core::HTexture hTex);
	  virtual void SetImage(Core::HTexture hTex, const Core::Rect& pos);
	  virtual void SetPressedImage(Core::HTexture hTex);
	  virtual void SetPressedImage(Core::HTexture hTex, const Core::Rect& pos);

	  virtual void SetSpriteBank(ISpriteBank *pBank);
	  virtual void SetSprite(BUTTONSTATE _State, int iIndex,
		  Core::Color32 cColor=Core::Color32(255,255,255,255), bool bLoop=false);

	  virtual void SetIsPushButton(bool bPush);
	  virtual bool IsPushButton() const;

	  virtual void SetPressed(bool bPressed);
	  virtual bool IsPressed() const;

	  virtual void SetUseAlphaChannel(bool bUseAlpha);
	  virtual bool IsAlphaChannelUsed() const;

	  virtual void SetDrawBorder(bool bBorder);
	  virtual bool IsDrawingBorder() const;

	  MEMDEBUG_IMPL
	  MEMDEBUG_CLASSNAME("Button")
	  MEMDEBUG_CLASSSIZE(sizeof(Button))

	private:

		struct ButtonSprite
		{
			int iIndex;
			Core::Color32 cColor;
			bool bLoop;
		};

		bool m_bPressed;
		bool m_bIsPushButton;
		bool m_bUseAlphaChannel;
		
		bool m_bBorder;

		uint32 m_dwMouseOverTime;
		uint32 m_dwFocusTime;
		uint32 m_dwClickTime;

		ISpriteBank* m_pSpriteBank;
		IFont* m_pOverrideFont;

		ButtonSprite m_ButtonSprites[BS_COUNT];

		Core::HTexture m_hTexture, m_hPressedTexture;
		Core::Rect m_ImageRect,m_PressedImageRect;
};

}

#endif