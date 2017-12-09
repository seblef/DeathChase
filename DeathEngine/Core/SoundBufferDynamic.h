
#ifndef _SOUNDBUFFERDYNAMIC_H_
#define _SOUNDBUFFERDYNAMIC_H_

#include "ISoundBuffer.h"
#include "../../SDK/OpenAL/al.h"
#include "../../SDK/OpenAL/ogg.h"
#include "../../SDK/OpenAL/codec.h"
#include "../../SDK/OpenAL/vorbisfile.h"

namespace Core
{

class SoundBufferDynamicInstance : public ISoundBufferInstance
{
	public:

		SoundBufferDynamicInstance(const uint32 dwSource, const char *szFileName);
		~SoundBufferDynamicInstance();

		void Play(bool bLoop);
		void Stop();
		void Update(bool bLoop);

	protected:

		static size_t ReadFunc(void *pPtr, size_t size, size_t nmemb, void *pDataSource);
		static int SeekFunc(void *pDataSource, ogg_int64_t offset, int whence);
		static int CloseFunc(void *pDataSource);
		static long TellFunc(void *pDataSource);

		uint32 m_dwBuffers[2];
		ALenum m_Format;
		OggVorbis_File m_Stream;
		vorbis_info *m_pVorbisInfo;

		const char *ErrorString(int iCode);
		bool AddDataToBuffer(uint32 dwBuffer, bool bLoop);
};

class SoundBufferDynamic : public ISoundBuffer
{
	public:

		SoundBufferDynamic(const char *szFileName) : ISoundBuffer(szFileName) {}
		~SoundBufferDynamic() {}

		ISoundBufferInstance *CreateInstance(uint32 dwSource) { return new SoundBufferDynamicInstance(dwSource,m_szFileName); }
};

}

#endif
