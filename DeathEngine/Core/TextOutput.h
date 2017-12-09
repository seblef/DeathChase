
#ifndef _TEXTOUTPUT_H_

#define _TEXTOUTPUT_H_

#include <stdio.h>

#ifdef DEEXPORT
#include "../Export/ITextOutput.h"
#endif

namespace Core
{

#ifdef DEEXPORT
class TextOutput : public ITextOutput
#else
class TextOutput
#endif
{
	public:

		TextOutput();
		~TextOutput();

		static TextOutput *GetInstance() { return &m_sInstance; };

		bool Start(const char *szFileName,
			const bool bSpecial);
		bool End();

		void AddMsg(const char *szMsg,...);

#ifdef DEEXPORT
		void SetOutputRedirect(TextOutputFct pFunc) { m_pFunc=pFunc; }
#endif

	protected:

		char m_szFileName[64];
		bool m_bSpecial;
		bool m_bStarted;
		FILE *m_pFile;

#ifdef DEEXPORT
		TextOutputFct m_pFunc;
#endif

		static TextOutput m_sInstance;

};

}

#define g_Output			Core::TextOutput::GetInstance()

#endif