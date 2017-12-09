
#ifndef _TEXTURE_H_

#define _TEXTURE_H_

#include "IResource.h"
#include <d3dx9.h>
#include "../Dev/MemDebugger.h"
#include "../Common.h"

namespace Core
{

class Texture : public IResource
{
	public:

		Texture(IDirect3DDevice9 *pDevice);
		~Texture();

		bool Load(const char *szName, const char *szPath=0);
		bool Save(const char *szPath);
		bool Create(const char *szName, const int iWidth, const int iHeight, D3DFORMAT _Format);
		bool Compress();
		bool GenerateMipMaps();
		bool Fill(void *pBuffer, int iWidth, int iHeight, int iBPP);
		void Release();

		IDirect3DTexture9 *GetTexture() { return m_pTexture; };
		int GetWidth() const { return m_iWidth; };
		int GetHeight() const { return m_iHeight; };
		D3DFORMAT GetFormat() const { return m_Format; };
		const char *GetName() const { return m_szName; };

		bool BeginRestart();
		bool EndRestart(IDirect3DDevice9 *pDevice);

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("Texture")
		MEMDEBUG_CLASSSIZE(sizeof(Texture))

	protected:

		D3DFORMAT GetDXTFormat(D3DFORMAT _Format);
		bool ConvertTo(D3DFORMAT _Format);

		IDirect3DDevice9 *m_pDevice;
		IDirect3DTexture9 *m_pTexture;

		int m_iWidth,m_iHeight;
		uint32 m_dwPitch;
		D3DFORMAT m_Format;
		D3DXIMAGE_FILEFORMAT m_FileFormat;
		bool m_bMustSaveAtRestart;
		bool m_bMustMipMapAtRestart;
		bool m_bCreated; //    Created ? Loaded
		bool m_bValidRestart;

		char m_szName[64];
		byte *m_pRestartBuffer;
};

}

#endif