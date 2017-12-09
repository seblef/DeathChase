
#include "FileSystem.h"
#include "TextOutput.h"
#include <direct.h>
#include <string>

namespace Core
{

FileSystem::FileSystem()
{
	strcpy(m_szStartDirectory,GetWorkingDirectory());
}

FileSystem::~FileSystem()
{
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
	if(!ChangeWorkingDirectory(m_szStartDirectory))
		g_Output->AddMsg("x Impossible de reinitialiser le repertoire courant\n");
}

bool FileSystem::CreateDirectory_(const char *szDir)
{
	_mkdir(szDir);
	return true;
}

bool FileSystem::ExtractPath(const char *szFileName, char *szPathName)
{
	if(!szFileName || !szPathName) return false;

	std::string strFileName(szFileName);
	const char *c=strFileName.c_str();
	for(int i=strFileName.length()-1;i>=0;i--)
		if(c[i]=='\\')
		{
			strcpy(szPathName,strFileName.substr(0,i).c_str());
			return true;
		}

	return false;
}

void FileSystem::ExtractFileName(const char *szPathName, char *szFileName)
{
	if(!szPathName || !szFileName) return;

	std::string strPathName(szPathName);
	const char *c=strPathName.c_str();
	const int iLength=strPathName.length();
	for(int i=iLength-1;i>=0;i--)
		if(c[i]=='\\')
		{
			strcpy(szFileName,strPathName.substr(i+1,iLength-i-1).c_str());
			return;
		}

	strcpy(szFileName,szPathName);
}

}