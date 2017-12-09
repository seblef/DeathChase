
#include "ExplosionFactory.h"
#include "ExplosionTemplate.h"
#include "Explosion.h"

#include "../Core/ScriptFile.h"
#include "../Core/FileList.h"
#include "../Core/FileSystem.h"
#include "../Core/Sound.h"
#include "../Core/MaterialMgr.h"


namespace Game
{

void ExplosionFactory::Release()
{
	TemplateMap::iterator i;
	for(i=m_Templates.begin();i!=m_Templates.end();i++)
	{
		g_Sound.ReleaseSound(i->second->m_hSound);
		g_MaterialMgr.ReleaseMaterial(i->second->m_hMaterial);

		delete i->second;
	}
}

void ExplosionFactory::Load()
{
	g_FileSystem.ResetWorkingDirectory();
	g_FileSystem.ChangeWorkingDirectory("Models\\");
	Core::FileList list;

	std::vector<std::string> filters;
	filters.push_back(std::string(".dexpl"));
	list.Reset(&filters,true);

	g_FileSystem.ResetWorkingDirectory();

	const int iFileCount=list.GetFileCount();
	for(int i=0;i<iFileCount;i++)
		ParseFile(list.GetFullFileName(i));
}

void ExplosionFactory::ParseFile(const char *szFileName)
{
	Core::ScriptFile file;
	if(!file.Open(szFileName)) return;

	std::string strName;

	const char *szToken=file.GetToken();
	while(szToken)
	{
		if(strcmp(szToken,"explosion")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				strName=szToken;

				TemplateMap::iterator pExpl=m_Templates.find(strName);
				if(pExpl!=m_Templates.end())
				{
					g_Output->AddMsg("x Error [%s]: Le modele d'explosion %s existe deja, il sera renomme %s_.\n",szFileName,strName.c_str(),strName.c_str());
					strName+="_";
				}

				szToken=file.GetToken();
				if(!szToken)
				{
					g_Output->AddMsg("x Error [%s]: Fin de fichier inattendue.\n",szFileName);
					return;
				}
				else if(strcmp(szToken,"{")!=0)
				{
					g_Output->AddMsg("x Error [%s]: { attendu apres la commande [explosion].\n",szFileName);
					return;
				}

				ExplosionTemplate *pTemplate=new ExplosionTemplate;
				strcpy(pTemplate->m_szDebrisSystemName,"DefaultExplosionDebris");
				strcpy(pTemplate->m_szSmokeSystemName,"DefaultExplosionSmoke");
				strcpy(pTemplate->m_szSplashSystemName,"DefaultExplosionSplash");

				szToken=file.GetToken();
				while(szToken && strcmp(szToken,"}")!=0)
				{
					if(strcmp(szToken,"life_time")==0)
						pTemplate->m_fLifeTime1=pTemplate->m_fLifeTime2=atof(file.GetToken());
					else if(strcmp(szToken,"count")==0)
						pTemplate->m_iCount=atoi(file.GetToken());
					else if(strcmp(szToken,"material")==0)
						pTemplate->m_hMaterial=g_MaterialMgr.LoadMaterial(file.GetToken(),0);
					else if(strcmp(szToken,"sound")==0)
						pTemplate->m_hSound=g_Sound.LoadSound(file.GetToken(),0);
					else if(strcmp(szToken,"debris_system")==0)
					{
						strcpy(pTemplate->m_szDebrisSystemName,file.GetToken());
						pTemplate->m_bDebris=true;
					}
					else if(strcmp(szToken,"smoke_system")==0)
					{
						strcpy(pTemplate->m_szSmokeSystemName,file.GetToken());
						pTemplate->m_bSmoke=true;
					}
					else if(strcmp(szToken,"splash_system")==0)
					{
						strcpy(pTemplate->m_szSplashSystemName,file.GetToken());
						pTemplate->m_bSplash=true;
					}
					else if(strcmp(szToken,"nosmoke")==0)
						pTemplate->m_bSmoke=false;
					else if(strcmp(szToken,"nodebris")==0)
						pTemplate->m_bDebris=false;
					else if(strcmp(szToken,"nosplash")==0)
						pTemplate->m_bSplash=false;
					else
						g_Output->AddMsg("x Error [%s]: Token %s inconnu.\n",szFileName,szToken);

					szToken=file.GetToken();
				}

				m_Templates.insert(std::make_pair(strName,pTemplate));
			}
			else
				g_Output->AddMsg("x Error [%s]: Fin de fichier inattendue.\n");
		}
		else
			g_Output->AddMsg("x Error [%s]: Token %s inconnu.\n",szFileName,szToken);

		szToken=file.GetToken();
	}

	file.Close();
}

Explosion *ExplosionFactory::CreateExplosion(const char *szTemplateName,
											 const Core::Vector3& vPos, float fSize,
											 bool bSmoke, bool bDebris, bool bSplash)
{
	static std::string strTemplateName;
	strTemplateName=szTemplateName;

	TemplateMap::iterator pExplTemplate=m_Templates.find(strTemplateName);
	if(pExplTemplate==m_Templates.end()) return 0;

	ExplosionTemplate *pTemplate=pExplTemplate->second;
	Explosion *pExpl=Explosion::New();

	pExpl->Create(vPos,fSize,pTemplate->m_fLifeTime1,
		pTemplate->m_hMaterial, pTemplate->m_hSound,
		bSmoke & pTemplate->m_bSmoke,
		bDebris & pTemplate->m_bDebris,
		bSplash & pTemplate->m_bSplash,
		pTemplate->m_szSmokeSystemName,
		pTemplate->m_szSplashSystemName,
		pTemplate->m_szDebrisSystemName);

	return pExpl;
}

}