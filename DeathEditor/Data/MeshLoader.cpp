
#include "MeshLoader.h"
#include "../../../Loaders/SDK/Iloader.h"
#include "../FileList.h"
#include "../FileSystem.h"
#include "../Interfaces.h"
#include "../../DeathEngine/Export/IMesh.h"
#include "../../DeathEngine/Export/IMaterialMgr.h"
#include "../../DeathEngine/Export/IMaterial.h"
#include "../../DeathEngine/Export/IShaderMgr.h"
#include "../../DeathEngine/Export/ITextureMgr.h"

#define SMAP_SPECULAR					(1)
#define SMAP_EMISSIVE					(1 << 1)
#define SMAP_LIGHTED					(1 << 2)

#define SMAP_TEXBASE					(1 << 3)
#define SMAP_TEXSECADD					(1 << 4)
#define SMAP_TEXSECFILTER				(1 << 5)
#define SMAP_TEXSECFILTER2X				(1 << 6)

#define SMAP_BUMP						(1 << 7)
#define SMAP_ENVIRONMENT				(1 << 8)


typedef bool (*LoaderFnc)(ILoader **);

bool MeshLoader::Init()
{
	Release();

	g_FileSystem.ChangeWorkingDirectory("Loaders\\");
	FileList list;

	std::vector<std::string> filters;
	filters.push_back(".dll");
	list.Reset(&filters,true);

	const int iCount=list.GetFileCount();
	if(iCount==0)
	{
		g_Output2->AddMsg("o Aucun loader trouvé.\n");
		g_FileSystem.ResetWorkingDirectory();
		return true;
	}

	std::string strTemp;
	std::string strExt;

	for(int i=0;i<iCount;i++)
	{
		g_Output2->AddMsg("! Chargement du loader %s.\n",list.GetFileName(i));

		Loader ld;
		ld.hDLL=::LoadLibrary(list.GetFullFileName(i));

		if(ld.hDLL)
		{
			LoaderFnc GetLoader=(LoaderFnc)::GetProcAddress(ld.hDLL,"CreateLoader");
			if(GetLoader)
			{
				if(GetLoader(&ld.pLoader))
				{
					m_Loaders.push_back(ld);

					const int iExtCount=ld.pLoader->GetExtensionCount();
					for(int j=0;j<iExtCount;j++)
					{
						strExt=ld.pLoader->GetExtension(j);

						strTemp="*.";
						strTemp+=strExt;
						
						m_Filters.push_back(strTemp);

						m_strExtensions+=strExt;
						m_strExtensions+=" file (*.";
						m_strExtensions+=strExt;
						m_strExtensions+=")|*.";
						m_strExtensions+=strExt;
						m_strExtensions+="|";

						if(i > 0)
							m_strExtPath+="; *.";
						else
							m_strExtPath="*.";

						m_strExtPath+=strExt;
					}
				}
				else
					g_Output2->AddMsg("x Erreur à la création de l'interface ILoader.\n");
			}
			else
				g_Output2->AddMsg("x Erreur: La fonction CreateLoader n'a pas été trouvée.\n");
		}
		else
			g_Output2->AddMsg("x Erreur au chargement de la dll.\n");
	}

	if(m_strExtensions.length() > 0)
		m_strExtensions+="|";

	g_FileSystem.ResetWorkingDirectory();

	return true;
}

bool MeshLoader::Release()
{
	std::vector<Loader>::iterator i;
	for(i=m_Loaders.begin();i!=m_Loaders.end();i++)
	{
		if(i->hDLL)
		{
			LoaderFnc ReleaseLoader=(LoaderFnc)::GetProcAddress(i->hDLL,"DestroyLoader");
			if(ReleaseLoader)
				ReleaseLoader(&i->pLoader);

			::FreeLibrary(i->hDLL);
		}
	}

	m_Loaders.clear();
	m_Filters.clear();

	m_strExtensions="";
	m_strExtPath="\\";

	return true;
}

ILoader *MeshLoader::LoadMesh(const char *szFileName, const char *szPath, const char *szExt)
{
	if(!szFileName || m_Loaders.size()==0) return 0;

	std::string strFileName;
	if(szPath)
	{
		strFileName=szPath;
		strFileName+=szFileName;
	}
	else
		strFileName=szFileName;

	std::string strExt;
	std::string strExtension(szExt);
	ILoader *pLoader=0;

	std::vector<Loader>::iterator i;
	for(i=m_Loaders.begin();i!=m_Loaders.end();i++)
	{
		for(int j=0;j<i->pLoader->GetExtensionCount();j++)
		{
			strExt=i->pLoader->GetExtension(j);
			if(stricmp(strExt.c_str(),strExtension.c_str())==0)
			{
				pLoader=i->pLoader;
				break;
				break;
			}
		}
	}

	if(pLoader->Load(szFileName,szPath,true))
		return pLoader;
	else
		return 0;
}

void MeshLoader::ConvertMaterial(IMaterial *pMat, LMaterial *mat,
								 const char *szTexPath)
{
	uint32 dwFlags=0;

	pMat->SetDiffuseColor(mat->m_cDiffuseColor);

	if(mat->m_cEmissiveColor.r > 0 ||
	   mat->m_cEmissiveColor.g > 0 ||
	   mat->m_cEmissiveColor.b > 0)
	{
		pMat->SetEmissiveColor(mat->m_cEmissiveColor);
	   dwFlags|=SMAP_EMISSIVE;
	}

	if(mat->m_fShininess!=0.0f)
	{
		pMat->SetShininess(mat->m_fShininess);
		pMat->SetSpecularColor(mat->m_cSpecularColor);
		dwFlags|=SMAP_SPECULAR;
	}

	if(mat->m_bAlphaEnable)
	{
		if(strcmp(mat->m_szSrcBlend,"one")==0)
			pMat->SetSrcBlend(D3DBLEND_ONE);
		else if(strcmp(mat->m_szSrcBlend,"zero")==0)
			pMat->SetSrcBlend(D3DBLEND_ZERO);
		else if(strcmp(mat->m_szSrcBlend,"src_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_SRCALPHA);
		else if(strcmp(mat->m_szSrcBlend,"one_minus_src_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_INVSRCALPHA);
		else if(strcmp(mat->m_szSrcBlend,"dest_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_DESTALPHA);
		else if(strcmp(mat->m_szSrcBlend,"one_minue_dest_alpha")==0)
			pMat->SetSrcBlend(D3DBLEND_INVDESTALPHA);
		else if(strcmp(mat->m_szSrcBlend,"src_color")==0)
			pMat->SetSrcBlend(D3DBLEND_SRCCOLOR);
		else if(strcmp(mat->m_szSrcBlend,"one_minus_src_solor")==0)
			pMat->SetSrcBlend(D3DBLEND_INVSRCCOLOR);
		else if(strcmp(mat->m_szSrcBlend,"dest_color")==0)
			pMat->SetSrcBlend(D3DBLEND_DESTCOLOR);
		else if(strcmp(mat->m_szSrcBlend,"one_minus_dest_color")==0)
			pMat->SetSrcBlend(D3DBLEND_INVDESTCOLOR);
		else
			pMat->SetSrcBlend(D3DBLEND_SRCALPHA);

		if(strcmp(mat->m_szDestBlend,"one")==0)
			pMat->SetDstBlend(D3DBLEND_ONE);
		else if(strcmp(mat->m_szDestBlend,"zero")==0)
			pMat->SetDstBlend(D3DBLEND_ZERO);
		else if(strcmp(mat->m_szDestBlend,"src_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_SRCALPHA);
		else if(strcmp(mat->m_szDestBlend,"one_minus_src_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_INVSRCALPHA);
		else if(strcmp(mat->m_szDestBlend,"dest_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_DESTALPHA);
		else if(strcmp(mat->m_szDestBlend,"one_minue_dest_alpha")==0)
			pMat->SetDstBlend(D3DBLEND_INVDESTALPHA);
		else if(strcmp(mat->m_szDestBlend,"src_color")==0)
			pMat->SetDstBlend(D3DBLEND_SRCCOLOR);
		else if(strcmp(mat->m_szDestBlend,"one_minus_src_solor")==0)
			pMat->SetDstBlend(D3DBLEND_INVSRCCOLOR);
		else if(strcmp(mat->m_szDestBlend,"dest_color")==0)
			pMat->SetDstBlend(D3DBLEND_DESTCOLOR);
		else if(strcmp(mat->m_szDestBlend,"one_minus_dest_color")==0)
			pMat->SetDstBlend(D3DBLEND_INVDESTCOLOR);
		else
			pMat->SetDstBlend(D3DBLEND_SRCALPHA);
	}

	if(strcmp(mat->m_szDepthFunction,"never")==0)
		pMat->SetDepthFnc(D3DCMP_NEVER);
	else if(strcmp(mat->m_szDepthFunction,"always")==0)
		pMat->SetDepthFnc(D3DCMP_ALWAYS);
	else if(strcmp(mat->m_szDepthFunction,"less")==0)
		pMat->SetDepthFnc(D3DCMP_LESS);
	else if(strcmp(mat->m_szDepthFunction,"lequal")==0)
		pMat->SetDepthFnc(D3DCMP_LESSEQUAL);
	else if(strcmp(mat->m_szDepthFunction,"equal")==0)
		pMat->SetDepthFnc(D3DCMP_EQUAL);
	else if(strcmp(mat->m_szDepthFunction,"nequal")==0)
		pMat->SetDepthFnc(D3DCMP_NOTEQUAL);
	else if(strcmp(mat->m_szDepthFunction,"gequal")==0)
		pMat->SetDepthFnc(D3DCMP_GREATEREQUAL);
	else if(strcmp(mat->m_szDepthFunction,"greater")==0)
		pMat->SetDepthFnc(D3DCMP_GREATER);
	else
		pMat->SetDepthFnc(D3DCMP_LESSEQUAL);

	if(strcmp(mat->m_szStencilFunction,"never")==0)
		pMat->SetStencilFnc(D3DCMP_NEVER);
	else if(strcmp(mat->m_szStencilFunction,"always")==0)
		pMat->SetStencilFnc(D3DCMP_ALWAYS);
	else if(strcmp(mat->m_szStencilFunction,"less")==0)
		pMat->SetStencilFnc(D3DCMP_LESS);
	else if(strcmp(mat->m_szStencilFunction,"lequal")==0)
		pMat->SetStencilFnc(D3DCMP_LESSEQUAL);
	else if(strcmp(mat->m_szStencilFunction,"equal")==0)
		pMat->SetStencilFnc(D3DCMP_EQUAL);
	else if(strcmp(mat->m_szStencilFunction,"nequal")==0)
		pMat->SetStencilFnc(D3DCMP_NOTEQUAL);
	else if(strcmp(mat->m_szStencilFunction,"gequal")==0)
		pMat->SetStencilFnc(D3DCMP_GREATEREQUAL);
	else if(strcmp(mat->m_szStencilFunction,"greater")==0)
		pMat->SetStencilFnc(D3DCMP_GREATER);
	else
		pMat->SetStencilFnc(D3DCMP_LESSEQUAL);

	pMat->SetZWriteEnable(mat->m_bZWriteEnable ? TRUE : FALSE);
	
	if(strcmp(mat->m_szCullMode,"none")==0)
		pMat->SetCullMode(D3DCULL_NONE);
	else if(strcmp(mat->m_szCullMode,"front")==0)
		pMat->SetCullMode(D3DCULL_CW);
	else
		pMat->SetCullMode(D3DCULL_CCW);

	pMat->SetCastShadows(mat->m_bCastShadows);
	pMat->SetReceiveShadows(mat->m_bReceiveShadows);

	int iCurrentStage=0;
	if(strlen(mat->m_szBaseTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szBaseTexture,szTexPath);
		if(hTex.IsNull())
			MessageBox(0,"Erreur au chargement de la texture de base.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			dwFlags|=SMAP_TEXBASE;
			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szLightTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szLightTexture,szTexPath);
		if(hTex.IsNull())
			MessageBox(0,"Erreur au chargement de la texture de lumière.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			if(iCurrentStage==0)
				dwFlags|=SMAP_TEXBASE;
			else
				dwFlags|=SMAP_TEXSECFILTER;

			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szBumpTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szBumpTexture,szTexPath);
		if(hTex.IsNull())
			MessageBox(0,"Erreur au chargement de la texture de bump.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			dwFlags|=SMAP_BUMP;
			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szEnvironmentTexture)!=0)
	{
		Core::HTexture hTex=g_TextureMgr->LoadTexture(mat->m_szEnvironmentTexture,szTexPath);
		if(hTex.IsNull())
			MessageBox(0,"Erreur au chargement de la texture de base.","Erreur",MB_OK | MB_ICONERROR);
		else
		{
			pMat->AddTexture(hTex,iCurrentStage);
			dwFlags|=SMAP_ENVIRONMENT;
			iCurrentStage++;
		}
	}

	if(strlen(mat->m_szShaderName)!=0)
		pMat->SetShader(RENDERINGPASS_SPECIAL,g_ShaderMgr->LoadShader(mat->m_szShaderName));
	else
	{
		char szAmbientShader[32];
		char szSunShader[32];

		if(dwFlags & SMAP_TEXBASE)
		{
//			if(pMat->IsEmissiveEnable())
//				strcpy(szAmbientShader,"StdAmbient_EmissTex");
//			else
				strcpy(szAmbientShader,"StdAmbient_Tex");

			strcpy(szSunShader,"StdSun_Tex");
		}
		else
		{
//			if(pMat->IsEmissiveEnable())
//				strcpy(szAmbientShader,"StdAmbient_EmissNoTex");
//			else
				strcpy(szAmbientShader,"StdAmbient_NoTex");

			strcpy(szSunShader,"StdSun_NoTex");
		}

		pMat->SetShader(RENDERINGPASS_AMBIENT,g_ShaderMgr->LoadShader(szAmbientShader));
		pMat->SetShader(RENDERINGPASS_SUN,g_ShaderMgr->LoadShader(szSunShader));
	}
}

