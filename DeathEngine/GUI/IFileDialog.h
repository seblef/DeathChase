
#ifndef _IFILEDIALOG_H_

#define _IFILEDIALOG_H_

#include "IElement.h"

namespace GUI
{

class IFileDialog : public IElement
{
	public:

		IFileDialog(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect) :
		  IElement(ET_FILEDIALOG,pEnvironment,pParent,pUser,dwID,rect) {}
		virtual ~IFileDialog() {};

		virtual const char *GetFileName() const=0;
		virtual const char *GetPath() const=0;
		virtual void AddExtFilter(const char *szFilter)=0;
		virtual void SetNoDirs(const bool bNoDirs)=0;
		virtual void SetPath(const char *szPath)=0;
};

}

#endif