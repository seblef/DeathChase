
#include "SoundSource.h"
#include "ISoundBuffer.h"
#include "../../SDK/OpenAL/al.h"

namespace Core
{

SoundSource::~SoundSource()
{
	Release();
	if(m_pBuffer) delete m_pBuffer;
}

bool SoundSource::IsPlaying()
{
	if(!m_dwSource) return false;

	ALint iState=AL_STOPPED;
	alGetSourcei(m_dwSource,AL_SOURCE_STATE,&iState);

	return iState==AL_PLAYING;
}

void SoundSource::SetRelative(bool bRelative)
{
	if(m_dwSource)
		alSourcei(m_dwSource,AL_SOURCE_RELATIVE,bRelative ? AL_TRUE : AL_FALSE);
}

void SoundSource::SetPosition(const Vector3& vPos)
{
	if(m_dwSource)
		alSourcefv(m_dwSource,AL_POSITION,(const float *)&vPos);
}

void SoundSource::SetVelocity(const Vector3& vVel)
{
	if(m_dwSource)
		alSourcefv(m_dwSource,AL_POSITION,(const float *)&vVel);
}

void SoundSource::SetGain(float fGain)
{
	if(m_dwSource)
		alSourcef(m_dwSource,AL_GAIN,fGain);
}

void SoundSource::SetReferenceDistance(float fDist)
{
	if(m_dwSource)
		alSourcef(m_dwSource,AL_REFERENCE_DISTANCE,fDist);
}

void SoundSource::SetRollOff(float fRollOff)
{
	if(m_dwSource)
		alSourcef(m_dwSource,AL_ROLLOFF_FACTOR,fRollOff);
}

void SoundSource::Play(ISoundBuffer *pBuffer, bool bLoop)
{
	if(!pBuffer || !m_dwSource) return;

	Stop();
	if(m_pBuffer) delete m_pBuffer;
	
	m_pBuffer=pBuffer->CreateInstance(m_dwSource);
	m_pBuffer->Play(bLoop);
}

void SoundSource::Update(bool bLoop)
{
	if(m_dwSource && m_pBuffer)
		m_pBuffer->Update(bLoop);
}

void SoundSource::Stop()
{
	if(m_dwSource && m_pBuffer)
	{
		if(IsPlaying())
			m_pBuffer->Stop();

		delete m_pBuffer;
		m_pBuffer=0;
	}
}

void SoundSource::Release()
{
	if(m_dwSource)
	{
		Stop();
		alDeleteSources(1,(ALuint*)&m_dwSource);
	}
}

bool SoundSource::Create()
{
	alGetError();
	alGenSources(1,(ALuint *)&m_dwSource);
	if(alGetError()!=AL_NO_ERROR)
		return false;

	alSourcef(m_dwSource,AL_REFERENCE_DISTANCE,75.0f);
	return true;
}

}