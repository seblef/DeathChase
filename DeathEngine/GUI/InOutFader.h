
#ifndef _INOUTFADER_H_

#define _INOUTFADER_H_

#include "IInOutFader.h"

namespace GUI
{

class InOutFader : public IInOutFader
{
	public:

		InOutFader(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect);
		virtual void Clear();

		virtual void Draw();

		virtual Core::Color32 GetColor() const;

		virtual void SetColor(Core::Color32 cColor);
		virtual void SetColor(Core::Color32 cSource, Core::Color32 cDest);

		virtual void FadeIn(uint32 dwTime);
		virtual void FadeOut(uint32 dwTime);

		virtual bool IsReady() const;

	private:

		enum FadeAction
		{
			FA_NOTHING=0,
			FA_FADEIN,
			FA_FADEOUT
		};

		uint32 m_dwStartTime;
		uint32 m_dwEndTime;
		FadeAction m_Action;

		Core::Color32 m_cColor[2];
		Core::Color32 m_cFull;
		Core::Color32 m_cTrans;
};

}

#endif
