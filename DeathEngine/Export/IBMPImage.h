
#ifndef _IBMPIMAGE_H_
#define _IBMPIMAGE_H_

#include "../Common.h"
#include "IExportObj.h"

class IBMPImage : public IExportObject
{
	public:

		IBMPImage() : IExportObject(EOBJ_BMPIMAGE) {}
		virtual ~IBMPImage() {}

		virtual bool Load(const char *szName)=0;
		virtual bool Create(const int iWidth, const int iHeight, const int iBPP)=0;
		virtual bool Save(const char *szName)=0;

		virtual void MirrorY()=0;

		virtual byte *GetPalette()=0;
		virtual byte *GetBuffer()=0;
		virtual int GetWidth() const=0;
		virtual int GetHeight() const=0;
		virtual int GetBPP() const=0;
};

#endif
