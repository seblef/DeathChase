
#ifndef _SOUND_H_
#define _SOUND_H_

#include "TSingleton.h"
#include "THandleMgr.h"
#include "ITask.h"
#include "SoundListener.h"
#include <map>
#include <string>

namespace Core
{

class SoundVirtualSource;
class ISoundBuffer;
class SoundSource;
class SoundPlayingSource;

class Sound : public TSingleton<Sound>, public ITask
{
	public:

		Sound(const int iPriority);
		~Sound();

        bool Initialize();
        bool Stop();
        
        bool OnSuspend();
        bool OnResume();
        
        bool Update();

		HSound LoadSound(const char *szFileName, const char *szPath);
		void ReleaseSound(HSound& hSound);

		SoundVirtualSource *Play(HSound hSound, int iPriority, bool bManaged=true, bool bLoop=false);
		void StopSound(SoundVirtualSource *pVirtualSource);

		int GetAvailableChannels();
		int GetPlayingChannels();

		SoundListener *GetListener() { return &m_Listener; }

	protected:

		typedef THandleMgr<ISoundBuffer *,HSound> HSoundMgr;
		struct istring_less
		{
			bool operator() (const std::string& l, const std::string& r) const
			{
				return _stricmp(l.c_str(),r.c_str()) < 0;
			}
		};
	
		typedef std::map<std::string,HSound,istring_less> NameIndex;
		
		HSoundMgr m_Sounds;
		NameIndex m_NameIndex;

		typedef std::vector<SoundSource*> SourceVec;
		typedef std::vector<SoundVirtualSource*> VirtualSourceVec;
		typedef std::vector<SoundPlayingSource*> PlayingSourceVec;

		float m_fTotalTime;
		SourceVec m_TotalSources;
		SourceVec m_FreeSources;
		VirtualSourceVec m_ManagedSources;
		PlayingSourceVec m_PlayingSources;
		
		SoundListener m_Listener;

		bool m_bInited;

		void UpdateSources();
};

}

#define g_Sound					Core::Sound::GetSingletonRef()

#endif