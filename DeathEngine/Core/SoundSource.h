
#ifndef _SOUNDSOURCE_H_
#define _SOUNDSOURCE_H_

#include "Vector.h"

namespace Core
{

class ISoundBuffer;
class ISoundBufferInstance;

class SoundSource
{
	public:

		SoundSource() : m_dwSource(0), m_pBuffer(0) {}
		virtual ~SoundSource();

		void Play(ISoundBuffer *pBuffer, bool bLoop=false);
		void Update(bool bLoop=false);
		void Stop();

		bool IsPlaying();

		void SetRelative(bool bRelative);
		void SetPosition(const Vector3& vPos);
		void SetVelocity(const Vector3& vVel);
		void SetReferenceDistance(float fDist);
		void SetRollOff(float fRollOff);
		void SetGain(float fGain);

		bool Create();
		void Release();

	protected:

		uint32 m_dwSource;
		ISoundBufferInstance *m_pBuffer;

};

}

#endif