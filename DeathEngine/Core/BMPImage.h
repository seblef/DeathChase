
#ifndef _BMPIMAGE_H_

#define _BMPIMAGE_H_

#ifdef DEEXPORT
#include "../Export/IBMPImage.h"
#else
#include "../Common.h"
#define IBMPImage		Core::BMPImage
#endif

namespace Core
{

#ifdef DEEXPORT
class BMPImage : public IBMPImage
#else
class BMPImage
#endif
{
	public:

		BMPImage();
		~BMPImage();

		bool Load(const char *szName);
		bool Create(const int iWidth, const int iHeight, const int iBPP);
		bool Save(const char *szName);
		void Clear();

		void MirrorY();

		byte *GetPalette() { return m_pPalette; };
		byte *GetBuffer() { return m_pBuffer; };
		int GetWidth() const { return m_iWidth; };
		int GetHeight() const { return m_iHeight; };
		int GetBPP() const { return m_iBPP; };

	protected:

		byte *m_pPalette;
		byte *m_pBuffer;

		int m_iWidth,m_iHeight;
		int m_iBPP;

};

}

#endif