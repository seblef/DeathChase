
#ifndef _IMAGEFILEDIALOG_H_
#define _IMAGEFILEDIALOG_H_

#include "FileDialog.h"

namespace GUI
{

class ImageFileDialog : public FileDialog
{
	public:

		ImageFileDialog(const char *szTitle, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser,uint32 dwID);
		virtual ~ImageFileDialog();

		virtual void Reset(const char *szTitle, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser, uint32 dwID);
		virtual void Clear();

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("ImageFileDialog")
		MEMDEBUG_CLASSSIZE(sizeof(ImageFileDialog))

	protected:

		Core::HTexture m_hTexture;
		bool LoadTexture();
};

}

#endif