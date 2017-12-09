
#ifndef _FILESYSTEM_H_

#define _FILESYSTEM_H_

#include "../DeathEngine/Core/TSingleton.h"
#include "../DeathEngine/Dirs.h"

#include <string>
#include <vector>

enum FileSystemType_
{
	FILE_TEXTURE=0,
	FILE_SHADER,
	FILE_MATERIAL,
	FILE_MESH,
	FILE_SOUND,
	FILE_SPACESHIP,
	FILE_PARTICLESYSTEM,
	FILE_EXPLOSION,
	FILE_WEAPON,
	FILE_DEFENSE,
	FILE_BONUS,
	FILE_MAP,
	FILE_HEIGHTMAP,

	FILE_COUNT

};

class FileEntry
{
	public:

		FileEntry() {}
		~FileEntry()
		{
			std::vector<FileEntry *>::iterator i;
			for(i=Children.begin();i!=Children.end();i++)
				delete *i;
		}

		typedef std::vector<FileEntry*>	FileEntryVector;

		std::string strPathName;
		std::string strFileName;
		FileEntryVector Children;

};

class FileSystem : public Core::TSingleton<FileSystem>
{
	public:

		FileSystem();
		~FileSystem();

		bool ParseDirectories();
		void RebuildFileList(FileSystemType_ type);
		const std::vector<std::string> *GetFilters(FileSystemType_ type);
		FileEntry *GetFileList(FileSystemType_ type);

		bool ChangeWorkingDirectory(const char *szDir);
		void ResetWorkingDirectory();
		const char *GetWorkingDirectory() const;
		bool CreateDirectory_(const char *szDir);

	private:

		FileEntry *RecursParse(const char *szName,
			std::vector<std::string> *pFilters, 
			std::string& strCurrentPath); // return true si un des sous repertoire contient un fichier de ce type

		char m_szStartDirectory[1024];

		FileEntry *m_pEntries[FILE_COUNT];
		std::vector<std::string> m_Extensions[FILE_COUNT];

};


#define g_FileSystem				FileSystem::GetSingletonRef()

#endif