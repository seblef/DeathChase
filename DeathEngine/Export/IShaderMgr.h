
#ifndef _ISHADERMGR_H_
#define _ISHADERMGR_H_

#include "../Core/Handles.h"

class IShader;

class IShaderMgr
{
	public:

		IShaderMgr() {}
		virtual ~IShaderMgr() {}

		virtual Core::HShader LoadShader(const char *szName)=0;
		virtual void ReleaseShader(Core::HShader &hShader)=0;

		virtual bool SetShader(Core::HShader hShader)=0;

		virtual IShader *GetShader(Core::HShader hShader)=0;
};

#endif
