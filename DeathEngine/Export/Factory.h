
#ifndef _FACTORY_H_
#define _FACTORY_H_

#ifdef DEEXPORT

#include "IFactory.h"
#include "../Core/TSingleton.h"
#include "../Core/TMemoryAllocator.h"

class Factory : public IFactory, public Core::TSingleton<Factory>
{
	public:

		Factory();
		~Factory();

		IExportObject *CreateObject(ExportObjectType _Type);
		void ReleaseObject(IExportObject *pObj);

	private:

		Core::TMemoryAllocator<IExportObject> *m_Allocators[EOBJ_COUNT];
};

#endif
#endif
