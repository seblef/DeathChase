
#include "FileList.h"
#include <stdlib.h>
#include <io.h>
#include <direct.h>

FileList::FileList()
{
	Reset(0);
}

uint32 FileList::GetFileCount() const
{
	return m_Files.size();
}

const char *FileList::GetFileName(uint32 dwIndex) const
{
	if(dwIndex < m_Files.size())
		return m_Files[dwIndex].strName.c_str();
	else
		return 0;
}

const char *FileList::GetFullFileName(uint32 dwIndex)
{
	if(dwIndex >= m_Files.size())
		return 0;

	if(m_Files[dwIndex].strFullName.size() < m_Files[dwIndex].strName.size())
	{
		m_Files[dwIndex].strFullName=m_strPath;

		if(m_strPath.size() > 3)
			m_Files[dwIndex].strFullName.append("/");

		m_Files[dwIndex].strFullName.append(m_Files[dwIndex].strName);
	}

	return m_Files[dwIndex].strFullName.c_str();
}

bool FileList::IsDirectory(uint32 dwIndex) const
{
	if(dwIndex >= m_Files.size())
		return false;
	else
		return m_Files[dwIndex].bDirectory;
}

void FileList::Reset(std::vector<std::string> *pFilters, bool bNoDirs)
{
	m_Files.clear();

	char tmp[_MAX_PATH];
	_getcwd(tmp,_MAX_PATH);
	m_strPath=tmp;

	struct _finddata_t c_file;
	long hFile;
	FileEntry entry;

	if((hFile=_findfirst("*",&c_file))!=-1L)
	{
		if(!pFilters || pFilters->size()==0)
		{
			do
			{
				entry.bDirectory=(_A_SUBDIR & c_file.attrib)!=0;

				if(!bNoDirs)
				{
					entry.strName=c_file.name;
					entry.dwSize=c_file.size;
					m_Files.push_back(entry);
				}
			}
			while(_findnext(hFile,&c_file)==0);
		}
		else
		{
			do
			{
				bool bAdd=false;
				entry.bDirectory=(_A_SUBDIR & c_file.attrib)!=0;
				if(entry.bDirectory && !bNoDirs)
					bAdd=true;
				else
				{
					std::string strName(c_file.name);
					int name_len=strName.length();
					int j,filter_len;

					for(j=0;j<pFilters->size();j++)
					{
						filter_len=(*pFilters)[j].length();
						if(name_len > filter_len)
						{
							if(strName.substr(name_len-filter_len,filter_len)==(*pFilters)[j])
								bAdd=true;
						}
					}
				}

				if(bAdd)
				{
					entry.strName=c_file.name;
					entry.dwSize=c_file.size;
					m_Files.push_back(entry);
				}
			}
			while(_findnext(hFile,&c_file)==0);
		}

		_findclose(hFile);
	}
}
