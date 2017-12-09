
#ifndef _ICONFIG_H_
#define _ICONFIG_H_

#include "../Core/Vector.h"
#include "../Core/Color.h"

class IConfig
{
	public:

		IConfig() {}
		virtual ~IConfig() {}

		virtual bool Save(const char *szFileName)=0;

		virtual int GetIParameter(const char *szName, int iDefault)=0;
		virtual float GetFParameter(const char *szName, float fDefault)=0;
		virtual bool GetBParameter(const char *szName, bool bDefault)=0;
		virtual const char *GetSParameter(const char *szName, const char *szDefault)=0;
		virtual const Core::Vector3& GetVParameter(const char *szName,
			const Core::Vector3& vDefault=Core::Vector3::NullVector)=0;
		virtual const Core::Color32& GetCParameter(const char *szName,
			const Core::Color32& cDefault=Core::Color32(255,255,255,255))=0;

		virtual void SetIParameter(const char *szName, int iValue)=0;
		virtual void SetFParameter(const char *szName, float fValue)=0;
		virtual void SetBParameter(const char *szName, bool bValue)=0;
		virtual void SetSParameter(const char *szName, const char *szValue)=0;
		virtual void SetVParameter(const char *szName, const Core::Vector3& vValue)=0;
		virtual void SetCParameter(const char *szName, const Core::Color32& cValue)=0;
};

#endif