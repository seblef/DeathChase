
#ifndef _ITEXTUREMGR_H_
#define _ITEXTUREMGR_H_

#include "../Core/Handles.h"
#include <d3d9.h>

class ITextureMgr
{
	public:

		ITextureMgr() {}
		virtual ~ITextureMgr() {}

		virtual Core::HTexture CreateEmptyTexture(const int iWidth,
									const int iHeight,
									D3DFORMAT _Format,
									const char *szName)=0;
		virtual Core::HTexture CreateTexture(const int iWidth,
							   const int iHeight,
							   D3DFORMAT _Format,
							   void *pBuffer,
							   const char *szName)=0;
		virtual Core::HTexture LoadTexture(const char *szName, const char *szPath=0)=0;

		virtual void ReleaseTexture(Core::HTexture &hTex)=0;

		virtual Core::HTexture GetTexture(const char *szName)=0;
		virtual IDirect3DTexture9 *GetTexture(Core::HTexture hTex)=0;
		virtual bool GetTextureSize(Core::HTexture hTex, int& iWidth, int& iHeight)=0;
		virtual bool GetTextureName(Core::HTexture hTex, char *szName)=0;

		virtual bool FillTexture(Core::HTexture hTex, void *pBuffer, int iWidth, int iHeight, int iBPP)=0;
		virtual bool CompressTexture(Core::HTexture hTex)=0;
		virtual bool GenerateTextureMipMaps(Core::HTexture hTex)=0;
		
		virtual void SetTexture(const int iStage, const Core::HTexture hTex)=0;

		virtual bool SaveTexture(Core::HTexture hTex, const char *szPath=0)=0;
};

#endif