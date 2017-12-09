
#include "SoundBufferDynamic.h"
#include "TextOutput.h"

namespace Core
{

#define BUFFER_SIZE (4096 * 50)

SoundBufferDynamicInstance::SoundBufferDynamicInstance(const uint32 dwSource, const char *szFileName) :
	ISoundBufferInstance(dwSource)
{
	ov_callbacks callbacks={ReadFunc,SeekFunc,CloseFunc,TellFunc};

	FILE *pOGGFile=fopen(szFileName,"rb");
	if(!pOGGFile)
	{
		g_Output->AddMsg("x Erreur au chargement du son %s.\n",szFileName);
		return;
	}

	int iRes=ov_open_callbacks((void *)pOGGFile,&m_Stream,0,0,callbacks);
	if(iRes < 0)
	{
		fclose(pOGGFile);
		g_Output->AddMsg("x Error a l'ouverture du flux OGG (%s,%s).\n",szFileName,ErrorString(iRes));
		return;
	}

	m_pVorbisInfo=ov_info(&m_Stream,-1);
	if(m_pVorbisInfo->channels==1)
		m_Format=AL_FORMAT_MONO16;
	else
		m_Format=AL_FORMAT_STEREO16;

	alGenBuffers(2,(ALuint *)m_dwBuffers);
}

SoundBufferDynamicInstance::~SoundBufferDynamicInstance()
{
	if(m_dwBuffers[0])
		alDeleteBuffers(2,(ALuint*)m_dwBuffers);

	ov_clear(&m_Stream);
}

void SoundBufferDynamicInstance::Play(bool bLoop)
{
	ov_raw_seek(&m_Stream,0);

	if(!AddDataToBuffer(m_dwBuffers[0],bLoop)) return;
	if(!AddDataToBuffer(m_dwBuffers[1],bLoop)) return;

	alSourcei(m_dwSource,AL_BUFFER,0);
	alSourcei(m_dwSource,AL_LOOPING,AL_FALSE);

	alSourceQueueBuffers(m_dwSource,2,(ALuint*)m_dwBuffers);
	if(alGetError()!=AL_NO_ERROR) return;

	alSourcePlay(m_dwSource);
}

void SoundBufferDynamicInstance::Stop()
{
	ALuint buffer;
	alSourceUnqueueBuffers(m_dwSource,1,&buffer);
	alSourceUnqueueBuffers(m_dwSource,1,&buffer);
}

void SoundBufferDynamicInstance::Update(bool bLoop)
{
	int iProcessed=0;
	alGetSourcei(m_dwSource,AL_BUFFERS_PROCESSED,&iProcessed);

	if(iProcessed==2)
		g_Output->AddMsg("x OGG Falling behind\n");

	while(iProcessed--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(m_dwSource,1,&buffer);
		if(AddDataToBuffer(buffer,bLoop))
			alSourceQueueBuffers(m_dwSource,1,&buffer);
	}
}

bool SoundBufferDynamicInstance::AddDataToBuffer(uint32 dwBuffer, bool bLoop)
{
	char data[BUFFER_SIZE];
	int iSize=0;
	int iSection;
	int iRes;

	while(iSize < BUFFER_SIZE)
	{
		iRes=ov_read(&m_Stream,data+iSize,BUFFER_SIZE-iSize,0,2,1,&iSection);

		if(iRes > 0) iSize+=iRes;
		else if(iRes < 0) return false;
		else break;
	}

	if(iSize==0)
	{
		if(!bLoop)
			return false;
		else
		{
			ov_raw_seek(&m_Stream,0);
			return AddDataToBuffer(dwBuffer,false);
		}
	}

	alBufferData(dwBuffer,m_Format,data,iSize,m_pVorbisInfo->rate);
	return true;
}

const char *SoundBufferDynamicInstance::ErrorString(int iCode)
{
	switch(iCode)
	{
		case OV_EREAD: return "Read";
		case OV_ENOTVORBIS: return "Not vorbis data";
		case OV_EVERSION: return "Vorbis version";
		case OV_EBADHEADER: return "Invalid vorbis header";
		case OV_EFAULT: return "Internal error";
		default: return "Unknown error";
	}
}

size_t SoundBufferDynamicInstance::ReadFunc(void *pPtr, size_t size, size_t nmemb, void *pDataSource)
{
	return fread(pPtr,size,nmemb,(FILE *)pDataSource);
}

int SoundBufferDynamicInstance::SeekFunc(void *pDataSource, ogg_int64_t offset, int whence)
{
	return fseek((FILE *)pDataSource,(long)offset,whence);
}

int SoundBufferDynamicInstance::CloseFunc(void *pDataSource)
{
	return fclose((FILE *)pDataSource);
}

long SoundBufferDynamicInstance::TellFunc(void *pDataSource)
{
	return ftell((FILE *)pDataSource);
}

}
