
#ifndef _CUPDATEDIALOG_H_
#define _CUPDATEDIALOG_H_

#include "../stdafx.h"
#include "../DeathEditorDoc.h"

class CUpdateDialog : public CDialog
{
	public:

		CUpdateDialog(CDeathEditorDoc *pDoc) : m_pDoc(pDoc) {}
		CUpdateDialog(CDeathEditorDoc *pDoc,UINT dwID, CWnd *pParent=NULL) :
			m_pDoc(pDoc), CDialog(dwID,pParent) {}
		CUpdateDialog(CDeathEditorDoc *pDoc,const char *szTemplate, CWnd *pParent=NULL) :
			m_pDoc(pDoc), CDialog(szTemplate,pParent) {}
		virtual ~CUpdateDialog() {}

		virtual void Update(int iType, void *pParam=0)=0;

	protected:

		CDeathEditorDoc *m_pDoc;
};

#endif