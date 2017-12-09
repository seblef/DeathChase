
#include "ShaderMgr.h"
#include "Config.h"
#include "Renderer.h"

namespace Core
{

ShaderMgr::ShaderMgr(IDirect3DDevice9 *pDevice) : m_pDevice(pDevice)
{
	MYASSERT(pDevice,"D3D Device invalide",true);

	const char *szShaders=g_Config.GetSParameter("ShadersDetails","low");
	if(strcmp(szShaders,"medium")==0)
		Shader::SetShadersVersions(1);
	else if(strcmp(szShaders,"high")==0)
		Shader::SetShadersVersions(2);
	else
		Shader::SetShadersVersions(0);
}

ShaderMgr::~ShaderMgr()
{
	NameIndex::iterator i;
	
	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Shader *pShader=*m_Shaders.Dereference(i->second);

		if(pShader)
		{
			pShader->Release();
			delete pShader;
		}
	}
}

HShader ShaderMgr::LoadShader(const char *szName)
{
	if(!szName) return HShader();

	std::string strName(szName);
	NameIndex::iterator rc=m_NameIndex.find(strName);
	HShader hShader;

	if(rc==m_NameIndex.end())
	{
		Shader **pShader=m_Shaders.Acquire(hShader);
		*pShader=new Shader(m_pDevice);
		
		if(!(*pShader)->Load(szName))
		{
			delete *pShader;
			m_Shaders.Release(hShader);
			hShader=HShader();
			g_Output->AddMsg("x Erreur lors du chargement du shader %s\n",szName);
		}
		else
			m_NameIndex.insert(std::make_pair(strName,hShader));
	}
	else
	{
		hShader=rc->second;
		Shader *pShader=*m_Shaders.Dereference(hShader);
		pShader->AddRef();
	}
	
	return hShader;
}

void ShaderMgr::ReleaseShader(HShader &hShader)
{
	Shader **pShader=m_Shaders.Dereference(hShader);
	
	if(pShader && *pShader)
	{
		if((*pShader)->RemRef() <= 0)
		{
			NameIndex::iterator i=m_NameIndex.find(std::string((*pShader)->GetName()));
			if(i!=m_NameIndex.end())
				m_NameIndex.erase(i);

			(*pShader)->Release();
			delete *pShader;
			m_Shaders.Release(hShader);
		}
	}

	hShader=HShader();
}

bool ShaderMgr::SetShader(HShader hShader)
{
	if(hShader==m_hLastShader)
		return true;

	Shader **ppShader=m_Shaders.Dereference(hShader);

	if(ppShader && *ppShader)
	{
		g_Renderer.SetShader(*ppShader);
		m_hLastShader=hShader;
	}
	else
		g_Output->AddMsg("! Failed setting shader %i,%i\n",hShader.GetIndex(),hShader.GetMagic());

	return false;
}

IShader *ShaderMgr::GetShader(HShader hShader)
{
	Shader **ppShader=m_Shaders.Dereference(hShader);
	if(ppShader)
		return (IShader *)*ppShader;
	else
		return 0;
}

bool ShaderMgr::BeginRestart()
{
	bool bOK=true;

	NameIndex::iterator i;
	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Shader *pShader=*m_Shaders.Dereference(i->second);
		bOK&=pShader->BeginRestart();
	}

	return bOK;
}

bool ShaderMgr::EndRestart(IDirect3DDevice9 *pDevice)
{
	const char *szShaders=g_Config.GetSParameter("ShadersDetails","low");
	if(strcmp(szShaders,"medium")==0)
		Shader::SetShadersVersions(1);
	else if(strcmp(szShaders,"high")==0)
		Shader::SetShadersVersions(2);
	else
		Shader::SetShadersVersions(0);

	m_pDevice=pDevice;
	bool bOK=true;

	NameIndex::iterator i;
	for(i=m_NameIndex.begin();i!=m_NameIndex.end();i++)
	{
		Shader *pShader=*m_Shaders.Dereference(i->second);
		bOK&=pShader->EndRestart(pDevice);
	}

	return bOK;
}

}