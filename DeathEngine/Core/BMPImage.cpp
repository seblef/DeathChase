
#include "BMPImage.h"
#include "../MyAssert.h"
#include <windows.h>
#include <stdio.h>

namespace Core
{

BMPImage::BMPImage() : m_pBuffer(0),m_pPalette(0),
	m_iWidth(-1),m_iHeight(-1),m_iBPP(-1)
{}

BMPImage::~BMPImage()
{
	Clear();
}

bool BMPImage::Load(const char *szName)
{
	MYASSERT(szName,"Bad bmp image name pointer",true);

	Clear();

	FILE *pFile=fopen(szName,"rb");
	if(!pFile)
		return false;

	BITMAPFILEHEADER header;
	fread(&header,sizeof(header),1,pFile);

	if(header.bfType!=0x424d && header.bfType!=0x4d42)
	{
		fclose(pFile);
		return false;
	}

	BITMAPINFOHEADER info;
	fread(&info,sizeof(info),1,pFile);

	if(info.biCompression!=BI_RGB)
	{
		fclose(pFile);
		return false;
	}

	int iBufferSize=info.biWidth * info.biHeight;
	int iPaletteSize=-1;

	switch(info.biBitCount)
	{
		case 1:
			iBufferSize/=8;
			iPaletteSize=6;
			break;

		case 2:
			iBufferSize/=4;
			iPaletteSize=4*3;
			break;

		case 4:
			iBufferSize/=2;
			iPaletteSize=16*3;
			break;

		case 8:
			iPaletteSize=256*3;
			break;

		case 16:
			iBufferSize*=2;
			break;

		case 24:
			iBufferSize*=3;
			break;

		case 32:
			iBufferSize*=4;
			break;

		default:
			fclose(pFile);
			return false;
	}

	m_pBuffer=new byte[iBufferSize];
	if(!m_pBuffer)
	{
		fclose(pFile);
		return false;
	}

	if(iPaletteSize!=-1)
	{
		m_pPalette=new byte[iPaletteSize];
		if(!m_pPalette)
		{
			delete[] m_pBuffer;
			m_pBuffer=0;
			fclose(pFile);
			return false;
		}

		fread(m_pPalette,iPaletteSize,1,pFile);
	}

	fread(m_pBuffer,iBufferSize,1,pFile);
	fclose(pFile);

	m_iWidth=info.biWidth;
	m_iHeight=info.biHeight;
	m_iBPP=info.biBitCount;

	return true;
}

bool BMPImage::Create(const int iWidth, const int iHeight, const int iBPP)
{
	MYASSERT(iWidth > 0 && iHeight > 0 && iBPP > 0 && iBPP <= 32,"Bad bmp image create params",true);

	Clear();

	int iBufferSize=iWidth * iHeight;
	int iPaletteSize=-1;

	switch(iBPP)
	{
		case 1:
			iBufferSize/=8;
			iPaletteSize=6;
			break;

		case 2:
			iBufferSize/=4;
			iPaletteSize=4*3;
			break;

		case 4:
			iBufferSize/=2;
			iPaletteSize=16*3;
			break;

		case 8:
			iPaletteSize=256*3;
			break;

		case 16:
			iBufferSize*=2;
			break;

		case 24:
			iBufferSize*=3;
			break;

		case 32:
			iBufferSize*=4;
			break;

		default:
			return false;
	}

	m_pBuffer=new byte[iBufferSize];
	if(!m_pBuffer)
		return false;

	if(iPaletteSize!=-1)
	{
		m_pPalette=new byte[iPaletteSize];
		if(!m_pPalette)
		{
			delete[] m_pBuffer; m_pBuffer=0;
			return false;
		}
	}

	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_iBPP=iBPP;

	return true;
}

bool BMPImage::Save(const char *szName)
{
	return true;
}

void BMPImage::Clear()
{
	if(m_pBuffer) delete[] m_pBuffer;
	if(m_pPalette) delete[] m_pPalette;
	
	m_pBuffer=0;
	m_pPalette=0;

	m_iWidth=-1;
	m_iHeight=-1;
	m_iBPP=-1;
}

void BMPImage::MirrorY()
{
	if(!m_pBuffer)
		return;

	if(m_iBPP==24)
	{
		byte *pTemp=new byte[m_iWidth*m_iHeight*3];
		byte *pSrc=m_pBuffer + (m_iWidth * (m_iHeight-1)) * 3;
		byte *pDest=pTemp;
		int y;

		for(y=0;y<m_iHeight;y++)
		{
			memcpy(pDest,pSrc,m_iWidth*3);
			pDest+=m_iWidth*3;
			pSrc-=m_iWidth*3;
		}

		delete[] m_pBuffer;
		m_pBuffer=pTemp;
	}
}

}