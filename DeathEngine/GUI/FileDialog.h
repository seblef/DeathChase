
#ifndef _FILEDIALOG_H_

#define _FILEDIALOG_H_

#include "IFileDialog.h"
#include "IButton.h"
#include "IListBox.h"
#include "../Core/FileList.h"
#include "../Dev/MemDebugger.h"

namespace GUI
{

class FileDialog : public IFileDialog
{
	public:

		FileDialog(const char *szTitle, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser,uint32 dwID);
		virtual ~FileDialog();

		virtual void Reset(const char *szTitle, IEnvironment *pEnvironment, IElement *pParent,
			IEventReceiver *pUser, uint32 dwID);
		virtual void Clear();

		virtual const char *GetFileName() const;
		virtual const char *GetPath() const;

		virtual void AddExtFilter(const char *szFilter);

		virtual bool OnEvent(const Event& _Event);
		virtual void Draw();

		virtual void SetNoDirs(const bool bNoDirs);
		virtual void SetPath(const char *szPath);

		void Remove();

		MEMDEBUG_IMPL
		MEMDEBUG_CLASSNAME("FileDialog")
		MEMDEBUG_CLASSSIZE(sizeof(FileDialog))

	protected:

		void FillListBox();
		void SendSelectedEvent();
		void SendCancelEvent();

		Core::Vector2i m_vDragStart;
		std::string m_strFileName;
		std::string m_strPath;
		bool m_bDragging;
		bool m_bNoDirs;

		IButton *m_pClose;
		IButton *m_pOK;
		IButton *m_pCancel;
		IListBox *m_pFileBox;
		IElement *m_pFileNameText;
		IElement *m_pEventParent;

		Core::FileList *m_pFileList;

		std::vector<std::string> m_Filters;
};

}

#endif
