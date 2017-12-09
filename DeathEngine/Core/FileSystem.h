
#ifndef _FILESYSTEM_H_

#define _FILESYSTEM_H_

#include "TSingleton.h"

namespace Core
{

class FileSystem : public TSingleton<FileSystem>
{
	public:

		FileSystem();
		~FileSystem();

		bool ChangeWorkingDirectory(const char *szDir);
		void ResetWorkingDirectory();
		const char *GetWorkingDirectory() const;
		bool CreateDirectory_(const char *szDir);
		bool ExtractPath(const char *szFileName, char *szPath);
		void ExtractFileName(const char *szPathName, char *szFileName);

	private:

		char m_szStartDirectory[1024];

};

}

#define g_FileSystem				Core::FileSystem::GetSingletonRef()

#endif