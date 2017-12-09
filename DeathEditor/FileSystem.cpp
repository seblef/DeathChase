
#include "FileSystem.h"
#include <direct.h>
#include <string.h>
#include "FileList.h"

const char *szFileTypeNames[]=
{
	"Textures",
	"Shaders",
	"Matériaux",
	"Modèles",
	"Sons",
	"Vaisseaux",
	"Systèmes de particules",
	"Explosions",
	"Armes",
	"Défenses",
	"Bonus",
	"Maps",
	"Heightmaps"
};

FileSystem::FileSystem()
{
	strcpy(m_szStartDirectory,GetWorkingDirectory());

	for(int i=0;i<FILE_COUNT;i++)
		m_pEntries[i]=0;

	m_Extensions[FILE_TEXTURE].push_back(".bmp");
	m_Extensions[FILE_TEXTURE].push_back(".jpg");
	m_Extensions[FILE_TEXTURE].push_back(".tga");
	m_Extensions[FILE_TEXTURE].push_back(".png");

	m_Extensions[FILE_SHADER].push_back(".hlsl");

	m_Extensions[FILE_MATERIAL].push_back(".dmat");

	m_Extensions[FILE_MESH].push_back(".dgeo");

	m_Extensions[FILE_SOUND].push_back(".wav");
	m_Extensions[FILE_SOUND].push_back(".ogg");

	m_Extensions[FILE_SPACESHIP].push_back(".sps");

	m_Extensions[FILE_PARTICLESYSTEM].push_back(".dps");

	m_Extensions[FILE_EXPLOSION].push_back(".dexp");

	m_Extensions[FILE_WEAPON].push_back(".dwp");

	m_Extensions[FILE_DEFENSE].push_back(".ddf");

	m_Extensions[FILE_BONUS].push_back(".dbn");

	m_Extensions[FILE_MAP].push_back(".dmap");

	m_Extensions[FILE_HEIGHTMAP].push_back(".hgt");
	m_Extensions[FILE_HEIGHTMAP].push_back(".bmp");
}

FileSystem::~FileSystem()
{
	for(int i=0;i<FILE_COUNT;i++)
		if(m_pEntries[i])
			delete m_pEntries[i];
}

bool FileSystem::ChangeWorkingDirectory(const char *szDir)
{
	return (_chdir(szDir)==0);
}

const char *FileSystem::GetWorkingDirectory() const
{
	static char szDir[1024];
	_getcwd(szDir, 1024);

	return szDir;
}

void FileSystem::ResetWorkingDirectory()
{
	ChangeWorkingDirectory(m_szStartDirectory);
}

bool FileSystem::CreateDirectory_(const char *szDir)
{
	_mkdir(szDir);
	return true;
}

bool FileSystem::ParseDirectories()
{
	for(int i=0;i<FILE_COUNT;i++)
		RebuildFileList((FileSystemType_)i);

	return true;
}

void FileSystem::RebuildFileList(FileSystemType_ type)
{
	ResetWorkingDirectory();

	if(m_pEntries[type]) delete m_pEntries[type];

	std::string strPath("");
	m_pEntries[type]=RecursParse(szFileTypeNames[type],&m_Extensions[type],strPath);
}

FileEntry *FileSystem::RecursParse(const char *szName,std::vector<std::string> *pFilters, std::string& strCurrentPath)
{
	FileList list;
	list.Reset(pFilters);
	bool bValid=false;
	FileEntry *pEntry=0;

	for(int i=0;i<list.GetFileCount();i++)
	{
		if(list.IsDirectory(i) && strcmp(list.GetFileName(i),".")!=0 && strcmp(list.GetFileName(i),"..")!=0)
		{
			std::string strPath(strCurrentPath + list.GetFileName(i));
			strPath+="\\";
			g_FileSystem.ChangeWorkingDirectory(list.GetFileName(i));
			FileEntry *pSubEntry=RecursParse(list.GetFileName(i),pFilters,strPath);
			g_FileSystem.ChangeWorkingDirectory("..");
			if(pSubEntry)
			{
				if(!pEntry)
				{
					pEntry=new FileEntry;
					pEntry->strFileName=szName;
					pEntry->strPathName=GetWorkingDirectory();
					pEntry->strPathName+="\\";
				}
				pEntry->Children.push_back(pSubEntry);
			}
		}
	}

	for(int i=0;i<list.GetFileCount();i++)
	{
		if(!list.IsDirectory(i))
		{
			if(!pEntry)
			{
				pEntry=new FileEntry;
				pEntry->strFileName=szName;
				pEntry->strPathName=GetWorkingDirectory();
			}

			FileEntry *pFile=new FileEntry;
			pFile->strFileName=list.GetFileName(i);
			pFile->strPathName=strCurrentPath;
			pEntry->Children.push_back(pFile);
		}
	}

	return pEntry;
}

const std::vector<std::string> *FileSystem::GetFilters(FileSystemType_ type)
{
	if(type >= 0 && type < FILE_COUNT)
		return &m_Extensions[type];
	else
		return 0;
}

FileEntry *FileSystem::GetFileList(FileSystemType_ type)
{
	if(type >= 0 && type < FILE_COUNT)
		return m_pEntries[type];
	else
		return 0;
}