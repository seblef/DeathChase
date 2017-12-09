
#ifndef _IFACTORY_H_
#define _IFACTORY_H_

#include "IExportObj.h"

class IFactory
{
	public:

		IFactory() {}
		virtual ~IFactory() {}

		virtual IExportObject *CreateObject(ExportObjectType _Type)=0;
		virtual void ReleaseObject(IExportObject *pObj)=0;
};

#endif