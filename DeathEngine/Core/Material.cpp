
#include "MaterialInstance.h"
#include "Material.h"

#include "ShaderMgr.h"
#include "TextureMgr.h"
#include "ScriptFile.h"
#include "FileSystem.h"

#include "Renderer.h"

#include "../ConvertChar.h"

#define MATCOL_NONE						0
#define MATCOL_DIFFUSEENABLE			1
#define MATCOL_SPECULARENABLE			2
#define MATCOL_EMISSIVEENABLE			4

const float g_fColorTrans=1.0f / 255.0f;

namespace Core
{

int Material::m_siRenderingPass=0;

MaterialInstance::MaterialInstance(Material *pMat, uint32 dwAnimTime) : 
	m_pMaterial(pMat),m_dwAnimTime(dwAnimTime)
{
	MYASSERT(pMat,"Impossible de créer une instance sur un matériau null",true);
	m_dwStartTime=timeGetTime();
}

void MaterialInstance::Reset(Material *pMat, uint32 dwAnimTime)
{
	MYASSERT(pMat,"Impossible de créer une instance sur un matériau null",true);
	m_dwStartTime=timeGetTime();
	m_pMaterial=pMat;
	m_dwAnimTime=dwAnimTime;
}

bool MaterialInstance::Set()
{
	const uint32 dwNow=timeGetTime();
	uint32 dwTime=dwNow - m_dwStartTime;
	if(dwTime > m_dwAnimTime)
	{
		while(dwTime > m_dwAnimTime)
			dwTime-=m_dwAnimTime;

		m_dwStartTime=dwNow - dwTime;
	}

	return m_pMaterial->Set(dwTime);
}



Material::Material(IDirect3DDevice9 *pDevice) : m_pDevice(pDevice),
	m_bMustRecompile(true),
	m_cDiffuse(1.0f,0.f,0.f,0.f),
	m_cSpecular(1.0f,0.f,0.f,0.f),
	m_cEmissive(1.0f,0.f,0.f,0.f),
	m_fShininess(1.0f),
	m_dwColorFlags(0),
	m_bBlend(FALSE),
	m_SrcBlend(D3DBLEND_ONE),
	m_DstBlend(D3DBLEND_ONE),
	m_DepthFnc(D3DCMP_LESSEQUAL),
	m_StencilFnc(D3DCMP_ALWAYS),
	m_CullMode(D3DCULL_CCW),
	m_bNoDraw(false),
	m_bCastShadows(true),
	m_bReceiveShadows(true),
	m_bAnimated(false),
	m_dwMaxAnimTime(0),
	m_bZWriteEnable(TRUE)
{
	strcpy(m_szName,"DefaultMaterial");

	for(int i=0;i<MAX_TEXTURE_STAGES;i++)
	{
		m_fFrequency[i]=1.0f;
		m_dwAnimTimes[i]=0;
	}
}

Material::~Material()
{
	Release();
}

void Material::Release()
{
	for(int i=0;i<RENDERINGPASS_COUNT;i++)
		g_ShaderMgr.ReleaseShader(m_hShader[i]);

	for(int i=0;i<MAX_TEXTURE_STAGES;i++)
	{
		if(m_Stages[i].size() > 0)
		{
			TextureVector::iterator j;
			for(j=m_Stages[i].begin();j!=m_Stages[i].end();j++)
				g_TextureMgr.ReleaseTexture(*j);
		}

		m_Stages[i].clear();
	}
}

bool Material::Create(const char *szName)
{
	MYASSERT(szName,"Nom de materiau null",true);

	strcpy(m_szName,szName);
	return true;
}

void Material::SetName(const char *szName)
{
	MYASSERT(szName,"Nom de materiau null",true);
	strcpy(m_szName,szName);
}

HTexture Material::GetTexture(const int iStage, const int iAnimNum)
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);

	if(iAnimNum < m_Stages[iStage].size())
		return m_Stages[iStage][iAnimNum];
	else
		return HTexture();
}

void Material::SetTexture(HTexture hTex, const int iStage, const int iAnimNum)
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);

	if(iAnimNum < m_Stages[iStage].size())
		m_Stages[iStage][iAnimNum]=hTex;
}

void Material::AddTexture(Core::HTexture hTex, const int iStage)
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);

	m_Stages[iStage].push_back(hTex);
	if(m_Stages[iStage].size() > 1)
		m_bAnimated=true;
}

void Material::RemoveTexture(const int iStage, const int iAnimNum)
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);

	if(iAnimNum < m_Stages[iStage].size())
	{
		g_TextureMgr.ReleaseTexture(m_Stages[iStage][iAnimNum]);
		m_Stages[iStage].erase(m_Stages[iStage].begin() + iAnimNum);
	}
}

float Material::GetTextureAnimFrequency(const int iStage) const
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);
	return m_fFrequency[iStage];
}

uint32 Material::GetTextureAnimTime(const int iStage) const
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);
	return m_dwAnimTimes[iStage];
}

void Material::SetTextureAnimFrequency(const float fFreq, const int iStage)
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);
	m_fFrequency[iStage]=fFreq==0.0f ? 0.001f : fFreq;
	m_dwAnimTimes[iStage]=(uint32)(1000.0f / m_fFrequency[iStage]);
}

void Material::SetTextureAnimTime(const uint32 dwTime, const int iStage)
{
	MYASSERT(iStage >= 0 && iStage < MAX_TEXTURE_STAGES,"Etage de texture en dehors des limites [0..MAX_TEXTURE_STAGES-1]",true);
	m_dwAnimTimes[iStage]=dwTime;
	if(dwTime!=0)
		m_fFrequency[iStage]=1000.0f / ((float)dwTime);
}

void Material::SetAlphaBlendEnable(const BOOL bBlend)
{
	m_bBlend=bBlend;
	m_bMustRecompile=true;
}

void Material::SetSrcBlend(D3DBLEND SrcBlend)
{
	m_SrcBlend=SrcBlend;
	m_bBlend=TRUE;
	m_bMustRecompile=true;
}

void Material::SetDstBlend(D3DBLEND DstBlend)
{
	m_DstBlend=DstBlend;
	m_bBlend=TRUE;
	m_bMustRecompile=true;
}

void Material::SetDepthFnc(D3DCMPFUNC DepthFnc)
{
	m_DepthFnc=DepthFnc;
	m_bMustRecompile=true;
}

void Material::SetStencilFnc(D3DCMPFUNC StencilFnc)
{
	m_StencilFnc=StencilFnc;
	m_bMustRecompile=true;
}

void Material::SetCullMode(D3DCULL CullMode)
{
	m_CullMode=CullMode;
	m_bMustRecompile=true;
}

void Material::SetDiffuseColor(const Color32f& cDiffuse)
{
	m_cDiffuse=cDiffuse;
	m_bMustRecompile=true;
	m_dwColorFlags|=MATCOL_DIFFUSEENABLE;
}

void Material::SetSpecularColor(const Color32f& cSpecular)
{
	m_cSpecular=cSpecular;
	m_bMustRecompile=true;
	m_dwColorFlags|=MATCOL_SPECULARENABLE;
}

void Material::SetEmissiveColor(const Color32f& cEmissive)
{
	m_cEmissive=cEmissive;
	m_bMustRecompile=true;
	m_dwColorFlags|=MATCOL_EMISSIVEENABLE;
}

bool Material::IsDiffuseEnable() const
{
	return (m_dwColorFlags & MATCOL_DIFFUSEENABLE)!=0;
}

bool Material::IsSpecularEnable() const
{
	return (m_dwColorFlags & MATCOL_SPECULARENABLE)!=0;
}

bool Material::IsEmissiveEnable() const
{
	return (m_dwColorFlags & MATCOL_EMISSIVEENABLE)!=0;
}

void Material::DisableDiffuse()
{
	m_dwColorFlags&=(MATCOL_SPECULARENABLE | MATCOL_EMISSIVEENABLE);
	m_bMustRecompile=true;
}

void Material::DisableSpecular()
{
	m_dwColorFlags&=(MATCOL_DIFFUSEENABLE | MATCOL_EMISSIVEENABLE);
	m_bMustRecompile=true;
}

void Material::DisableEmissive()
{
	m_dwColorFlags&=(MATCOL_DIFFUSEENABLE | MATCOL_SPECULARENABLE);
	m_bMustRecompile=true;
}

void Material::SetShininess(const float fShininess)
{
	m_fShininess=fShininess;
	m_bMustRecompile=true;
}

bool Material::Load(const char *szFileName, const char *szPath)
{
	char szTemp[1024];
	if(szPath)
		sprintf(szTemp,"%s%s",szPath,szFileName);
	else
		sprintf(szTemp,"Materials\\%s",szFileName);

	ScriptFile file; // file.Close() s'appelle à la destruction de file, aucun appel ici
	if(!file.Open(szTemp))
		return false;

	const char *szToken=file.GetToken();

	if(!szToken)
		return false;

	if(strcmp(szToken,"material")==0)
	{
		szToken=file.GetToken();
		if(szToken)
			strcpy(m_szName,szToken);
		else
		{
			g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [material]\n",szFileName);
			return false;
		}
	}
	else
	{
		g_Output->AddMsg("x Erreur: Les fichiers materiau doivent commencer par:\n material [nom]\n{\n (%s)\n",szFileName);
		return false;
	}

	szToken=file.GetToken();
	if(!szToken)
	{
		g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [material]\n",szFileName);
		return false;
	}
	else if(strcmp(szToken,"{")!=0)
	{
		g_Output->AddMsg("x Erreur: Les fichiers materiau doivent commencer par:\n material [nom]\n{\n (%s)\n",szFileName);
		return false;
	}

	szToken=file.GetToken();
	while(szToken)
	{
		if(strcmp(szToken,"}")==0)
			break;
		else if(strcmp(szToken,"nodraw")==0)
			m_bNoDraw=true;
		else if(strcmp(szToken,"castnoshadows")==0)
			m_bCastShadows=false;
		else if(strcmp(szToken,"receivenoshadows")==0)
			m_bReceiveShadows=false;
		else if(strcmp(szToken,"animated")==0)
			m_bAnimated=true;
		else if(strcmp(szToken,"ambient_shader")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				const char *szDir=g_FileSystem.GetWorkingDirectory();
				g_FileSystem.ResetWorkingDirectory();
				m_hShader[RENDERINGPASS_AMBIENT]=g_ShaderMgr.LoadShader(szToken);
				g_FileSystem.ChangeWorkingDirectory(szDir);
				if(m_hShader[RENDERINGPASS_AMBIENT].IsNull())
					g_Output->AddMsg("x Erreur [%s]: Le shader %s n'existe pas ou est corrompu.",szFileName,szToken);
			}
			else
			{
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [shader].\n",szFileName);
				break;
			}
		}
		else if(strcmp(szToken,"sun_shader")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				const char *szDir=g_FileSystem.GetWorkingDirectory();
				g_FileSystem.ResetWorkingDirectory();
				m_hShader[RENDERINGPASS_SUN]=g_ShaderMgr.LoadShader(szToken);
				g_FileSystem.ChangeWorkingDirectory(szDir);
				if(m_hShader[RENDERINGPASS_SUN].IsNull())
					g_Output->AddMsg("x Erreur [%s]: Le shader %s n'existe pas ou est corrompu.",szFileName,szToken);
			}
			else
			{
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [shader].\n",szFileName);
				break;
			}
		}
		else if(strcmp(szToken,"light_shader")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				const char *szDir=g_FileSystem.GetWorkingDirectory();
				g_FileSystem.ResetWorkingDirectory();
				m_hShader[RENDERINGPASS_LIGHT]=g_ShaderMgr.LoadShader(szToken);
				g_FileSystem.ChangeWorkingDirectory(szDir);
				if(m_hShader[RENDERINGPASS_LIGHT].IsNull())
					g_Output->AddMsg("x Erreur [%s]: Le shader %s n'existe pas ou est corrompu.",szFileName,szToken);
			}
			else
			{
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [shader].\n",szFileName);
				break;
			}
		}
		else if(strcmp(szToken,"special_shader")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				const char *szDir=g_FileSystem.GetWorkingDirectory();
				g_FileSystem.ResetWorkingDirectory();
				m_hShader[RENDERINGPASS_SPECIAL]=g_ShaderMgr.LoadShader(szToken);
				g_FileSystem.ChangeWorkingDirectory(szDir);
				if(m_hShader[RENDERINGPASS_SPECIAL].IsNull())
					g_Output->AddMsg("x Erreur [%s]: Le shader %s n'existe pas ou est corrompu.",szFileName,szToken);
			}
			else
			{
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [shader].\n",szFileName);
				break;
			}
		}
		else if(strcmp(szToken,"maps")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				const int iStage=atoi(szToken);
				if(iStage < 0 || iStage >= MAX_TEXTURE_STAGES)
					g_Output->AddMsg("x Erreur [%s]: L'etage de texture doit etre compris entre 0 et %i.\n",szFileName,MAX_TEXTURE_STAGES);
				else
				{
					szToken=file.GetToken();
					while(szToken && strcmp(szToken,"-")!=0)
					{
						HTexture hTex=g_TextureMgr.LoadTexture(szToken,"");
						if(hTex.IsNull())
							g_Output->AddMsg("x Erreur [%s]: La texture %s n'existe pas ou est corrompue.\n",szFileName,szToken);
						else
							AddTexture(hTex,iStage);

						szToken=file.GetToken();
					}

					if(!szToken)
						g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [map]\n",szFileName);
					else if(m_Stages[iStage].size() > 1)
					{
						szToken=file.GetToken();
						if(szToken)
							SetTextureAnimFrequency(atof(szToken),iStage);
						else
							g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [map]\n",szFileName);
					}
				}
			}
		}
		else if(strcmp(szToken,"blend")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				if(strcmp(szToken,"add")==0)
				{
					SetSrcBlend(D3DBLEND_ONE);
					SetDstBlend(D3DBLEND_ONE);
				}
				else if(strcmp(szToken,"filter")==0)
				{
					SetSrcBlend(D3DBLEND_SRCCOLOR);
					SetDstBlend(D3DBLEND_INVSRCCOLOR);
				}
				else if(strcmp(szToken,"blend")==0)
				{
					SetSrcBlend(D3DBLEND_SRCALPHA);
					SetDstBlend(D3DBLEND_INVSRCALPHA);
				}
				else
				{
					if(strcmp(szToken,"one")==0)
						SetSrcBlend(D3DBLEND_ONE);
					else if(strcmp(szToken,"zero")==0)
						SetSrcBlend(D3DBLEND_ZERO);
					else if(strcmp(szToken,"src_color")==0)
						SetSrcBlend(D3DBLEND_SRCCOLOR);
					else if(strcmp(szToken,"one_minus_src_color")==0)
						SetSrcBlend(D3DBLEND_INVSRCCOLOR);
					else if(strcmp(szToken,"src_alpha")==0)
						SetSrcBlend(D3DBLEND_SRCALPHA);
					else if(strcmp(szToken,"one_minus_src_alpha")==0)
						SetSrcBlend(D3DBLEND_INVSRCALPHA);
					else if(strcmp(szToken,"dest_color")==0)
						SetSrcBlend(D3DBLEND_DESTCOLOR);
					else if(strcmp(szToken,"one_minus_dest_color")==0)
						SetSrcBlend(D3DBLEND_INVDESTCOLOR);
					else if(strcmp(szToken,"dest_alpha")==0)
						SetSrcBlend(D3DBLEND_DESTALPHA);
					else if(strcmp(szToken,"one_minue_dest_alpha")==0)
						SetSrcBlend(D3DBLEND_INVDESTALPHA);
					else
						g_Output->AddMsg("x Erreur [%s]: Token [%s] inconnu apres la commande blend.\n",szFileName,szToken);

					szToken=file.GetToken();
					if(szToken)
					{
						if(strcmp(szToken,"one")==0)
							SetDstBlend(D3DBLEND_ONE);
						else if(strcmp(szToken,"zero")==0)
							SetDstBlend(D3DBLEND_ZERO);
						else if(strcmp(szToken,"src_color")==0)
							SetDstBlend(D3DBLEND_SRCCOLOR);
						else if(strcmp(szToken,"one_minus_src_color")==0)
							SetDstBlend(D3DBLEND_INVSRCCOLOR);
						else if(strcmp(szToken,"src_alpha")==0)
							SetDstBlend(D3DBLEND_SRCALPHA);
						else if(strcmp(szToken,"one_minus_src_alpha")==0)
							SetDstBlend(D3DBLEND_INVSRCALPHA);
						else if(strcmp(szToken,"dest_color")==0)
							SetDstBlend(D3DBLEND_DESTCOLOR);
						else if(strcmp(szToken,"one_minus_dest_color")==0)
							SetDstBlend(D3DBLEND_INVDESTCOLOR);
						else if(strcmp(szToken,"dest_alpha")==0)
							SetDstBlend(D3DBLEND_DESTALPHA);
						else if(strcmp(szToken,"one_minue_dest_alpha")==0)
							SetDstBlend(D3DBLEND_INVDESTALPHA);
						else
							g_Output->AddMsg("x Erreur [%s]: Token [%s] inconnu apres la commande blend.\n",szFileName,szToken);
					}
					else
						g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [blend]\n",szFileName);
				}
			}
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [blend]\n",szFileName);
		}
		else if(strcmp(szToken,"nozwrite")==0)
			m_bZWriteEnable=FALSE;
		else if(strcmp(szToken,"depth")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				if(strcmp(szToken,"always")==0)
					SetDepthFnc(D3DCMP_ALWAYS);
				else if(strcmp(szToken,"lequal")==0)
					SetDepthFnc(D3DCMP_LESSEQUAL);
				else if(strcmp(szToken,"less")==0)
					SetDepthFnc(D3DCMP_LESS);
				else if(strcmp(szToken,"equal")==0)
					SetDepthFnc(D3DCMP_EQUAL);
				else if(strcmp(szToken,"nequal")==0)
					SetDepthFnc(D3DCMP_NOTEQUAL);
				else if(strcmp(szToken,"greater")==0)
					SetDepthFnc(D3DCMP_GREATER);
				else if(strcmp(szToken,"gequal")==0)
					SetDepthFnc(D3DCMP_GREATEREQUAL);
				else if(strcmp(szToken,"never")==0)
					SetDepthFnc(D3DCMP_NEVER);
				else
					g_Output->AddMsg("x Erreur [%s]: Token [%s] inconnu apres la commande depth.\n",szFileName,szToken);
			}
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [depth]\n",szFileName);
		}
		else if(strcmp(szToken,"stencil")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				if(strcmp(szToken,"always")==0)
					SetStencilFnc(D3DCMP_ALWAYS);
				else if(strcmp(szToken,"lequal")==0)
					SetStencilFnc(D3DCMP_LESSEQUAL);
				else if(strcmp(szToken,"less")==0)
					SetStencilFnc(D3DCMP_LESS);
				else if(strcmp(szToken,"equal")==0)
					SetStencilFnc(D3DCMP_EQUAL);
				else if(strcmp(szToken,"nequal")==0)
					SetStencilFnc(D3DCMP_NOTEQUAL);
				else if(strcmp(szToken,"greater")==0)
					SetStencilFnc(D3DCMP_GREATER);
				else if(strcmp(szToken,"gequal")==0)
					SetStencilFnc(D3DCMP_GREATEREQUAL);
				else if(strcmp(szToken,"never")==0)
					SetStencilFnc(D3DCMP_NEVER);
				else
					g_Output->AddMsg("x Erreur [%s]: Token [%s] inconnu apres la commande stencil.\n",szFileName,szToken);
			}
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [stencil]\n",szFileName);
		}
		else if(strcmp(szToken,"cull")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				if(strcmp(szToken,"front")==0)
					SetCullMode(D3DCULL_CW);
				else if(strcmp(szToken,"back")==0)
					SetCullMode(D3DCULL_CCW);
				else if(strcmp(szToken,"none")==0)
					SetCullMode(D3DCULL_NONE);
				else
					g_Output->AddMsg("x Erreur [%s]: Token [%s] inconnu apres la commande cull.\n",szFileName,szToken);
			}
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [cull]\n",szFileName);
		}
		else if(strcmp(szToken,"diffuse")==0)
		{
			Color32 color;
			if(file.ParseColor(color))
				SetDiffuseColor(Color32f(color));
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [diffuse]\n",szFileName);
		}
		else if(strcmp(szToken,"specular")==0)
		{
			Color32 color;
			if(file.ParseColor(color))
				SetSpecularColor(Color32f(color));
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [specular]\n",szFileName);
		}
		else if(strcmp(szToken,"emissive")==0)
		{
			Color32 color;
			if(file.ParseColor(color))
				SetEmissiveColor(Color32f(color));
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [emissive]\n",szFileName);
		}
		else if(strcmp(szToken,"shininess")==0)
		{
			szToken=file.GetToken();
			if(szToken)
				SetShininess(atof(szToken));
			else
				g_Output->AddMsg("x Erreur [%s]: Fin de fichier inattendue apres la commande [shininess]\n",szFileName);
		}
		else
			g_Output->AddMsg("x Erreur [%s]: Commande [%s] inconnue.\n",szFileName,szToken);

		szToken=file.GetToken();
	}

	file.Close();

	return true;
}

bool Material::Save(const char *szFileName, const char *szPath, const bool bSaveTextures)
{
	MYASSERT(szFileName,"Nom de fichier null pour la sauvegarde du materiau",true);
	char szTemp[1024];

	if(szPath)
		sprintf(szTemp,"%s%s",szPath,szFileName);
	else
		sprintf(szTemp,"Materials\\%s",szFileName);

	FILE *pFile=fopen(szTemp,"w");
	if(!pFile) return false;

	fprintf(pFile,"\n-- Death Chase material\n\nmaterial %s\n{\n",m_szName);

	if(m_bNoDraw) fprintf(pFile,"	nodraw\n");
	if(!m_bCastShadows) fprintf(pFile,"	castnoshadows\n");
	if(!m_bReceiveShadows) fprintf(pFile,"	receivenoshadows\n");
	if(m_bAnimated) fprintf(pFile,"	animated\n");

	fprintf(pFile,"\n");

	const char *szPassShaderNames[]={"ambient_shader","sun_shader","light_shader","special_shader"};

	for(int i=0;i<RENDERINGPASS_COUNT;i++)
	{
		if(!m_hShader[i].IsNull())
		{
			Shader *pShader=(Shader *)g_ShaderMgr.GetShader(m_hShader[m_siRenderingPass]);
			if(pShader) fprintf(pFile,"	%s %s\n",szPassShaderNames[i],pShader->GetName());
		}
	}

	for(int i=0;i<MAX_TEXTURE_STAGES;i++)
		if(m_Stages[i].size() > 0)
		{
			bool bValid=false;
			int iCount=0;
			char szMapTemp[1024];
			sprintf(szMapTemp,"	maps %i ",i);

			TextureVector::iterator j;
			for(j=m_Stages[i].begin();j!=m_Stages[i].end();j++)
			{
				char szTextureName[64];
				if(g_TextureMgr.GetTextureName(*j,szTextureName))
				{
					strcat(szMapTemp,szTextureName);
					strcat(szMapTemp," ");
					bValid=true;
					iCount++;
				}
			}

			strcat(szMapTemp,"- ");

			if(iCount > 1)
				strcat(szMapTemp,FloatToChar(m_fFrequency[i]));

			if(bValid)
				fprintf(pFile,szMapTemp);
		}

	fprintf(pFile,"\n");

	if(m_bBlend)
	{
		fprintf(pFile,"	blend ");
		switch(m_SrcBlend)
		{
			case D3DBLEND_ONE: fprintf(pFile,"one "); break;
			case D3DBLEND_ZERO: fprintf(pFile,"zero "); break;
			case D3DBLEND_SRCCOLOR: fprintf(pFile,"src_color "); break;
			case D3DBLEND_INVSRCCOLOR: fprintf(pFile,"one_minus_src_color "); break;
			case D3DBLEND_SRCALPHA: fprintf(pFile,"src_alpha "); break;
			case D3DBLEND_INVSRCALPHA: fprintf(pFile,"one_minus_src_alpha "); break;
			case D3DBLEND_DESTCOLOR: fprintf(pFile,"dest_color "); break;
			case D3DBLEND_INVDESTCOLOR: fprintf(pFile,"one_minus_dest_color "); break;
			case D3DBLEND_DESTALPHA: fprintf(pFile,"dest_alpha "); break;
			case D3DBLEND_INVDESTALPHA: fprintf(pFile,"one_minus_dest_alpha "); break;
			default: fprintf(pFile,"one "); break;
		}

		switch(m_DstBlend)
		{
			case D3DBLEND_ONE: fprintf(pFile,"one\n"); break;
			case D3DBLEND_ZERO: fprintf(pFile,"zero\n"); break;
			case D3DBLEND_SRCCOLOR: fprintf(pFile,"src_color\n"); break;
			case D3DBLEND_INVSRCCOLOR: fprintf(pFile,"one_minus_src_color\n"); break;
			case D3DBLEND_SRCALPHA: fprintf(pFile,"src_alpha\n"); break;
			case D3DBLEND_INVSRCALPHA: fprintf(pFile,"one_minus_src_alpha\n"); break;
			case D3DBLEND_DESTCOLOR: fprintf(pFile,"dest_color\n"); break;
			case D3DBLEND_INVDESTCOLOR: fprintf(pFile,"one_minus_dest_color\n"); break;
			case D3DBLEND_DESTALPHA: fprintf(pFile,"dest_alpha\n"); break;
			case D3DBLEND_INVDESTALPHA: fprintf(pFile,"one_minus_dest_alpha\n"); break;
			default: fprintf(pFile,"one\n"); break;
		}
	}

	if(!m_bZWriteEnable)
		fprintf(pFile,"\n	nozwrite\n\n");

	if(m_DepthFnc!=D3DCMP_LESSEQUAL)
	{
		fprintf(pFile,"	depth ");
		switch(m_DepthFnc)
		{
			case D3DCMP_ALWAYS: fprintf(pFile,"always\n"); break;
			case D3DCMP_NEVER: fprintf(pFile,"never\n"); break;
			case D3DCMP_LESS: fprintf(pFile,"less\n"); break;
			case D3DCMP_EQUAL: fprintf(pFile,"equal\n"); break;
			case D3DCMP_NOTEQUAL: fprintf(pFile,"nequal\n"); break;
			case D3DCMP_GREATER: fprintf(pFile,"greater\n"); break;
			case D3DCMP_GREATEREQUAL: fprintf(pFile,"gequal\n"); break;
			default: fprintf(pFile,"lequal\n"); break;
		}
	}

	if(m_StencilFnc!=D3DCMP_ALWAYS)
	{
		fprintf(pFile,"	stencil ");
		switch(m_StencilFnc)
		{
			case D3DCMP_LESSEQUAL: fprintf(pFile,"lequal\n"); break;
			case D3DCMP_NEVER: fprintf(pFile,"never\n"); break;
			case D3DCMP_LESS: fprintf(pFile,"less\n"); break;
			case D3DCMP_EQUAL: fprintf(pFile,"equal\n"); break;
			case D3DCMP_NOTEQUAL: fprintf(pFile,"nequal\n"); break;
			case D3DCMP_GREATER: fprintf(pFile,"greater\n"); break;
			case D3DCMP_GREATEREQUAL: fprintf(pFile,"gequal\n"); break;
			default: fprintf(pFile,"always\n"); break;
		}
	}

	if(m_CullMode!=D3DCULL_CCW)
	{
		fprintf(pFile,"	cull ");
		switch(m_CullMode)
		{
			case D3DCULL_CW: fprintf(pFile,"front\n"); break;
			case D3DCULL_NONE: fprintf(pFile,"none\n"); break;
			default: fprintf(pFile,"back\n"); break;
		}
	}

	fprintf(pFile,"\n");

	if(IsDiffuseEnable())
		fprintf(pFile,"	diffuse %i %i %i %i\n",(int)(m_cDiffuse.fa*255.0f),(int)(m_cDiffuse.fr*255.0f),(int)(m_cDiffuse.fg*255.0f),(int)(m_cDiffuse.fb*255.0f));

	if(IsSpecularEnable())
	{
		fprintf(pFile,"	specular %i %i %i %i\n",(int)(m_cSpecular.fa*255.0f),(int)(m_cSpecular.fr*255.0f),(int)(m_cSpecular.fg*255.0f),(int)(m_cSpecular.fb*255.0f));
		fprintf(pFile,"	shininess %f\n",m_fShininess);
	}

	if(IsEmissiveEnable())
		fprintf(pFile,"	emissive %i %i %i %i\n",(int)(m_cEmissive.fa*255.0f),(int)(m_cEmissive.fr*255.0f),(int)(m_cEmissive.fg*255.0f),(int)(m_cEmissive.fb*255.0f));

	fprintf(pFile,"\n}\n");
	fclose(pFile);

	if(bSaveTextures)
	{
		for(int i=0;i<MAX_TEXTURE_STAGES;i++)
			if(m_Stages[i].size() > 0)
			{
				for(int j=0;j<m_Stages[i].size();j++)
					g_TextureMgr.SaveTexture(m_Stages[i][j],"");
			}
	}

	return true;
}

bool Material::Set(const uint32 dwTime)
{
	if(m_bNoDraw) return true;
	if(m_hShader[m_siRenderingPass].IsNull()) return false;

	g_ShaderMgr.SetShader(m_hShader[m_siRenderingPass]);

//	g_Renderer.SetRenderState(D3DRS_ZFUNC,m_DepthFnc);

	if(m_siRenderingPass==RENDERINGPASS_AMBIENT || (m_siRenderingPass==RENDERINGPASS_SPECIAL && m_bBlend))
	{
		g_Renderer.SetRenderState(D3DRS_ALPHABLENDENABLE,m_bBlend);
	//	g_Renderer.SetRenderState(D3DRS_ZWRITEENABLE,m_bZWriteEnable);

		if(m_bBlend)
		{
			g_Renderer.SetRenderState(D3DRS_SRCBLEND,m_SrcBlend);
			g_Renderer.SetRenderState(D3DRS_DESTBLEND,m_DstBlend);
		}
	}

	g_Renderer.SetRenderState(D3DRS_CULLMODE,m_CullMode);

	if(m_bMustRecompile)
		CompileMat();

	if(m_siRenderingPass > 0 && (m_dwColorFlags & MATCOL_SPECULARENABLE))
	{
		const float fSpecular[4]={m_cSpecular.fr,m_cSpecular.fg,m_cSpecular.fb,m_fShininess};
		g_Renderer.SetVertexShaderConstant(COLOR_SPECULAR_REG,fSpecular);
	}

	if(m_siRenderingPass==0 && (m_dwColorFlags & MATCOL_EMISSIVEENABLE))
	{
		const float fEmissive[4]={m_cEmissive.fr,m_cEmissive.fg,m_cEmissive.fb,1.0f};
		g_Renderer.SetVertexShaderConstant(COLOR_EMISSIVE_REG,fEmissive);
	}

	if(m_siRenderingPass==RENDERINGPASS_AMBIENT)
	{
		for(int i=0;i<2;i++)
		{
			if(m_Stages[i].size() > 0)
			{
				if(m_Stages[i].size() > 1)
					SetTexture(i,dwTime);
				else
					g_TextureMgr.SetTexture(i,m_Stages[i][0]);
			}
		}
	}
	else
	{
		for(int i=0;i<MAX_TEXTURE_STAGES;i++)
		{
			if(m_Stages[i].size() > 0)
			{
				if(m_Stages[i].size() > 1)
					SetTexture(i,dwTime);
				else
					g_TextureMgr.SetTexture(i,m_Stages[i][0]);
			}
		}
	}

	return true;
}

bool Material::CompileMat()
{
/*if(m_dwColorFlags==0) return true;

	m_Mat.Power=m_fShininess;
	m_Mat.Ambient.a=m_Mat.Diffuse.a=m_cDiffuse.fa;
	m_Mat.Ambient.r=m_Mat.Diffuse.r=m_cDiffuse.fr;
	m_Mat.Ambient.g=m_Mat.Diffuse.g=m_cDiffuse.fg;
	m_Mat.Ambient.b=m_Mat.Diffuse.b=m_cDiffuse.fb;

	m_Mat.Specular.a=m_cSpecular.fa;
	m_Mat.Specular.r=m_cSpecular.fr;
	m_Mat.Specular.g=m_cSpecular.fg;
	m_Mat.Specular.b=m_cSpecular.fb;
*/
	m_bMustRecompile=false;

	return true;
}

bool Material::SameAs(IMaterial *pMat_)
{
	Material *pMat=(Material *)pMat_;

	if(!pMat) return false;
	if(strcmp(pMat->m_szName,m_szName)!=0) return false;
	if(m_cDiffuse!=pMat->m_cDiffuse ||
	   m_cSpecular!=pMat->m_cSpecular ||
	   m_cEmissive!=pMat->m_cEmissive ||
	   m_fShininess!=pMat->m_fShininess) return false;
	
	return true;
}

inline void Material::SetTexture(const int iStage, uint32 dwTime)
{
	const uint32 dtime=m_dwAnimTimes[iStage] / (uint32)m_Stages[iStage].size();
	uint32 dwNum=dwTime / dtime;

	if(dwNum >= m_Stages[iStage].size()) dwNum=0;

	g_TextureMgr.SetTexture(iStage,m_Stages[iStage][dwNum]);
}

}