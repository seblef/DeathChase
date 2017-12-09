
#include "ScriptFile.h"
#include <string>

namespace Core
{

ScriptFile::ScriptFile() : m_pFile(0)
{
	m_szFileName[0]='\0';
}

ScriptFile::~ScriptFile()
{
	if(m_pFile)
		fclose(m_pFile);
}

void ScriptFile::Clear()
{
	if(m_pFile)
		fclose(m_pFile);

	m_pFile=0;
	m_szFileName[0]='\0';
}

bool ScriptFile::Open(const char *szFile)
{
	Close();

	if(szFile)
	{
		if((m_pFile=fopen(szFile,"r"))==0)
			return false;

		strcpy(m_szFileName,szFile);
	}

	return true;
}

void ScriptFile::Close()
{
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile=0;
		m_szFileName[0]='\0';
	}
}

const char *ScriptFile::GetToken()
{
	if(!m_pFile)
		return 0;

	static std::string sToken;
	
	if(feof(m_pFile))
		return 0;
	else
	{
		char c=(char)fgetc(m_pFile);
		while((c==' ' || c=='	' || c=='\n') && !feof(m_pFile))
			c=(char)fgetc(m_pFile);
			
		if(feof(m_pFile))
			return 0;

		if(c=='\"')
			return GetText();
			
		if(c=='{' || c=='}')
		{
			sToken=c;
			return sToken.c_str();
		}
		
		sToken="";
		while(c!='\n' && c!=' ' && c!='	' && c!='{' && c!='}' && !feof(m_pFile))
		{
			sToken+=c;
			c=(char)fgetc(m_pFile);
		}
		
		// Commentaire
		if(sToken=="--")
		{
			c=(char)fgetc(m_pFile);
			while(c!='\n')
				c=(char)fgetc(m_pFile);
	
   			return GetToken();
		}
		else
			return sToken.c_str();
	}
}

const char *ScriptFile::GetText()
{
	static char szText[2048];
	int pos=0;

	char c=fgetc(m_pFile);
	while(!feof(m_pFile) && c!='\"' && pos < 2047)
	{
		szText[pos]=c;
		pos++;
		c=fgetc(m_pFile);
	}

	szText[pos]='\0';

	return szText;
}

bool ScriptFile::ParseColor(Core::Color32 &c)
{
	if(!m_pFile)
		return false;

	int argb[4];
	int i;
	for(i=0;i<4;i++)
	{
		const char *szToken=GetToken();
		if(!szToken)
			return false;

		argb[i]=atoi(szToken);
	}

	c=Color32((byte)argb[0],(byte)argb[1],(byte)argb[2],(byte)argb[3]);

	return true;
}

bool ScriptFile::ParseVector(Core::Vector3& v)
{
	if(!m_pFile)
		return false;

	float xyz[3];
	int i;
	for(i=0;i<3;i++)
	{
		const char *szToken=GetToken();
		if(!szToken)
			return false;

		xyz[i]=(float)atof(szToken);
	}

	v=Vector3(xyz[0],xyz[1],xyz[2]);

	return true;
}

}