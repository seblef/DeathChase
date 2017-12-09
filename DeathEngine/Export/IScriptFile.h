
#ifndef _ISCRIPTFILE_H_
#define _ISCRIPTFILE_H_

#include "../Core/Vector.h"
#include "../Core/Color.h"
#include "IExportObj.h"

class IScriptFile : public IExportObject
{
	public:

		IScriptFile()  : IExportObject(EOBJ_SCRIPTFILE) {}
		virtual ~IScriptFile() {}

		virtual bool Open(const char *szFile)=0;
		virtual void Close()=0;
		virtual const char *GetFileName() const=0;
		
		virtual const char *GetToken()=0;
		virtual bool ParseColor(Core::Color32& c)=0;
		virtual bool ParseVector(Core::Vector3& v)=0;
};

#endif