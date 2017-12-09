
#ifndef _IFONT_H_

#define _IFONT_H_

#include "../Core/IReferenceCounter.h"
#include "../Common.h"
#include "../Core/Color.h"
#include "../Core/Rect.h"

namespace GUI
{

enum FONTTYPE
{
	FT_BITMAP=0,
	FT_VECTOR,
	FT_OS,
	FT_CUSTOM

};

class IFont : public virtual Core::IReferenceCounter
{
	public:

		virtual ~IFont() {};

		virtual void Draw(const char *szText, const Core::Rect& pos,
			Core::Color32 cColor, bool hcenter=false, bool vcenter=false,
			const Core::Rect *pClip=0)=0;
		virtual Core::Vector2i GetDimension(const char *szText) const=0;

		virtual int GetCharacterFromPos(const char *szText, int px) const=0;

		virtual FONTTYPE GetType() const { return FT_CUSTOM; };

		virtual void SetKerningWidth(int iKerning)=0;
		virtual void SetKerningHeight(int iKerning)=0;

		virtual int GetKerningWidth(const char *szThisLetter=0, const char *szPreviousLetter=0) const=0;
		virtual int GetKerningHeight() const=0;
};

}

#endif
