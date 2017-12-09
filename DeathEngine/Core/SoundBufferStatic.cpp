
#include "SoundBufferStatic.h"
#include "../../SDK/OpenAL/al.h"
#include "../../SDK/OpenAL/alut.h"

namespace Core
{

void SoundBufferStaticInstance::Play(bool bLoop)
{
	if(!m_dwBuffer) return;

	alSourcei(m_dwSource,AL_BUFFER,0);
	alSourcei(m_dwSource,AL_BUFFER,m_dwBuffer);
	alSourcei(m_dwSource,AL_LOOPING,bLoop ? AL_TRUE : AL_FALSE);
	alSourcePlay(m_dwSource);
}

void SoundBufferStaticInstance::Stop()
{
	if(!m_dwBuffer) return;
	alSourceStop(m_dwSource);
}


SoundBufferStatic::SoundBufferStatic(const char *szFileName) :
	ISoundBuffer(szFileName),
	m_dwBuffer(0)
{
	alGetError();
	alGenBuffers(1,(ALuint*)&m_dwBuffer);
	if(alGetError()!=AL_NO_ERROR)
		return;

	void *pData;
	ALenum format;
	ALsizei size;
	ALsizei freq;
	ALboolean loop;

	alutLoadWAVFile((ALbyte*)szFileName,&format,&pData,&size,&freq,&loop);
	if(alGetError()!=AL_NO_ERROR) return;

	alBufferData(m_dwBuffer,format,pData,size,freq);
	alutUnloadWAV(format,pData,size,freq);
}

SoundBufferStatic::~SoundBufferStatic()
{
	if(m_dwBuffer)
		alDeleteBuffers(1,(ALuint*)&m_dwBuffer);
}


}