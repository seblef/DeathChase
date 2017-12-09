
#ifndef _TEXTUREMGR_H_

#define _TEXTUREMGR_H_

#include "TSingleton.h"
#include "Texture.h"
#include "THandleMgr.h"

#include <map>
#include <string>

#ifdef DEEXPORT
#include "../Export/ITextureMgr.h"
#endif

namespace Core
{
#ifdef DEEXPORT
class TextureMgr : public ITextureMgr, public TSingleton<TextureMgr>
#else
class TextureMgr : public TSingleton<TextureMgr>
#endif
{
	public:
 
		typedef THandleMgr<Texture *,HTexture> HTextureMgr;
		
		TextureMgr(IDirect3DDevice9 *pDevice) : m_pDevice(pDevice) {};
		~TextureMgr();
		
		HTexture CreateEmptyTexture(const int iWidth,
									const int iHeight,
									D3DFORMAT _Format,
									const char *szName);
		HTexture CreateTexture(const int iWidth,
							   const int iHeight,
							   D3DFORMAT _Format,
							   void *pBuffer,
							   const char *szName);
		HTexture LoadTexture(const char *szName, const char *szPath=0);
		void ReleaseTexture(HTexture &hTex);
		HTexture GetTexture(const char *szName);
		bool FillTexture(HTexture hTex, void *pBuffer, int iWidth, int iHeight, int iBPP);

		bool CompressTexture(HTexture hTex);
		bool GenerateTextureMipMaps(HTexture hTex);
		
		IDirect3DTexture9 *GetTexture(HTexture hTex);
		bool GetTextureSize(HTexture hTex, int& iWidth, int& iHeight);
		bool GetTextureName(HTexture hTex, char *szName);

		void SetTexture(const int iStage, const HTexture hTex);

		bool SaveTexture(Core::HTexture hTex, const char *szPath=0);

		bool BeginRestart();
		bool EndRestart(IDirect3DDevice9 *pDevice);

	private:
	
		struct istring_less
		{
			bool operator() (const std::string& l, const std::string& r) const
			{
				return _stricmp(l.c_str(),r.c_str()) < 0;
			}
		};
	
		typedef std::map<std::string,HTexture,istring_less> NameIndex;
		
		HTextureMgr m_Textures;
		NameIndex m_NameIndex;
		
		IDirect3DDevice9 *m_pDevice;

};

}

#define g_TextureMgr			Core::TextureMgr::GetSingletonRef()

#endif
