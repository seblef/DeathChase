
#include "SoundVirtualSource.h"
#include "SoundPlayingSource.h"
#include "SoundSource.h"

namespace Core
{

SoundVirtualSource::SoundVirtualSource(int iPriority, bool bLoop, bool bManaged) :
	m_pPlayingSource(0), m_pBuffer(0),
	m_iPriority(iPriority),
	m_fGain(1.0f), m_fReferenceDistance(75.0f), m_fRollOff(1.0f),
	m_bRelative(false), m_bManaged(bManaged), m_bLoop(bLoop)
{
}

void SoundVirtualSource::Play(ISoundBuffer *pBuffer)
{
	Stop();

	m_pBuffer=pBuffer;
}

void SoundVirtualSource::ActualPlay()
{
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
	{
		m_pPlayingSource->m_pActualSource->SetGain(m_fGain);
		m_pPlayingSource->m_pActualSource->SetRollOff(m_fRollOff);
		m_pPlayingSource->m_pActualSource->SetReferenceDistance(m_fReferenceDistance);
		m_pPlayingSource->m_pActualSource->SetRelative(m_bRelative);
		m_pPlayingSource->m_pActualSource->SetPosition(m_vPosition);
		m_pPlayingSource->m_pActualSource->SetVelocity(m_vVelocity);
		m_pPlayingSource->m_pActualSource->Play(m_pBuffer,m_bLoop);
	}
}

bool SoundVirtualSource::IsPlaying()
{
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
		return m_pPlayingSource->m_pActualSource->IsPlaying();
	else
		return false;
}

void SoundVirtualSource::SetRelative()
{
	m_bRelative=true;
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
		m_pPlayingSource->m_pActualSource->SetRelative(true);
}

void SoundVirtualSource::SetPosition(const Vector3& vPos)
{
	m_vPosition=vPos;
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
		m_pPlayingSource->m_pActualSource->SetPosition(vPos);
}

void SoundVirtualSource::SetVelocity(const Vector3& vVel)
{
	m_vVelocity=vVel;
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
		m_pPlayingSource->m_pActualSource->SetVelocity(vVel);
}

void SoundVirtualSource::SetGain(float fGain)
{
	m_fGain=fGain;
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
		m_pPlayingSource->m_pActualSource->SetGain(fGain);
}

void SoundVirtualSource::SetReferenceDistance(float fDist)
{
	m_fReferenceDistance=fDist;
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
		m_pPlayingSource->m_pActualSource->SetReferenceDistance(fDist);
}

void SoundVirtualSource::SetRollOff(float fRollOff)
{
	m_fRollOff=fRollOff;
	if(m_pPlayingSource && m_pPlayingSource->m_pActualSource)
		m_pPlayingSource->m_pActualSource->SetRollOff(fRollOff);
}

void SoundVirtualSource::UpdateDistance(const Vector3& vListener)
{
	m_fDistance=(m_vPosition-vListener).GetSquareLength();
}

}