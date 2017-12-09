
#include "ColorConverter.h"
#include <memory.h>

namespace Core
{

void ColorConverter::Convert1BitTo16Bits(const byte *pSrc, uint16 *pDest, int iWidth, int iHeight, int iLinePad, bool bFlip)
{
	if(!pSrc || !pDest)
		return;

	if(bFlip)
		*pDest+=iWidth*iHeight;

	int x,y;
	for(y=0;y<iHeight;y++)
	{
		int iShift=7;
		if(bFlip)
			pDest-=iWidth;

		for(x=0;x<iWidth;x++)
		{
			pDest[x]=*pSrc >> iShift & 0x01 ? (uint16)0xffff : (uint16)0x8000;

			if((--iShift) < 0)
			{
				iShift=7;
				pSrc++;
			}
		}

		if(iShift!=7)
			pSrc++;

		if(!bFlip)
			pDest+=iWidth;

		pSrc+=iLinePad;
	}
}

void ColorConverter::Convert4BitTo16Bit(const byte *pSrc, uint16 *pDest, int iWidth, int iHeight, const uint32 *pPalette, int iLinePad, bool bFlip)
{
	if(!pSrc || !pDest || !pPalette)
		return;

	static uint16 wPalette[16];
	
	int i;
	for(i=0;i<16;i++)
		wPalette[i]=X8R8G8B8ToA1R5G5B5(pPalette[i]);

	if(bFlip)
		pDest+=iWidth * iHeight;

	int x,y;
	for(y=0;y<iHeight;y++)
	{
		int iShift=4;
		if(bFlip)
			pDest-=iWidth;

		for(x=0;x<iWidth;x++)
		{
			pDest[x]=wPalette[(byte)((*pSrc >> iShift) & 0xf)];

			if(iShift==0)
			{
				iShift=4;
				pSrc++;
			}
			else
				iShift=0;
		}

		if(iShift==0)
			pSrc++;

		if(!bFlip)
			pDest+=iWidth;

		pSrc+=iLinePad;
	}
}

void ColorConverter::Convert8BitTo16Bit(const byte *pSrc, uint16 *pDest, int iWidth, int iHeight, const uint32 *pPalette, int iLinePad, bool bFlip)
{
	if(!pSrc || !pDest || !pPalette)
		return;

	static uint16 wPalette[256];
	int i;
	for(i=0;i<256;i++)
		wPalette[i]=X8R8G8B8ToA1R5G5B5(pPalette[i]);

	if(bFlip)
		pDest+=iWidth*iHeight;

	int x,y;
	for(y=0;y<iHeight;y++)
	{
		if(bFlip)
			pDest-=iWidth;

		for(x=0;x<iWidth;x++)
		{
			pDest[x]=wPalette[(byte)(*pSrc)];
			pSrc++;
		}

		if(!bFlip)
			pDest+=iWidth;

		pSrc+=iLinePad;
	}
}

void ColorConverter::Convert16BitTo16Bit(const uint16 *pSrc, uint16 *pDest, int iWidth, int iHeight, int iLinePad, bool bFlip)
{
	if(!pSrc || !pDest)
		return;

	if(bFlip)
		pDest+=iWidth*iHeight;

	int y;
	for(y=0;y<iHeight;y++)
	{
		if(bFlip)
			pDest-=iWidth;

		memcpy(pDest,pSrc,iWidth*sizeof(uint16));

		if(!bFlip)
			pDest+=iWidth;

		pSrc+=iWidth;
		pSrc+=iLinePad;
	}
}

void ColorConverter::Convert24BitTo24Bit(const byte *pSrc, byte *pDest, int iWidth, int iHeight, int iLinePad, bool bFlip, bool bBgr)
{
	if(!pSrc || !pDest)
		return;

	const int iLineWidth=3*iWidth;

	if(bFlip)
		pDest+=iLineWidth * iHeight;

	int x,y;
	for(y=0;y<iHeight;y++)
	{
		if(bFlip)
			pDest-=iLineWidth;

		if(bBgr)
		{
			for(x=0;x<iLineWidth;x+=3)
			{
				pDest[x]=pSrc[x+2];
				pDest[x+1]=pSrc[x+1];
				pDest[x+2]=pSrc[x];
			}
		}
		else
			memcpy(pDest,pSrc,iLineWidth);

		if(!bFlip)
			pDest+=iLineWidth;

		pSrc+=iLineWidth;
		pSrc+=iLinePad;
	}
}

void ColorConverter::Convert16BitToA8R8G8B8AndResize(const uint16 *pSrc, uint32 *pDest, int iNewWidth, int iNewHeight, int iWidth, int iHeight)
{
	if(!pSrc || !pDest || !iNewWidth || !iNewHeight)
		return;

	const float fSrcXStep=(float)iWidth / (float)iNewWidth;
	const float fSrcYStep=(float)iHeight / (float)iNewHeight;
	float sy;
	int t;
	int x,y;

	for(x=0;x<iNewWidth;x++)
	{
		sy=0.0f;

		for(y=0;y<iNewHeight;y++)
		{
			t=pSrc[(int)(((int)sy) * iWidth + x*fSrcXStep)];
			t=(((t >> 15) & 0x1) << 31) | (((t >> 10) & 0x1F) << 19) |
				(((t >> 5) & 0x1F) << 11) | (t & 0x1F) << 3;
			pDest[(int)(y*iNewWidth+x)]=t;

			sy+=fSrcYStep;
		}
	}
}

void ColorConverter::Convert32BitTo32Bit(const uint32 *pSrc, uint32 *pDest, int iWidth, int iHeight, int iLinePad, bool bFlip)
{
	if(!pSrc || !pDest)
		return;

	if(bFlip)
		pDest+=iWidth*iHeight;

	int y;
	for(y=0;y<iHeight;y++)
	{
		if(bFlip)
			pDest-=iWidth;

		memcpy(pDest,pSrc,iWidth*sizeof(uint32));

		if(!bFlip)
			pDest+=iWidth;

		pSrc+=iWidth;
		pSrc+=iLinePad;
	}
}

void ColorConverter::ConvertA1R5G5B5ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	byte *dB=(byte *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		dB[2]=(*sB & 0x7c00) >> 7;
		dB[1]=(*sB & 0x03e0) >> 2;
		dB[0]=(*sB & 0x1f) << 3;

		sB+=1; dB+=3;
	}
}

void ColorConverter::ConvertA1R5G5B5ToB8G8R8(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	byte *dB=(byte *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		dB[0]=(*sB & 0x7c00) >> 7;
		dB[1]=(*sB & 0x03e0) >> 2;
		dB[1]=(*sB & 0x1f) << 3;

		sB+=1; dB+=3;
	}
}

void ColorConverter::ConvertA1R5G5B5ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	uint32 *dB=(uint32 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
		*dB++=A1R5G5B5ToA8R8G8B8(*sB++);
}

void ColorConverter::ConvertA1R5G5B5ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	memcpy(pDest,pSrc,dwCount * 2);
}

void ColorConverter::ConvertA1R5G5B5ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	uint16 *dB=(uint16 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
		*dB++=A1R5G5B5ToR5G6B5(*sB++);
}

void ColorConverter::ConvertA8R8G8B8ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	byte *sB=(byte *)pSrc;
	byte *dB=(byte *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		dB[0]=sB[2];
		dB[1]=sB[1];
		dB[2]=sB[0];

		sB+=4;
		dB+=3;
	}
}

void ColorConverter::ConvertA8R8G8B8ToB8G8R8(const void *pSrc, void *pDest, uint32 dwCount)
{
	byte *sB=(byte *)pSrc;
	byte *dB=(byte *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		dB[0]=sB[0];
		dB[1]=sB[1];
		dB[2]=sB[2];

		sB+=4;
		dB+=3;
	}
}

void ColorConverter::ConvertA8R8G8B8ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	memcpy(pDest,pSrc,dwCount*4);
}

void ColorConverter::ConvertA8R8G8B8ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint32 *sB=(uint32 *)pSrc;
	uint16 *dB=(uint16 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
		*dB++=A8R8G8B8ToA1R5G5B5(*sB++);
}

void ColorConverter::ConvertA8R8G8B8ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	byte *sB=(byte *)pSrc;
	uint16 *dB=(uint16 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		int r=sB[2] >> 3;
		int g=sB[1] >> 2;
		int b=sB[0] >> 3;

		dB[0]=(r << 11) | (g << 5) | (b);

		sB+=4;
		dB+=1;
	}
}

void ColorConverter::ConvertA8R8G8B8ToR3G3B2(const void *pSrc, void *pDest, uint32 dwCount)
{
	byte *sB=(byte *)pSrc;
	byte *dB=(byte *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		byte r=sB[2] & 0xe0;
		byte g=(sB[1] & 0xe0) >> 3;
		byte b=(sB[0] & 0xe0) >> 6;

		dB[0]=(r | g | b);

		sB+=4;
		dB+=1;
	}
}

void ColorConverter::ConvertR8G8B8ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	memcpy(pDest,pSrc,dwCount * 3);
}

void ColorConverter::ConvertR8G8B8ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	byte *sB=(byte *)pSrc;
	uint32 *dB=(uint32 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		*dB=0xff000000 | (sB[0] << 16) | (sB[1] << 8) | sB[2];

		sB+=3;
		dB++;
	}
}

void ColorConverter::ConvertR8G8B8ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	byte *sB=(byte *)pSrc;
	uint16 *dB=(uint16 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		int r=sB[0] >> 3;
		int g=sB[1] >> 3;
		int b=sB[2] >> 3;

		dB[0]=(0x8000) | (r << 10) | (g << 5) | (b);

		sB+=3;
		dB+=1;
	}
}

void ColorConverter::ConvertR8G8B8ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	byte *sB=(byte *)pSrc;
	uint16 *dB=(uint16 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		int r=sB[0] >> 3;
		int g=sB[1] >> 2;
		int b=sB[2] >> 3;

		dB[0]=(r << 11) | (g << 5) | (b);

		sB+=3;
		dB+=1;
	}
}

void ColorConverter::ConvertR5G6B5ToR5G6B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	memcpy(pDest,pSrc,dwCount*2);
}

void ColorConverter::ConvertR5G6B5ToR8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	byte *dB=(byte *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		dB[0]=(*sB & 0xf800) << 8;
		dB[1]=(*sB & 0x07e0) << 2;
		dB[2]=(*sB & 0x001f) << 3;

		sB+=4;
		dB+=3;
	}
}

void ColorConverter::ConvertR5G6B5ToB8G8R8(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	byte *dB=(byte *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
	{
		dB[2]=(*sB & 0xf800) << 8;
		dB[1]=(*sB & 0x07e0) << 2;
		dB[0]=(*sB & 0x001f) << 3;

		sB+=4;
		dB+=3;
	}
}

void ColorConverter::ConvertR5G6B5ToA8R8G8B8(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	uint32 *dB=(uint32 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
		*dB++=R5G6B5ToA8R8G8B8(*sB++);
}

void ColorConverter::ConvertR5G6B5ToA1R5G5B5(const void *pSrc, void *pDest, uint32 dwCount)
{
	uint16 *sB=(uint16 *)pSrc;
	uint16 *dB=(uint16 *)pDest;

	uint32 x;
	for(x=0;x<dwCount;x++)
		*dB++=R5G6B5ToA1R5G5B5(*sB++);
}

}