// DeathEditorDoc.cpp : implémentation de la classe CDeathEditorDoc
//

#include "stdafx.h"
#include "DeathEditor.h"

#include "DeathEditorDoc.h"

#include "NewMap.h"

#include "FileSystem.h"
#include "WindowMgr.h"

#include "Test.h"

#include "../DeathEngine/Export/IMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeathEditorDoc

IMPLEMENT_DYNCREATE(CDeathEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CDeathEditorDoc, CDocument)
	ON_COMMAND(ID_NEWMAP, &CDeathEditorDoc::OnNewmap)
	ON_COMMAND(ID_OPENMAP, &CDeathEditorDoc::OnOpenmap)
	ON_COMMAND(ID_QUIT, &CDeathEditorDoc::OnQuit)
	ON_COMMAND(ID_SAVE, &CDeathEditorDoc::OnSave)
	ON_COMMAND(ID_TEST_TESTER, &CDeathEditorDoc::OnTestTester)
END_MESSAGE_MAP()


// construction ou destruction de CDeathEditorDoc

CDeathEditorDoc::CDeathEditorDoc() : m_iCurrentEditor(-1),m_strDocName("")
{
	// TODO : ajoutez ici le code d'une construction unique

}

CDeathEditorDoc::~CDeathEditorDoc()
{
}

BOOL CDeathEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	g_WindowMgr.SetDoc(this);

	return TRUE;
}


// diagnostics pour CDeathEditorDoc

#ifdef _DEBUG
void CDeathEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDeathEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// commandes pour CDeathEditorDoc

void CDeathEditorDoc::OnNewmap()
{
	if(m_iCurrentEditor!=-1)
	{
		if(MessageBox(0,"Etes vous sur d'abandonner le travail en cours?","Nouvelle map",MB_YESNO | MB_ICONQUESTION)==IDNO)
			return;
	}

	UpdateAllViews(0,UPDATE_RESET);

	g_WindowMgr.ShowDialog(WT_NEWMAP);
	SetCurrentEditor(EDITOR_MAP);
}

void CDeathEditorDoc::SetCurrentEditor(const int iEditor)
{
	m_iCurrentEditor=iEditor;
}

void CDeathEditorDoc::SetDocumentName(const CString& strName)
{
	m_strDocName=strName;

	CString str("DeathEditor - ");
	str+=m_strDocName;
	theApp.GetMainWnd()->SetWindowText(str);
}

void CDeathEditorDoc::OnOpenmap()
{
	if(m_iCurrentEditor!=-1)
	{
		if(MessageBox(0,"Etes vous sur d'abandonner le travail en cours?","Nouvelle map",MB_YESNO | MB_ICONQUESTION)==IDNO)
			return;
	}

	g_FileSystem.ChangeWorkingDirectory("Maps\\");
	CString strPath2(g_FileSystem.GetWorkingDirectory());
	strPath2+="\\*.dmap";
	CFileDialog dialog(TRUE,"*.dmap",strPath2,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"DeathChase map (*.dmap)|*.dmap||");
	g_FileSystem.ResetWorkingDirectory();
	if(dialog.DoModal()==IDCANCEL)
		return;

	UpdateAllViews(0,UPDATE_RESET);

	CString strFullName(dialog.GetPathName());
	CString strFileName(dialog.GetFileName());
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+="\\";

	g_FileSystem.ResetWorkingDirectory();
	if(!g_Map->Load(dialog.GetFileTitle(),strPath))
	{
		MessageBox(0,"Erreur lors du chargement de la map.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}

	SetCurrentEditor(EDITOR_MAP);
	SetDocumentName(CString(g_Map->GetName()));

	UpdateAllViews(0,UPDATE_OPENMAP);
}

void CDeathEditorDoc::OnQuit()
{
	if(m_iCurrentEditor!=-1)
	{
		if(MessageBox(0,"Etes vous sur de vouloir quitter le travail en cours?","Quitter",MB_YESNO | MB_ICONQUESTION)==IDNO)
			return;
	}

	PostQuitMessage(0);
}

void CDeathEditorDoc::OnSave()
{
	g_FileSystem.ResetWorkingDirectory();

	switch(m_iCurrentEditor)
	{
		case EDITOR_MAP:
			if(g_Map->Save())
			{
				CString strFile("Maps\\");
				strFile+=g_Map->GetName();
				strFile+=".dmap";
				CString strMsg="La map a été sauvegardée sous ";
				strMsg+=strFile;
				strMsg+=".";
				MessageBox(0,strMsg,"Sauvegarde",MB_OK);
			}
			else
				MessageBox(0,"Erreur lors de la sauvegarde de la map.","Erreur",MB_OK | MB_ICONERROR);
			break;
		default:
			break;
	}
}

void CDeathEditorDoc::EnableMenus(bool bEnable)
{
	UINT nCmd=bEnable ? (MF_BYPOSITION | MF_ENABLED) : (MF_BYPOSITION | MF_DISABLED);

	CMenu *pMenu=AfxGetMainWnd()->GetMenu();
	if(pMenu)
	{
		for(int i=0;i<pMenu->GetMenuItemCount();i++)
		{
			if(i!=1 && i!=pMenu->GetMenuItemCount()-1) // Correspond aux commandes de vue et dev
				pMenu->EnableMenuItem(i,nCmd);
		}
	}
}

void CDeathEditorDoc::OnTestTester()
{
	if(m_iCurrentEditor==EDITOR_MAP)
	{
		HANDLE hStartEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	
		DWORD dwThreadID;
		HANDLE hThread=CreateThread(NULL,NULL,TestThread,(LPVOID)hStartEvent,0,&dwThreadID);
	
		if(!g_Engine->StartMapTest("F16.dship","Ships\\F16\\",dwThreadID))
			TerminateThread(hThread,0);
		else
			SetEvent(hStartEvent);
	}
}
