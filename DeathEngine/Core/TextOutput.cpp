
#include "TextOutput.h"
#include <stdarg.h>
#include <string.h>

#ifdef DEDEV
#include "../Dev/Console.h"
#endif

namespace Core
{

TextOutput TextOutput::m_sInstance;

TextOutput::TextOutput() : m_pFile(0),m_bSpecial(false), m_bStarted(false)
{
#ifdef DEEXPORT
	m_pFunc=0;
#endif
}

TextOutput::~TextOutput()
{
	if(m_bStarted)
		End();
}

bool TextOutput::Start(const char *szFileName, const bool bSpecial)
{
	if(!szFileName)
		return false;

	m_pFile=fopen(szFileName,"w");
	if(!m_pFile)
		return false;

	if(bSpecial)
		fclose(m_pFile);

	strcpy(m_szFileName,szFileName);
	m_bSpecial=bSpecial;
	m_bStarted=true;

	AddMsg("\n> DeathEngine Notification\n>  (2007 by Koyot)\n\n");

	return true;
}

bool TextOutput::End()
{
	if(m_bStarted && !m_bSpecial)
		fclose(m_pFile);

	m_pFile=0;
	m_bStarted=false;
	m_bSpecial=false;

	return true;
}

void TextOutput::AddMsg(const char *szMsg,...)
{
	va_list args;
	va_start(args,szMsg);

	if(m_bStarted)
	{
		if(m_bSpecial)
		{
			m_pFile=fopen(m_szFileName,"a");
			if(!m_pFile)
				return;
		}

		char szTempBuffer[1024];
		vsprintf(szTempBuffer,szMsg,args);

		fprintf(m_pFile,szTempBuffer);

#ifdef DEDEV
		if(Dev::g_bConsoleCreated)
			g_Console.Output(szTempBuffer);
#endif
#ifdef DEEXPORT
		if(m_pFunc) m_pFunc(szTempBuffer);
#endif

		if(m_bSpecial)
			fclose(m_pFile);
	}
}

}