
#include "TextureMgr.h"
#include "Renderer.h"
#include "../Dirs.h"
#include <d3dx9.h>

namespace Core
{

TextureMgr::~TextureMgr()
{
	NameIndex::iterator i;
	
	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Texture *pTex=*m_Textures.Dereference(i->second);

		if(pTex)
		{
			pTex->Release();
			delete pTex;
		}
	}
}

HTexture TextureMgr::LoadTexture(const char *szName, const char *szPath)
{
	if(!szName) return HTexture();

	HTexture hTex;

	std::string strName(szName);

	if(szPath)
		strName=szPath;
	else
		strName=DIR_TEXTURES;

	strName+=szName;

	NameIndex::iterator rc=m_NameIndex.find(strName);

	if(rc==m_NameIndex.end())
	{
		Texture **pTex=m_Textures.Acquire(hTex);
		*pTex=new Texture(m_pDevice);

		bool bOK=(*pTex)->Load(strName.c_str(),"");
		if(!bOK && szPath)
			bOK=(*pTex)->Load(szName);
		
		if(!bOK)
		{
			delete *pTex;
			m_Textures.Release(hTex);
			hTex=HTexture();
			g_Output->AddMsg("x Failed loading texture %s\n",strName.c_str());
		}
		else
			m_NameIndex.insert(std::make_pair(strName,hTex));
	}
	else
	{
		hTex=rc->second;
		Texture **pTex=m_Textures.Dereference(hTex);

		if(pTex && *pTex)
			(*pTex)->AddRef();
	}
	
	return hTex;
}

HTexture TextureMgr::CreateEmptyTexture(const int iWidth, const int iHeight,
										D3DFORMAT _Format, const char *szName)
{
	if(!szName) return HTexture();

	HTexture hTex;
	std::string strName(szName);
	NameIndex::iterator rc=m_NameIndex.find(strName);

	if(rc==m_NameIndex.end())
	{
		Texture **pTex=m_Textures.Acquire(hTex);
		*pTex=new Texture(m_pDevice);
		
		if(!(*pTex)->Create(szName,iWidth,iHeight,_Format))
		{
			delete *pTex;
			m_Textures.Release(hTex);
			hTex=HTexture();
			g_Output->AddMsg("x Failed creating texture %s\n",szName);
		}
		else
			m_NameIndex.insert(std::make_pair(strName,hTex));
	}
	else
	{
		hTex=rc->second;
		(*m_Textures.Dereference(hTex))->AddRef();
	}

	return hTex;
}

HTexture TextureMgr::GetTexture(const char *szName)
{
	MYASSERT(szName,"Bad texture name",true);

	NameIndex::iterator tex=m_NameIndex.find(std::string(szName));

	if(tex==m_NameIndex.end())
		return HTexture();
	else
		return tex->second;
}

HTexture TextureMgr::CreateTexture(const int iWidth, const int iHeight,
										D3DFORMAT _Format, void *pBuffer, const char *szName)
{
	if(!szName) return HTexture();

	HTexture hTex;
	std::string strName(szName);
	NameIndex::iterator rc=m_NameIndex.find(strName);

	if(rc==m_NameIndex.end())
	{
		Texture **pTex=m_Textures.Acquire(hTex);
		*pTex=new Texture(m_pDevice);
		
		if(!(*pTex)->Create(szName,iWidth,iHeight,_Format))
		{
			delete *pTex;
			m_Textures.Release(hTex);
			hTex=HTexture();
			g_Output->AddMsg("x Failed creating texture %s\n",szName);
		}
		else
		{
			m_NameIndex.insert(std::make_pair(strName,hTex));
			(*pTex)->Fill(pBuffer,iWidth,iHeight,4);
		}
	}
	else
	{
		hTex=rc->second;
		(*m_Textures.Dereference(hTex))->AddRef();
	}

	return hTex;
}

void TextureMgr::ReleaseTexture(HTexture &hTex)
{
	Texture **pTex=m_Textures.Dereference(hTex);
	
	if(pTex && *pTex)
	{
		(*pTex)->RemRef();
		
		if((*pTex)->GetRefCount() <= 0)
		{
			NameIndex::iterator i=m_NameIndex.find((*pTex)->GetName());
			if(i!=m_NameIndex.end())
				m_NameIndex.erase(i);

			(*pTex)->Release();
			delete *pTex;
			m_Textures.Release(hTex);
		}
	}

	hTex=HTexture();
}

IDirect3DTexture9 *TextureMgr::GetTexture(HTexture hTex)
{
	Texture **ppTex=m_Textures.Dereference(hTex);
	if(ppTex && *ppTex)
		return (*ppTex)->GetTexture();
	else
		return 0;
}

void TextureMgr::SetTexture(const int iStage, const HTexture hTex)
{
	Texture **ppTex=m_Textures.Dereference(hTex);

	if(ppTex)
		g_Renderer.SetTexture(iStage,*ppTex);
}

bool TextureMgr::GetTextureSize(HTexture hTex, int &iWidth, int &iHeight)
{
	if(!hTex.IsNull())
	{
		Texture **pTex=m_Textures.Dereference(hTex);

		if(pTex && *pTex)
		{
			iWidth=(*pTex)->GetWidth();
			iHeight=(*pTex)->GetHeight();

			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool TextureMgr::CompressTexture(Core::HTexture hTex)
{
	Texture **ppTex=m_Textures.Dereference(hTex);

	if(ppTex && *ppTex)
		return (*ppTex)->Compress();
	else
		return false;
}

bool TextureMgr::SaveTexture(HTexture hTex, const char *szPath)
{
	Texture **ppTex=m_Textures.Dereference(hTex);

	if(ppTex && *ppTex)
		return (*ppTex)->Save(szPath);
	else
		return false;
}

bool TextureMgr::GetTextureName(Core::HTexture hTex, char *szName)
{
	if(!szName) return false;
	Texture **ppTex=m_Textures.Dereference(hTex);

	if(ppTex && *ppTex)
	{
		strcpy(szName,(*ppTex)->GetName());
		return true;
	}
	else
	{
		strcpy(szName,"");
		return false;
	}
}

bool TextureMgr::GenerateTextureMipMaps(Core::HTexture hTex)
{
	Texture **ppTex=m_Textures.Dereference(hTex);

	if(ppTex && *ppTex)
		return (*ppTex)->GenerateMipMaps();
	else
		return false;
}

bool TextureMgr::FillTexture(HTexture hTex, void *pBuffer, int iWidth, int iHeight, int iBPP)
{
	Texture **ppTex=m_Textures.Dereference(hTex);

	if(ppTex && *ppTex)
	{
		bool bRet=(*ppTex)->Fill(pBuffer,iWidth,iHeight,iBPP);
		if(!bRet)
			g_Output->AddMsg("x Erreur lors du remplissage de la texture %s\n",(*ppTex)->GetName());
		return bRet;
	}
	else
		return false;
}

bool TextureMgr::BeginRestart()
{
	bool bOK=true;
	NameIndex::iterator i;
	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Texture *pTex=*m_Textures.Dereference(i->second);
		bOK&=pTex->BeginRestart();
	}

	return bOK;
}

bool TextureMgr::EndRestart(IDirect3DDevice9 *pDevice)
{
	m_pDevice=pDevice;
	bool bOK=true;
	NameIndex::iterator i;

	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Texture *pTex=*m_Textures.Dereference(i->second);
		bool b=pTex->EndRestart(pDevice);

		if(!b)
			g_Output->AddMsg("x Erreur lors du redémarrage de la texture %s\n",i->first.c_str());

		bOK&=b;
	}

	return bOK;
}

}
