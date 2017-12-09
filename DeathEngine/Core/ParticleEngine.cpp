
#include "ParticleEngine.h"
#include "ParticleSystem.h"
#include "ParticleEmitterStd.h"
#include "ParticleRendererStd.h"
#include "Camera.h"
#include "FileSystem.h"
#include "FileList.h"
#include "ScriptFile.h"

namespace Core
{

ParticleEngine::ParticleEngine() : m_Allocator("ParticleSystem")
{
}

ParticleEngine::~ParticleEngine()
{
	Release();
}

void ParticleEngine::Clear()
{
	std::vector<SystemItem>::iterator i;
	for(i=m_Systems.begin();i!=m_Systems.end();i++)
		if(i->pSystem)
		{
			m_Allocator.Delete(i->pSystem);
			i->pSystem=0;
		}

	m_Systems.clear();
}

void ParticleEngine::Release()
{
	m_Allocator.ResetAll();
	m_Systems.clear();

	std::map<std::string,ParticleSystemTemplate*>::iterator i;
	for(i=m_Templates.begin();i!=m_Templates.end();i++)
	{
		if(i->second->m_pEmitter) i->second->m_pEmitter->Delete();
		if(i->second->m_pRenderer) i->second->m_pRenderer->Delete();
		delete i->second;
	}

	m_Templates.clear();
}

bool ParticleEngine::Load()
{
	g_FileSystem.ResetWorkingDirectory();
	g_FileSystem.ChangeWorkingDirectory("Models\\");
	FileList list;

	std::vector<std::string> filters;
	filters.push_back(std::string(".dpart"));
	list.Reset(&filters,true);

	g_FileSystem.ResetWorkingDirectory();

	const int iFileCount=list.GetFileCount();
	for(int i=0;i<iFileCount;i++)
		ParseFile(list.GetFullFileName(i));

	return true;
}

ParticleSystem *ParticleEngine::CreateSystem(const char *szTemplateName)
{
	ParticleSystem *pSystem=m_Allocator.New();

	if(!szTemplateName)
		return pSystem;
	else
	{
		static std::string strName("");
		strName=szTemplateName;

		std::map<std::string,ParticleSystemTemplate*>::iterator pTemplate=m_Templates.find(strName);
		if(pTemplate==m_Templates.end())
			return pSystem;
		else
		{
			pSystem->Create(*pTemplate->second);
			return pSystem;
		}
	}
}

void ParticleEngine::PlaySystem(ParticleSystem *pSystem, bool bAutoDelete)
{
	std::vector<SystemItem>::iterator i;
	for(i=m_Systems.begin();i!=m_Systems.end();i++)
		if(i->pSystem==0)
		{
			i->pSystem=pSystem;
			i->bAutoDelete=bAutoDelete;
			return;
		}

	m_Systems.push_back(SystemItem(pSystem,bAutoDelete));
}

void ParticleEngine::StopSystem(Core::ParticleSystem *pSystem)
{
	std::vector<SystemItem>::iterator i;
	for(i=m_Systems.begin();i!=m_Systems.end();i++)
		if(i->pSystem==pSystem)
		{
			if(i->bAutoDelete)
				m_Allocator.Delete(i->pSystem);

			i->pSystem=0;
		}
}

void ParticleEngine::Update(float fTime)
{
	std::vector<SystemItem>::iterator i;
	for(i=m_Systems.begin();i!=m_Systems.end();i++)
		if(i->pSystem)
		{
			i->pSystem->Update(fTime);
			if(i->pSystem->IsFinished())
			{
				if(i->bAutoDelete)
					m_Allocator.Delete(i->pSystem);

				i->pSystem=0;
			}
		}
}

void ParticleEngine::Draw(Camera *pCamera)
{
	const ViewFrustrum *pFrustrum=pCamera->GetFrustrum();

	std::vector<SystemItem>::iterator i;
	for(i=m_Systems.begin();i!=m_Systems.end();i++)
	{
		if(i->pSystem)
		{
			if(pFrustrum->IsBoxInside(i->pSystem->GetBBox()))
				i->pSystem->Draw(pCamera);
		}
	}
}

void ParticleEngine::ParseFile(const char *szFileName)
{
	ScriptFile file;
	if(!file.Open(szFileName)) return;

	std::string strName;

	const char *szToken=file.GetToken();
	while(szToken)
	{
		if(strcmp(szToken,"system")==0)
		{
			szToken=file.GetToken();
			if(szToken)
			{
				strName=szToken;
				std::map<std::string,ParticleSystemTemplate*>::iterator pSys=m_Templates.find(strName);
				if(pSys!=m_Templates.end())
				{
					g_Output->AddMsg("x Error [%s]: Le modele %s existe deja, il sera renomme %s_.\n",szFileName,szToken,szToken);
					strName+="_";
				}

				szToken=file.GetToken();

				if(!szToken)
				{
					g_Output->AddMsg("x Error [%s]: Fin de fichier inattendue.\n",szFileName);
					return;
				}

				if(strcmp(szToken,"{")!=0)
				{
					g_Output->AddMsg("x Error [%s]: { attendu après la commande [system].\n",szFileName);
					return;
				}

				ParticleSystemTemplate *pTemplate=new ParticleSystemTemplate;

				szToken=file.GetToken();
				while(szToken && strcmp(szToken,"}")!=0)
				{
					if(strcmp(szToken,"life_range")==0)
					{
						pTemplate->m_fLife1=atof(file.GetToken());
						pTemplate->m_fLife2=atof(file.GetToken());
					}
					else if(strcmp(szToken,"friction_range")==0)
					{
						pTemplate->m_fFriction1=atof(file.GetToken());
						pTemplate->m_fFriction2=atof(file.GetToken());
					}
					else if(strcmp(szToken,"velocity_range")==0)
					{
						file.ParseVector(pTemplate->m_vVelocity1);
						file.ParseVector(pTemplate->m_vVelocity2);
					}
					else if(strcmp(szToken,"start_color_range")==0)
					{
						Core::Color32 c1,c2;
						file.ParseColor(c1);
						file.ParseColor(c2);
						pTemplate->m_cStartColor1=c1; pTemplate->m_cStartColor1*=255.0f;
						pTemplate->m_cStartColor2=c2; pTemplate->m_cStartColor2*=255.0f;
					}
					else if(strcmp(szToken,"end_color_range")==0)
					{
						Core::Color32 c1,c2;
						file.ParseColor(c1);
						file.ParseColor(c2);
						pTemplate->m_cEndColor1=c1; pTemplate->m_cEndColor1*=255.0f;
						pTemplate->m_cEndColor2=c2; pTemplate->m_cEndColor2*=255.0f;
					}
					else if(strcmp(szToken,"start_size_range")==0)
					{
						pTemplate->m_fStartSize1=atof(file.GetToken());
						pTemplate->m_fStartSize2=atof(file.GetToken());
					}
					else if(strcmp(szToken,"end_size_range")==0)
					{
						pTemplate->m_fEndSize1=atof(file.GetToken());
						pTemplate->m_fEndSize2=atof(file.GetToken());
					}
					else if(strcmp(szToken,"mass_range")==0)
					{
						pTemplate->m_fMass1=atof(file.GetToken());
						pTemplate->m_fMass2=atof(file.GetToken());
					}
					else if(strcmp(szToken,"gravity")==0)
						file.ParseVector(pTemplate->m_vGravity);
					else if(strcmp(szToken,"particles_count")==0)
						pTemplate->m_iParticleCount=atoi(file.GetToken());
					else if(strcmp(szToken,"emit_loop")==0)
						pTemplate->m_dwFlags|=PARTICLE_EMITLOOP;
					else if(strcmp(szToken,"emit_once")==0)
						pTemplate->m_dwFlags|=PARTICLE_EMITONCE;
					else if(strcmp(szToken,"emit_speed")==0)
						pTemplate->m_fEmitSpeed=atof(file.GetToken());
					else if(strcmp(szToken,"emitter")==0)
					{
						pTemplate->m_pEmitter=CreateEmitter(file.GetToken());
						if(!pTemplate->m_pEmitter)
						{
							g_Output->AddMsg("x Error [%s]: Emetteur [%s] inconnu.\n",szFileName,szToken);
							delete pTemplate;
							return;
						}

						if(!pTemplate->m_pEmitter->Parse(&file))
						{
							delete pTemplate->m_pEmitter;
							delete pTemplate;
							return;
						}
					}
					else if(strcmp(szToken,"renderer")==0)
					{
						pTemplate->m_pRenderer=CreateRenderer(file.GetToken());
						if(!pTemplate->m_pRenderer)
						{
							g_Output->AddMsg("x Error [%s]: Renderer [%s] inconnu.\n",szFileName,szToken);
							delete pTemplate;
							return;
						}

						if(!pTemplate->m_pRenderer->Parse(&file))
						{
							delete pTemplate->m_pRenderer;
							delete pTemplate;
							return;
						}
					}
					else
						g_Output->AddMsg("x Error [%s]: Token %s inconnu.\n",szFileName,szToken);

					szToken=file.GetToken();
				}

				if(!pTemplate->m_pRenderer || !pTemplate->m_pEmitter)
					g_Output->AddMsg("x Error [%s]: Le systeme %s n'a pas de renderer ou d'emetteur.\n",szFileName,strName.c_str());
				else
					m_Templates.insert(std::make_pair(strName,pTemplate));
			}
			else
				g_Output->AddMsg("x Error [%s]: Fin de fichier inattendue.\n",szFileName);
		}
		else
			g_Output->AddMsg("x Error [%s]: Token %s inconnu.\n",szFileName,szToken);

		szToken=file.GetToken();
	}

	file.Close();
}

IParticleEmitter *ParticleEngine::CreateEmitter(const char *szName)
{
	if(stricmp(szName,"linear")==0)
		return ParticleEmitter_Linear::m_sAllocator.New();
	else if(stricmp(szName,"explosionring")==0)
		return ParticleEmitter_ExplosionRing::m_sAllocator.New();
	else if(stricmp(szName,"debris")==0)
		return ParticleEmitter_Debris::m_sAllocator.New();
	else if(stricmp(szName,"spray")==0)
		return ParticleEmitter_Spray::m_sAllocator.New();
	else if(stricmp(szName,"smoke")==0)
		return ParticleEmitter_Smoke::m_sAllocator.New();
	else if(stricmp(szName,"explosion")==0)
		return ParticleEmitter_Explosion::m_sAllocator.New();
	else if(stricmp(szName,"precipitation")==0)
		return ParticleEmitter_Precipitation::m_sAllocator.New();
	else
		return 0;
}

IParticleRenderer *ParticleEngine::CreateRenderer(const char *szName)
{
	if(stricmp(szName,"point")==0)
		return ParticleRenderer_Point::m_sAllocator.New();
	else if(stricmp(szName,"quad")==0)
		return ParticleRenderer_Quad::m_sAllocator.New();
	else if(stricmp(szName,"mesh")==0)
		return ParticleRenderer_Mesh::m_sAllocator.New();
	else
		return 0;
}

}