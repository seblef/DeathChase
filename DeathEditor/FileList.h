
#ifndef _FILELIST_H_

#define _FILELIST_H_

#include <string>
#include <vector>
#include "../DeathEngine/Common.h"

class FileList
{
	public:

		FileList();
		~FileList() {};

		uint32 GetFileCount() const;
		const char *GetFileName(uint32 dwIndex) const;
		const char *GetFullFileName(uint32 dwIndex);
		bool IsDirectory(uint32 dwIndex) const;

		void Reset(std::vector<std::string> *pFilters, bool bNoDirs=false);

private:

	class FileEntry
	{
		public:

			FileEntry() {};
			~FileEntry() {};

			std::string strName;
			std::string strFullName;
			uint32 dwSize;
			bool bDirectory;
	};

	std::string m_strPath;
	std::vector<FileEntry> m_Files;
};

#endif
