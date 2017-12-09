

#include "Config.h"
#include "ScriptFile.h"
#include "../ConvertChar.h"

namespace Core
{

const char *szParamTypeIndex[]={"I","F","B","S","V","C"};
const char *szParamTypeNames[]={ "int","float","bool","string","vector","color" };

Config::Config()
{
}

void Config::Destroy()
{
	m_Data.clear();
}

bool Config::GetBParameter(const char *szName, bool bDefault)
{
	Parameter *p=GetParam(szName,PARAM_BOOL);
	return p ? p->GetBValue_() : bDefault;
}

const Color32& Config::GetCParameter(const char *szName, const Core::Color32 &cDefault)
{
	Parameter *p=GetParam(szName,PARAM_COLOR);
	return p ? p->GetCValue_() : cDefault;
}

float Config::GetFParameter(const char *szName, float fDefault)
{
	Parameter *p=GetParam(szName,PARAM_FLOAT);
	return p ? p->GetFValue() : fDefault;
}

int Config::GetIParameter(const char *szName, int iDefault)
{
	Parameter *p=GetParam(szName,PARAM_INT);
	return p ? p->GetIValue() : iDefault;
}

const char *Config::GetSParameter(const char *szName, const char *szDefault)
{
	Parameter *p=GetParam(szName,PARAM_STRING);
	return p ? p->GetSValue() : szDefault;
}

const Vector3& Config::GetVParameter(const char *szName, const Core::Vector3 &vDefault)
{
	Parameter *p=GetParam(szName,PARAM_VECTOR);
	return p ? p->GetVValue() : vDefault;
}

Config::Parameter *Config::GetParam(const char *szName, Type type)
{
	static std::string strIndexName;
	strIndexName=szName;
	strIndexName+=szParamTypeIndex[type];

	ParamMap::iterator i=m_Data.find(strIndexName);
	return i!=m_Data.end() ? &i->second : 0;
}

bool Config::RemoveParameter(const char *szName, Type type)
{
	static std::string strIndexName;
	strIndexName=szName;
	strIndexName+=szParamTypeIndex[type];

	ParamMap::iterator i=m_Data.find(strIndexName);
	if(i!=m_Data.end())
	{
		m_Data.erase(i);
		return true;
	}
	else
		return false;
}

void Config::AddParameter(Parameter &p)
{
	static std::string strIndexName;
	strIndexName=p.GetName();
	strIndexName+=szParamTypeIndex[p.GetType()];

	ParamMap::iterator i=m_Data.find(strIndexName);
	if(i==m_Data.end())
		m_Data.insert(std::make_pair(strIndexName,p));
	else
		i->second=p;
}

void Config::SetBParameter(const char *szName, bool bValue)
{
	Parameter *p=GetParam(szName,PARAM_BOOL);
	if(p)
		p->SetBValue(bValue);
	else
	{
		Parameter np(bValue,szName);
		AddParameter(np);
	}
}

void Config::SetCParameter(const char *szName, const Color32& cValue)
{
	Parameter *p=GetParam(szName,PARAM_COLOR);
	if(p)
		p->SetCValue(cValue);
	else
	{
		Parameter np(cValue,szName);
		AddParameter(np);
	}
}

void Config::SetFParameter(const char *szName, float fValue)
{
	Parameter *p=GetParam(szName,PARAM_FLOAT);
	if(p)
		p->SetFValue(fValue);
	else
	{
		Parameter np(fValue,szName);
		AddParameter(np);
	}
}

void Config::SetIParameter(const char *szName, int iValue)
{
	Parameter *p=GetParam(szName,PARAM_INT);
	if(p)
		p->SetIValue(iValue);
	else
	{
		Parameter np(iValue,szName);
		AddParameter(np);
	}
}

void Config::SetSParameter(const char *szName, const char *szValue)
{
	Parameter *p=GetParam(szName,PARAM_STRING);
	if(p)
		p->SetSValue(szValue);
	else
	{
		Parameter np(szValue,szName);
		AddParameter(np);
	}
}

void Config::SetVParameter(const char *szName, const Vector3& vValue)
{
	Parameter *p=GetParam(szName,PARAM_VECTOR);
	if(p)
		p->SetVValue(vValue);
	else
	{
		Parameter np(vValue,szName);
		AddParameter(np);
	}
}

bool Config::ParseFile(const char *szFileName)
{
	ScriptFile file;
	if(!file.Open(szFileName))
	{
		g_Output->AddMsg("x Impossible d'ouvrir le fichier de configuration %s\n",szFileName);
		return false;
	}

	std::string szTokenType=file.GetToken();
	std::string szTokenName=file.GetToken();
	const char *szToken=0;
	while(szTokenType.length() > 0 && szTokenName.length() > 0)
	{
		if(szTokenType=="bool")
		{
			szToken=file.GetToken();
			if(szToken)
			{
				Parameter np(false,szTokenName.c_str());
				if(strcmp(szToken,"true")==0) np.SetBValue(true);
				else if(strcmp(szToken,"false")!=0) g_Output->AddMsg("x Valeur de la variable bool [%s] invalide dans le fichier de configuration %s\n",szTokenName.c_str(),szFileName);
				AddParameter(np);
			}
			else
				g_Output->AddMsg("x Fin de fichier inattendue dans le fichier de configuration %s\n",szFileName);
		}
		else if(szTokenType=="int")
		{
			szToken=file.GetToken();
			if(szToken)
			{
				Parameter np(atoi(szToken),szTokenName.c_str());
				AddParameter(np);
			}
			else
				g_Output->AddMsg("x Fin de fichier inattendue dans le fichier de configuration %s\n",szFileName);
		}
		else if(szTokenType=="float")
		{
			szToken=file.GetToken();
			if(szToken)
			{
				Parameter np((float)atof(szToken),szTokenName.c_str());
				AddParameter(np);
			}
			else
				g_Output->AddMsg("x Fin de fichier inattendue dans le fichier de configuration %s\n",szFileName);
		}
		else if(szTokenType=="string")
		{
			szToken=file.GetToken();
			if(szToken)
			{
				Parameter np(szToken,szTokenName.c_str());
				AddParameter(np);
			}
			else
				g_Output->AddMsg("x Fin de fichier inattendue dans le fichier de configuration %s\n",szFileName);
		}
		else if(szTokenType=="color")
		{
			Color32 c;
			if(file.ParseColor(c))
			{
				Parameter np(c,szTokenName.c_str());
				AddParameter(np);
			}
			else
				g_Output->AddMsg("x Fin de fichier inattendue dans le fichier de configuration %s\n",szFileName);
		}
		else if(szTokenType=="vector")
		{
			Vector3 v;
			if(file.ParseVector(v))
			{
				Parameter np(v,szTokenName.c_str());
				AddParameter(np);
			}
			else
				g_Output->AddMsg("x Fin de fichier inattendue dans le fichier de configuration %s\n",szFileName);
		}
		else
			g_Output->AddMsg("x Token [%s] inconnu dans le fichier de configuration %s\n",szTokenType.c_str(),szFileName);

		szToken=file.GetToken();
		szTokenType=szToken ? szToken : "";
		szToken=file.GetToken();
		szTokenName=szToken ? szToken : "";
	}	

	file.Close();

	return true;
}

bool Config::Save(const char *szFileName)
{
	MYASSERT(szFileName,"Mauvais nom de fichier",true);

	FILE *file=fopen(szFileName,"w");
	if(!file)
	{
		g_Output->AddMsg("x Erreur lors de l'ouverture/création du fichier de configuration %s\n",szFileName);
		return false;
	}

	std::string strLine;

	ParamMap::iterator i;
	for(i=m_Data.begin();i!=m_Data.end();i++)
	{
		Parameter &p=i->second;

		strLine.clear();
		strLine=szParamTypeNames[p.GetType()];
		strLine+=' ';
		strLine+=p.GetName();
		strLine+=' ';

		switch(p.GetType())
		{
			case PARAM_BOOL:
				strLine+=p.GetBValue_() ? "true" : "false";
				break;

			case PARAM_COLOR:
				strLine+=IntToChar(p.GetCValue_().a);
				strLine+=" ";
				strLine+=IntToChar(p.GetCValue_().r);
				strLine+=" ";
				strLine+=IntToChar(p.GetCValue_().g);
				strLine+=" ";
				strLine+=IntToChar(p.GetCValue_().b);
				break;

			case PARAM_FLOAT:
				strLine+=FloatToChar(p.GetFValue());
				break;

			case PARAM_INT:
				strLine+=IntToChar(p.GetIValue());
				break;

			case PARAM_STRING:
				strLine+=p.GetSValue();
				break;

			case PARAM_VECTOR:
				strLine+=FloatToChar(p.GetVValue().x);
				strLine+=" ";
				strLine+=FloatToChar(p.GetVValue().y);
				strLine+=" ";
				strLine+=FloatToChar(p.GetVValue().z);
				break;

			default:
				break;
		}

		strLine+="\n";
		fprintf(file,strLine.c_str());
	}

	fclose(file);
	return true;
}

}
