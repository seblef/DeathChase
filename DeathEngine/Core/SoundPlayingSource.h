
#ifndef _SOUNDPLAYINGSOURCE_H_
#define _SOUNDPLAYINGSOURCE_H_

namespace Core
{

class SoundSource;
class SoundVirtualSource;

class SoundPlayingSource
{
	public:

		SoundPlayingSource(SoundVirtualSource *pVirt) : m_pVirtualSource(pVirt), m_pActualSource(0), m_bStopped(false) {}
		~SoundPlayingSource() {}

		SoundVirtualSource *m_pVirtualSource;
		SoundSource *m_pActualSource;
		bool m_bStopped;

};

}

#endif