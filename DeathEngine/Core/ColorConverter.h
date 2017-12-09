
#ifndef _COLORCONVERTER_H_

#define _COLORCONVERTER_H_

#include "../Common.h"

namespace Core
{

class ColorConverter
{
	public:

		static uint16 RGBA16(uint32 r, uint32 g, uint32 b, uint32 a)
		{
			return ((a & 0x80) << 8 | (r & 0xF8) << 7 | (g & 0xF8) << 2 | (b & 0xF8) >> 3);
		}

		static uint16 RGB16(uint32 r, uint32 g, uint32 b)
		{
			return RGBA16(r,g,b,0xFF);
		}

		static uint16 RGB16From16(uint16 r, uint16 g, uint16 b)
		{
			return (r & 0x1F) << 10 | (g & 0x1F) << 5 | (b & 0x1F);
		}

		static uint16 X8R8G8B8ToA1R5G5B5(uint32 dwColor)
		{
			return (0x8000 | (dwColor & 0x00F80000) >> 9 | (dwColor & 0x0000F800) >> 6 | (dwColor & 0x000000F8) >> 3);
		}

		static uint16 A8R8G8B8ToA1R5G5B5(uint32 dwColor)
		{
			return ((dwColor & 0x80000000) >> 16 | (dwColor & 0x00F80000) >> 9 | (dwColor & 0x0000F800) >> 6 | (dwColor & 0x000000F8) >> 3);
		}

		static uint16 A8R8G8B8ToR5G6B5(uint32 dwColor)
		{
			return ((dwColor & 0x00F80000) >> 8 | (dwColor & 0x0000FC00) >> 5 | (dwColor & 0x000000F8) >> 3);
		}

		static uint32 A1R5G5B5ToA8R8G8B8(uint16 wColor)
		{
			return ( (( -( (int)wColor & 0x00008000) >> (int)31) & 0xFF000000) |
				   (( wColor & 0x00007C00) << 9) | (( wColor & 0x00007000) << 4) |
				   (( wColor & 0x000003E0) << 6) | (( wColor & 0x00000380) << 1) |
				   (( wColor & 0x0000001F) << 3) | (( wColor & 0x0000001C) >> 2));
		}

		static uint32 R5G6B5ToA8R8G8B8(uint16 wColor)
		{
			return 0xFF000000 | ((wColor & 0xF800) << 8) | ((wColor & 0x07E0) << 5) | ((wColor & 0x001F) << 3);
		}

		static uint16 R5G6B5ToA1R5G5B5(uint16 wColor)
		{
			return 0x8000 | (((wColor & 0xFFC0) >> 1) | (wColor & 0x1F));
		}

		static uint16 A1R5G5B5ToR5G6B5(uint16 wColor)
		{
			return (((wColor & 0x7FE0) << 1) | (wColor & 0x1F));
		}

		//			ARGB from A1R5G5B5

		static uint32 GetAlpha(uint16 wColor)
		{
			return ((wColor >> 15) & 0x1);
		}

		static uint32 GetRed(uint16 wColor)
		{
			return ((wColor >> 10) & 0x1F);
		}

		static uint32 GetGreen(uint16 wColor)
		{
			return ((wColor >> 5) & 0x1F);
		}

		static uint32 GetBlue(uint16 wColor)
		{
			return (wColor & 0x1F);
		}

		static int GetRedSigned(uint16 wColor)
		{
			return ((wColor >> 10) & 0x1F);
		}

		static int GetGreenSigned(uint16 wColor)
		{
			return ((wColor >> 5) & 0x1F);
		}

		static int GetBlueSigned(uint16 wColor)
		{
			return (wColor & 0x1F);
		}

		static int GetAverage(uint16 wColor)
		{
			return ((GetRed(wColor) << 3) + (GetGreen(wColor) << 3) + (GetBlue(wColor) << 3)) / 3;
		}

		// Monochrome -> A1R5G5B5
		static void Convert1BitTo16Bits(const byte *pSrc, uint16 *pDest, int iWidth, int iHeight, int iLinePad=0, bool bFlip=false);
		// 4 bits to A1R5G5B5
		static void Convert4BitTo16Bit(const byte *pSrc, uint16 *pDest, int iWidth, int iHeight, const uint32 *pPalette, int iLinePad=0, bool bFlip=false);
		// 8 bits ti A1R5G5B5
		static void Convert8BitTo16Bit(const byte *pSrc, uint16 *pDest, int iWidth, int iHeight, const uint32 *pPalette, int iLinePad=0, bool bFlip=false);
		// R8G8B8 to A1R5G5B5
		static void Convert16BitTo16Bit(const uint16 *pSrc, uint16 *pDest, int iWidth, int iHeight, int iLinePad, bool bFlip=false);
		// R8G8B8 to R8G8B8 ; Flip and mirror
		static void Convert24BitTo24Bit(const byte *pSrc, byte *pDest, int iWidth, int iHeight, int iLinePad=0, bool bFlip=false, bool bBgr=false);

		static void Convert16BitToA8R8G8B8AndResize(const uint16 *pSrc, uint32 *pOut, int iNewWidth, int iNewHeight, int iWidth, int iHeight);
		// X8R8G8B8, flip and mirror
		static void Convert32BitTo32Bit(const uint32 *pSrc, uint32 *pDest, int iWidth, int iHeight, int iLinePad, bool bFlip=false);

		static void ConvertA1R5G5B5ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA1R5G5B5ToB8G8R8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA1R5G5B5ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA1R5G5B5ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA1R5G5B5ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount);

		static void ConvertA8R8G8B8ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA8R8G8B8ToB8G8R8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA8R8G8B8ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA8R8G8B8ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertA8R8G8B8ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount);

		static void ConvertA8R8G8B8ToR3G3B2(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR8G8B8ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR8G8B8ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR8G8B8ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR8G8B8ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount);

		static void ConvertR5G6B5ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR5G6B5ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR5G6B5ToB8G8R8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR5G6B5ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount);
		static void ConvertR5G6B5ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount);
};

}

#endif