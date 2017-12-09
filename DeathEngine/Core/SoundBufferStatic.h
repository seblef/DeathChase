
#ifndef _SOUNDBUFFERSTATIC_H_
#define _SOUNDBUFFERSTATIC_H_

#include "ISoundBuffer.h"

namespace Core
{

class SoundBufferStaticInstance : public ISoundBufferInstance
{
	public:

		SoundBufferStaticInstance(const uint32 dwSource, const uint32 dwBuffer) : ISoundBufferInstance(dwSource), m_dwBuffer(dwBuffer) {}
		~SoundBufferStaticInstance() {}

		void Play(bool bLoop);
		void Stop();
		void Update(bool bLoop) {}

	protected:

		uint32 m_dwBuffer;
};

class SoundBufferStatic : public ISoundBuffer
{
	public:

		SoundBufferStatic(const char *szFileName);
		~SoundBufferStatic();

		ISoundBufferInstance *CreateInstance(uint32 dwSource) { return new SoundBufferStaticInstance(dwSource,m_dwBuffer); }

	protected:

		uint32 m_dwBuffer;
};

}

#endif
