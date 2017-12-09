
#ifndef _SCRIPTFILE_H_

#define _SCRIPTFILE_H_

#include <stdio.h>

#ifdef DEEXPORT
#include "../Export/IScriptFile.h"
#else
#include "Vector.h"
#include "Color.h"

#define IScriptFile			Core::ScriptFile
#endif


namespace Core
{
#ifdef DEEXPORT
class ScriptFile : public IScriptFile
#else
class ScriptFile
#endif
{
	public:

		ScriptFile();
		~ScriptFile();

		void Clear();

		bool Open(const char *szFile);
		void Close();
		const char *GetFileName() const { return m_szFileName; }
		
		const char *GetToken();
		bool ParseColor(Color32& c);
		bool ParseVector(Vector3& v);

	private:

		const char *GetText();

		FILE *m_pFile;
		char m_szFileName[256];
};

}

#endif

