
#include "Texture.h"
#include "BMPImage.h"
#include "../MyAssert.h"
#include "../Dirs.h"
#include <d3dx9.h>
#include <stdio.h>
#include <string>

namespace Core
{

Texture::Texture(IDirect3DDevice9 *pDevice) : m_pDevice(pDevice),
	m_pTexture(0),m_iWidth(-1),m_iHeight(-1),m_bMustSaveAtRestart(false),
	m_bMustMipMapAtRestart(false),m_pRestartBuffer(0),m_bCreated(false),
	m_bValidRestart(false),m_FileFormat(D3DXIFF_JPG)
{
	MYASSERT(pDevice,"Bad d3d device pointer",true);
}

Texture::~Texture()
{
	Release();
}

bool Texture::Load(const char *szName, const char *szPath)
{
	MYASSERT(szName,"Bad texture name pointer",true);

	Release();

	char buf[1024];
	if(szPath)
		sprintf(buf,"%s%s",szPath,szName);
	else
		sprintf(buf,"Textures/%s",szName);
		
	D3DXIMAGE_INFO info;
	if(D3DXGetImageInfoFromFile(buf,&info)!=D3D_OK)
	{
		m_pTexture=0;
		g_Output->AddMsg("x Failed GetImageInfo [%s].\n",szName);
		return false;
	}

	m_iWidth=info.Width;
	m_iHeight=info.Height;
	m_Format=GetDXTFormat(info.Format);
	m_FileFormat=info.ImageFileFormat;

	if(D3DXCreateTextureFromFileEx(m_pDevice, buf,
								   D3DX_DEFAULT, // Width
								   D3DX_DEFAULT, // Height
								   D3DX_DEFAULT, // Mips
								   0, // Usage
								   m_Format, // Format
								   D3DPOOL_MANAGED, // Pool
								   D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, // Filtre
								   D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, // Filtre Mips
								   0, // ColorKey
								   NULL,NULL,
								   &m_pTexture)!=D3D_OK)
	{
		m_pTexture=0;
		g_Output->AddMsg("x Failed loading texture\n");
		return false;
	}

	strcpy(m_szName,szName);

	return true;
}

bool Texture::Create(const char *szName,
					 const int iWidth, const int iHeight, D3DFORMAT _Format)
{
	MYASSERT(szName,"Bad texture name pointer",true);

	if(m_pDevice->CreateTexture(iWidth,iHeight,1,D3DUSAGE_DYNAMIC,
		_Format,D3DPOOL_DEFAULT,&m_pTexture,0)!=D3D_OK)
		return false;

	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_Format=_Format;

	if(_Format==D3DFMT_A8R8G8B8 ||
	   _Format==D3DFMT_A1R5G5B5 ||
	   _Format==D3DFMT_A4R4G4B4 ||
	   _Format==D3DFMT_A8R3G3B2)
	   m_FileFormat=D3DXIFF_TGA;
	else if(_Format==D3DFMT_P8)
		m_FileFormat=D3DXIFF_BMP;
	else
		m_FileFormat=D3DXIFF_JPG;

	strcpy(m_szName,szName);
	m_bMustSaveAtRestart=true;
	m_bCreated=true;
	return true;
}

void Texture::Release()
{
	if(m_pRestartBuffer)
		delete[] m_pRestartBuffer;

	m_pRestartBuffer=0;
	m_bMustSaveAtRestart=false;
	m_bMustMipMapAtRestart=false;

	if(m_pTexture)
		m_pTexture->Release();

	m_pTexture=0;
	m_iWidth=-1;
	m_iHeight=-1;
}

bool Texture::Compress()
{
	return ConvertTo(GetDXTFormat(m_Format));
}

bool Texture::ConvertTo(D3DFORMAT _Format)
{
	if(!m_pTexture)
		return false;

	D3DSURFACE_DESC desc;
	m_pTexture->GetLevelDesc(0,&desc);

	if(desc.Format==_Format)
		return true;

	IDirect3DTexture9 *pTexture;

	if(m_pDevice->CreateTexture(desc.Width,desc.Height,0,0,_Format,D3DPOOL_MANAGED,&pTexture,0)!=D3D_OK)
		return false;

	IDirect3DSurface9 *pSrc,*pDest;

	if(pTexture->GetSurfaceLevel(0,&pDest)!=D3D_OK)
	{
		pTexture->Release();
		return false;
	}

	if(m_pTexture->GetSurfaceLevel(0,&pSrc)!=D3D_OK)
	{
		pTexture->Release();
		return false;
	}

	if(D3DXLoadSurfaceFromSurface(pDest,0,0,pSrc,0,0,D3DX_FILTER_NONE,0)!=D3D_OK)
	{
		pTexture->Release();
		return false;
	}

	m_pTexture->Release();
	m_pTexture=pTexture;
	m_Format=_Format;
	m_bMustSaveAtRestart=true;

	return true;
}

bool Texture::GenerateMipMaps()
{
	if(m_pTexture)
	{
		if(m_pTexture->GetLevelCount() > 0)
			return true;

		m_pTexture->GenerateMipSubLevels();
		return true;

		m_bMustMipMapAtRestart=true;
	}
	else
		return false;
}

bool Texture::Fill(void *pBuffer, int iWidth, int iHeight, int iBPP)
{
	if(iWidth!=m_iWidth || iHeight!=m_iHeight || !m_pTexture || !pBuffer)
		return false;

	D3DLOCKED_RECT rect;
	if(m_pTexture->LockRect(0,&rect,0,D3DLOCK_DISCARD)!=D3D_OK)
		return false;

	if(rect.Pitch==(iWidth*iBPP))
		memcpy(rect.pBits,pBuffer,iHeight*iWidth*iBPP);
	else
	{
		int y;
		byte *src=(byte *)pBuffer;
		byte *dest=(byte *)rect.pBits;
		int bwidth=iWidth*iBPP;

		for(y=0;y<iHeight;y++)
		{
			memcpy(dest,src,bwidth);
			memset(dest+bwidth,0,rect.Pitch - bwidth);

			src+=bwidth;
			dest+=rect.Pitch;
		}
	}

	m_pTexture->UnlockRect(0);

	return true;
}

bool Texture::BeginRestart()
{
	if(!m_pTexture)
		return true;

	if(m_pRestartBuffer)
	{
		delete[] m_pRestartBuffer;
		m_pRestartBuffer=0;
	}

	if(m_bMustSaveAtRestart)
	{
		if(m_Format)
			ConvertTo(D3DFMT_A8R8G8B8);

		D3DLOCKED_RECT rect;
		if(m_pTexture->LockRect(0,&rect,0,D3DLOCK_DISCARD)!=D3D_OK)
			return false;

		m_pRestartBuffer=new byte[rect.Pitch * m_iHeight];
		memcpy(m_pRestartBuffer,rect.pBits,rect.Pitch * m_iHeight);
		m_dwPitch=rect.Pitch;

		m_pTexture->UnlockRect(0);
	}

	m_bValidRestart=true;
	m_pTexture->Release();
	m_pTexture=0;

	return true;
}

bool Texture::EndRestart(IDirect3DDevice9 *pDevice)
{
	m_pDevice=pDevice;
	if(m_bValidRestart)
	{
		std::string temp(m_szName);

		if(m_bCreated)
		{
			if(!Create(temp.c_str(),m_iWidth,m_iHeight,m_Format))
				return false;
		}
		else
		{
			if(!Load(temp.c_str()))
				return false;
		}

		if(m_bMustSaveAtRestart && m_pRestartBuffer)
		{
			D3DLOCKED_RECT rect;
			if(m_pTexture->LockRect(0,&rect,0,D3DLOCK_DISCARD)!=D3D_OK)
				return false;

			if(rect.Pitch!=m_dwPitch)
			{
				m_pTexture->UnlockRect(0);
				Fill((void *)m_pRestartBuffer,(int)m_dwPitch / 4,m_iHeight,4);
			}
			else
			{
				memcpy(rect.pBits,m_pRestartBuffer,rect.Pitch * m_iHeight);
				m_pTexture->UnlockRect(0);
			}

			delete[] m_pRestartBuffer;
			m_pRestartBuffer=0;
		}

		if(m_bMustMipMapAtRestart)
			GenerateMipMaps();

		m_bValidRestart=false;
	}

	return true;
}

bool Texture::Save(const char *szPath)
{
	if(!m_pTexture) return false;

	char szTemp[1024];
	if(szPath)
		sprintf(szTemp,"%s%s",szPath,m_szName);
	else
		sprintf(szTemp,"%s%s",DIR_TEXTURES,m_szName);

	return D3DXSaveTextureToFile(szTemp,m_FileFormat,m_pTexture,0)==D3D_OK;
}

D3DFORMAT Texture::GetDXTFormat(D3DFORMAT format)
{
	switch(format)
	{
		case D3DFMT_A8R8G8B8:
			return D3DFMT_DXT5;
			break;

		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
			return D3DFMT_DXT3;
			break;

		default:
			return D3DFMT_DXT1;
			break;
	}
}

}
