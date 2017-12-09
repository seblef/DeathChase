
#include "Sound.h"
#include "SoundSource.h"
#include "SoundBufferStatic.h"
#include "SoundBufferDynamic.h"
#include "SoundPlayingSource.h"
#include "SoundVirtualSource.h"
#include "Timer.h"
#include "../Dirs.h"
#include "../../SDK/OpenAL/al.h"
#include "../../SDK/OpenAL/alc.h"
#include <algorithm>

namespace Core
{

#define SOUND_CHANNELS_COUNT				32

Sound::Sound(const int iPriority) : m_bInited(false), m_fTotalTime(0.0f), ITask(iPriority)
{
	strcpy(m_szName,"Sound");
}

Sound::~Sound()
{
	Stop();
}

bool Sound::Stop()
{
	if(m_bInited)
	{
		SourceVec::iterator i;
		for(i=m_TotalSources.begin();i!=m_TotalSources.end();i++)
			delete *i;

		m_TotalSources.clear();
		m_FreeSources.clear();

		NameIndex::iterator j;
		for(j=m_NameIndex.begin();j!=m_NameIndex.end();j++)
		{
			ISoundBuffer **ppBuffer=m_Sounds.Dereference(j->second);

			if(ppBuffer && *ppBuffer)
				delete *ppBuffer;

			m_Sounds.Release(j->second);
		}

		m_NameIndex.clear();

		ALCcontext *pContext=alcGetCurrentContext();
		ALCdevice *pDevice=alcGetContextsDevice(pContext);

		alcDestroyContext(pContext);
		alcCloseDevice(pDevice);

		m_bInited=false;
	}

	return true;
}

static char *CheckString(char *x) 
{
	return (char *)(x?x:"null");
}

bool Sound::Initialize()
{
	ALCdevice *pDevice=alcOpenDevice(0);
	if(!pDevice)
	{
		g_Output->AddMsg("x Erreur la creation du device audio.\n");
		return false;
	}

	ALCcontext *pContext=alcCreateContext(pDevice,0);
	if(!pContext)
	{
		alcCloseDevice(pDevice);
		g_Output->AddMsg("x Erreur a la creation du contexte sonore.\n");
		return false;
	}

	alcMakeContextCurrent(pContext);
	alDistanceModel(AL_INVERSE_DISTANCE);

	g_Output->AddMsg("\n! <----------------- Sound device ------------------>\n\n");
	g_Output->AddMsg("      AL_VENDOR: %s\n",CheckString((char*)alGetString(AL_VENDOR)));
	g_Output->AddMsg("      AL_VERSION: %s\n",CheckString((char*)alGetString(AL_VERSION)));
	g_Output->AddMsg("      AL_RENDERER: %s\n",CheckString((char*)alGetString(AL_RENDERER)));
	g_Output->AddMsg("      AL_EXTENSIONS: %s\n",CheckString((char*)alGetString(AL_EXTENSIONS)));
	g_Output->AddMsg("! <------------------------------------------------->\n\n");

	for(int i=1;i<SOUND_CHANNELS_COUNT;i++)
	{
		SoundSource *pSource=new SoundSource;
		if(!pSource->Create())
		{
			g_Output->AddMsg("x Erreur a la creation du channel audio %i.\n",i);
			return false;
		}

		m_TotalSources.push_back(pSource);
		m_FreeSources.push_back(pSource);
	}

	m_bInited=true;
	return true;
}

bool Sound::OnSuspend()
{
	return true;
}

bool Sound::OnResume()
{
	return true;
}

bool Sound::Update()
{
	static uint64 dwLastTime=g_Timer.GetTimeInMicroS();
	uint64 dwTime=g_Timer.GetTimeInMicroS();

	const float fTime=((float)(dwTime-dwLastTime)) * 1e-6;

	dwLastTime=dwTime;

	PlayingSourceVec::iterator i;
	for(i=m_PlayingSources.begin();i!=m_PlayingSources.end();i++)
	{
		SoundSource *pSource=(*i)->m_pActualSource;
		if(pSource && pSource->IsPlaying())
			pSource->Update((*i)->m_pVirtualSource->IsLooping());
	}

	m_fTotalTime+=fTime;
	if(m_fTotalTime >= 0.2f)
	{
		m_fTotalTime=0.0f;
		UpdateSources();
	}

	return true;
}

static inline bool lt_virt(SoundPlayingSource *p2, SoundPlayingSource *p1)
{ 
	float dist1 = 0.0f;
	float dist2 = 0.0f;
	unsigned int priority1 = 0;
	unsigned int priority2 = 0;

    SoundVirtualSource *v1 = p1->m_pVirtualSource;
	SoundVirtualSource *v2 = p2->m_pVirtualSource;

	if (v1 && !p1->m_bStopped) priority1 = v1->GetPriority();
	if (v2 && !p2->m_bStopped) priority2 = v2->GetPriority();
	if (v1) dist1 = v1->GetDistance();
	if (v2) dist2 = v2->GetDistance();

	return (priority1 < priority2 ||
		(priority1 == priority2 && dist1 > dist2));
}

void Sound::UpdateSources()
{
	const Vector3& vPos(m_Listener.GetPosition());

	PlayingSourceVec::iterator i;
	for(i=m_PlayingSources.begin();i!=m_PlayingSources.end();i++)
	{
		if((*i)->m_pVirtualSource)
			(*i)->m_pVirtualSource->UpdateDistance(vPos);
	}

	std::sort(m_PlayingSources.begin(),m_PlayingSources.end(),lt_virt);

	const int iTotalSources=m_TotalSources.size();
	const int iTotalPlaying=m_PlayingSources.size();
	int iCount=0;

	PlayingSourceVec::reverse_iterator ri;
	for(ri=m_PlayingSources.rbegin();ri!=m_PlayingSources.rend();ri++,iCount++)
	{
		SoundPlayingSource *pSource=*ri;
		bool bStop=false;

		if(pSource->m_bStopped)
			bStop=true;
		else if(iTotalPlaying-iCount <= iTotalSources)
		{
			if(pSource->m_pActualSource)
			{
				if(!pSource->m_pActualSource->IsPlaying())
					bStop=true;
			}
			else
			{
				assert(!m_FreeSources.empty());
				pSource->m_pActualSource=m_FreeSources.back();
				m_FreeSources.pop_back();
				pSource->m_pVirtualSource->ActualPlay();
			}
		}
		else
			bStop=true;

		if(bStop)
		{
			if(pSource->m_pActualSource)
			{
				pSource->m_pActualSource->Stop();
				m_FreeSources.push_back(pSource->m_pActualSource);
				pSource->m_pActualSource=0;
			}

			if(pSource->m_pVirtualSource)
			{
				if(!pSource->m_pVirtualSource->IsLooping())
					pSource->m_bStopped=true;
			}
		}
	}

	while(!m_PlayingSources.empty())
	{
		SoundPlayingSource *pSource=m_PlayingSources.back();
		if(pSource->m_bStopped)
		{
			if(pSource->m_pVirtualSource)
				pSource->m_pVirtualSource->SetPlayingSource(0);

			delete pSource;
			m_PlayingSources.pop_back();
		}
		else
			break;
	}

	bool bRepeat=true;
	while(bRepeat)
	{
		bRepeat=false;
		VirtualSourceVec::iterator j;
		for(j=m_ManagedSources.begin();j!=m_ManagedSources.end();j++)
		{
			SoundVirtualSource *pSource=*j;
			if(!pSource->IsPlaying())
			{
				m_ManagedSources.erase(j);
				delete pSource;
				bRepeat=true;
				break;
			}
		}
	}
}

HSound Sound::LoadSound(const char *szFileName, const char *szPath)
{
	if(!szFileName) return HSound();

	std::string strFileName(szPath ? szPath : DIR_SOUNDS);
	strFileName+=szFileName;

	NameIndex::iterator i=m_NameIndex.find(strFileName);
	if(i!=m_NameIndex.end())
	{
		ISoundBuffer **ppBuffer=m_Sounds.Dereference(i->second);
		if(ppBuffer && *ppBuffer)
			(*ppBuffer)->AddRef();

		return i->second;
	}
	else
	{
		ISoundBuffer *pBuffer;
		std::string strExt(strFileName.substr(strFileName.length()-4,4));
		if(strExt==".wav")
			pBuffer=new SoundBufferStatic(strFileName.c_str());
		else if(strExt==".ogg")
			pBuffer=new SoundBufferDynamic(strFileName.c_str());
		else
		{
			g_Output->AddMsg("x Error [%s]: Format de fichier son inconnu.\n",strFileName.c_str());
			return HSound();
		}

		HSound hSound;
		ISoundBuffer **ppBuffer=m_Sounds.Acquire(hSound);
		*ppBuffer=pBuffer;

		m_NameIndex.insert(std::make_pair(strFileName,hSound));
		return hSound;
	}
}

void Sound::ReleaseSound(HSound& hSound)
{
	ISoundBuffer **ppBuffer=m_Sounds.Dereference(hSound);
	if(!ppBuffer || !*ppBuffer) return;

	(*ppBuffer)->RemRef();
	if((*ppBuffer)->GetRefCount() <= 0)
	{
		NameIndex::iterator i=m_NameIndex.find(std::string((*ppBuffer)->GetFileName()));
		if(i==m_NameIndex.end())
			return;

		m_NameIndex.erase(i);
		delete *ppBuffer;
		m_Sounds.Release(hSound);
		hSound=HSound();
	}
}

SoundVirtualSource *Sound::Play(HSound hSound, int iPriority, bool bManaged, bool bLoop)
{
//	assert(0);
	ISoundBuffer **ppBuffer=m_Sounds.Dereference(hSound);
	if(!ppBuffer || !(*ppBuffer))
		return 0;

	SoundVirtualSource *pSource=new SoundVirtualSource(iPriority,bLoop,bManaged);
	pSource->Play(*ppBuffer);

	SoundPlayingSource *pPlaying=new SoundPlayingSource(pSource);

	m_PlayingSources.push_back(pPlaying);
	pSource->SetPlayingSource(pPlaying);

	UpdateSources();

	if(bManaged)
		m_ManagedSources.push_back(pSource);
	
	return pSource;
}

void Sound::StopSound(SoundVirtualSource *pVirtualSource)
{
	if(!pVirtualSource) return;

	if(pVirtualSource->GetPlayingSource())
	{
		pVirtualSource->GetPlayingSource()->m_bStopped=true;
		pVirtualSource->GetPlayingSource()->m_pVirtualSource=0;

		UpdateSources();
	}
}

}