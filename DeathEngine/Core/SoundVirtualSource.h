
#ifndef _SOUNDVIRTUALSOURCE_H_
#define _SOUNDVIRTUALSOURCE_H_

#include "ISoundBuffer.h"
#include "Vector.h"

namespace Core
{

class SoundPlayingSource;

class SoundVirtualSource
{
	public:

		SoundVirtualSource(int iPriority, bool bLoop, bool bManaged);
		~SoundVirtualSource() { Stop(); }

		void Play(ISoundBuffer *pBuffer);
		void Stop() { m_pPlayingSource=0; }

		bool IsPlaying();
		bool IsManaged() const { return m_bManaged; }
		bool IsRelative() const { return m_bRelative; }
		bool IsLooping() const { return m_bLoop; }
		const Vector3& GetPosition() const { return m_vPosition; }
		int GetPriority() const { return m_iPriority; }
		ISoundBuffer *GetBuffer() { return m_pBuffer; }
		SoundPlayingSource *GetPlayingSource() { return m_pPlayingSource; }
		float GetDistance() const { return m_fDistance; }

		void SetRelative();
		void SetPosition(const Vector3& vPos);
		void SetVelocity(const Vector3& vVel);
		void SetGain(float fGain);
		void SetReferenceDistance(float fDist);
		void SetRollOff(float fRollOff);

		void ActualPlay();
		void SetPlayingSource(SoundPlayingSource *pSource) { m_pPlayingSource=pSource; }
		void UpdateDistance(const Vector3& vListener);

	protected:

		int m_iPriority;
		SoundPlayingSource *m_pPlayingSource;
		ISoundBuffer *m_pBuffer;

		Vector3 m_vPosition;
		Vector3 m_vVelocity;
		float m_fDistance;
		float m_fGain;
		float m_fReferenceDistance;
		float m_fRollOff;

		bool m_bRelative;
		bool m_bLoop;
		bool m_bManaged;
};

}

#endif
