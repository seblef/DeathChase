
#ifndef _CONFIG_H_

#define _CONFIG_H_

#include <string>
#include "TSingleton.h"
#include <map>

#ifdef DEEXPORT
#include "../Export/IConfig.h"
#else
#include "Vector.h"
#include "Color.h"
#endif

namespace Core
{

#ifdef DEEXPORT
class Config : public IConfig, public TSingleton<Config>
#else
class Config : public TSingleton<Config>
#endif
{
    public:

		enum Type
		{
			PARAM_INT=0,
			PARAM_FLOAT,
			PARAM_BOOL,
			PARAM_STRING,
			PARAM_VECTOR,
			PARAM_COLOR,

			PARAM_COUNT
		};

		class Parameter
		{
			public:

				Parameter(Type type) : m_Type(type) {}
				Parameter(int i, const char *szName) : m_iValue(i), m_Type(PARAM_INT), m_strName(szName) {}
				Parameter(bool b, const char *szName) : m_bValue(b), m_Type(PARAM_BOOL), m_strName(szName) {}
				Parameter(float f, const char *szName) : m_fValue(f), m_Type(PARAM_FLOAT), m_strName(szName) {}
				Parameter(const char *sz, const char *szName) : m_strValue(sz), m_Type(PARAM_STRING), m_strName(szName) {}
				Parameter(const Vector3& v, const char *szName) : m_vValue(v), m_Type(PARAM_VECTOR), m_strName(szName) {}
				Parameter(const Color32& c, const char *szName) : m_cValue(c), m_Type(PARAM_COLOR), m_strName(szName) {}
				Parameter(const Parameter& param) : m_Type(param.m_Type), m_strName(param.m_strName)
				{
					switch(m_Type) {
						case PARAM_INT:    m_iValue=param.m_iValue; break;
						case PARAM_BOOL:   m_bValue=param.m_bValue; break;
						case PARAM_FLOAT:  m_fValue=param.m_fValue; break;
						case PARAM_STRING: m_strValue=param.m_strValue; break;
						case PARAM_VECTOR: m_vValue=param.m_vValue; break;
						case PARAM_COLOR:  m_cValue=param.m_cValue; break;
						default: MYASSERT(0,"Type de parametre invalide",true); break; }
				}
				~Parameter() {}

				Type GetType() const { return m_Type; }

				int GetIValue() const { return m_iValue; }
				void SetIValue(const int i) { m_iValue=i; }

				bool GetBValue_() const { return m_bValue; }
				void SetBValue(const bool b) { m_bValue=b; }

				float GetFValue() const { return m_fValue; }
				void SetFValue(const float f) { m_fValue=f; }

				const char *GetSValue() const { return m_strValue.c_str(); }
				void SetSValue(const char *sz) { m_strValue=sz; }

				const Color32& GetCValue_() const { return m_cValue; }
				void SetCValue(const Color32& c) { m_cValue=c; }

				const Vector3& GetVValue() const { return m_vValue; }
				void SetVValue(const Vector3& v) { m_vValue=v; }

				const char *GetName() const { return m_strName.c_str(); }
				void SetName(const char *szName) { m_strName=szName; }

				const Parameter& operator=(const Parameter& p)
				{
					m_Type=p.m_Type;
					m_strName=p.m_strName;
					
					switch(m_Type) {
						case PARAM_INT:    m_iValue=p.m_iValue; break;
						case PARAM_BOOL:   m_bValue=p.m_bValue; break;
						case PARAM_FLOAT:  m_fValue=p.m_fValue; break;
						case PARAM_STRING: m_strValue=p.m_strValue; break;
						case PARAM_VECTOR: m_vValue=p.m_vValue; break;
						case PARAM_COLOR:  m_cValue=p.m_cValue; break;
						default: MYASSERT(0,"Type de parametre invalide",true); break; }

					return *this;
				}
				bool operator==(const Parameter& p)
				{
					if(m_strName==p.m_strName && m_Type==p.m_Type)
					{
						switch(m_Type) {
							case PARAM_INT:    return m_iValue==p.m_iValue;
							case PARAM_BOOL:   return m_bValue==p.m_bValue;
							case PARAM_FLOAT:  return m_fValue=p.m_fValue;
							case PARAM_STRING: return m_strValue==p.m_strValue;
							case PARAM_VECTOR: return m_vValue==p.m_vValue;
							case PARAM_COLOR:  return m_cValue==p.m_cValue;
							default: MYASSERT(0,"Type de parametre invalide",true); return false; }
					}
					else
						return false;
				}

			protected:

				Type m_Type;
				std::string m_strName;
				std::string m_strValue;
				Vector3 m_vValue;
				Color32 m_cValue;
			
				union
				{
					int m_iValue;
					bool m_bValue;
					float m_fValue;
				};
		};

		Config();
		~Config() { Destroy(); }

		void Destroy();

		bool ParseFile(const char *szFileName);
		bool Save(const char *szFileName);

		int GetIParameter(const char *szName, int iDefault);
		float GetFParameter(const char *szName, float fDefault);
		bool GetBParameter(const char *szName, bool bDefault);
		const char *GetSParameter(const char *szName, const char *szDefault);
		const Vector3& GetVParameter(const char *szName, const Vector3& vDefault=Vector3::NullVector);
		const Color32& GetCParameter(const char *szName, const Color32& cDefault=Color32(255,255,255,255));

		void SetIParameter(const char *szName, int iValue);
		void SetFParameter(const char *szName, float fValue);
		void SetBParameter(const char *szName, bool bValue);
		void SetSParameter(const char *szName, const char *szValue);
		void SetVParameter(const char *szName, const Vector3& vValue);
		void SetCParameter(const char *szName, const Color32& cValue);

		bool RemoveParameter(const char *szName, Type type);

	private:

		Parameter *GetParam(const char *szName, Type type);
		void AddParameter(Parameter& p);

		typedef std::map<std::string,Parameter> ParamMap;		
		ParamMap m_Data;
};

}

#define g_Config			Core::Config::GetSingletonRef()

#endif
