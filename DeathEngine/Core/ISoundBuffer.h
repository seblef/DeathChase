
#ifndef _ISOUNDBUFFER_H_
#define _ISOUNDBUFFER_H_

#include "../Common.h"
#include "IResource.h"
#include <string.h>

namespace Core
{

class ISoundBufferInstance
{
	public:

		ISoundBufferInstance(const uint32 dwSource) : m_dwSource(dwSource) {}
		virtual ~ISoundBufferInstance() {}

		virtual void Play(bool bLoop)=0;
		virtual void Stop()=0;
		virtual void Update(bool bLoop)=0;

	protected:

		uint32 m_dwSource;
};

class ISoundBuffer : public IResource
{
	public:

		ISoundBuffer(const char *szFileName) { strcpy(m_szFileName,szFileName); }
		virtual ~ISoundBuffer() {}

		virtual ISoundBufferInstance *CreateInstance(uint32 dwSource)=0;
		const char *GetFileName() const { return m_szFileName; }

	protected:

		char m_szFileName[64];
};

}

#endif
