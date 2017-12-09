// SpaceEditor.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include "SpaceEditor.h"
#include "MainFrm.h"

#include "SpaceEditorDoc.h"
#include "SpaceEditorView.h"

#include "../DeathEditor/Interfaces.h"
#include "../DeathEditor/FileSystem.h"
#include "../DeathEditor/Data/MeshLoader.h"

#include "Tools/ToolManager.h"
#include "Selector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceEditorApp

BEGIN_MESSAGE_MAP(CSpaceEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSpaceEditorApp::OnAppAbout)
	// Commandes de fichier standard
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// construction CSpaceEditorApp

CSpaceEditorApp::CSpaceEditorApp()
{
	// TODO : ajoutez ici du code de construction,
	// Placez toutes les initialisations significatives dans InitInstance
}


// Seul et unique objet CSpaceEditorApp

CSpaceEditorApp theApp;


// initialisation de CSpaceEditorApp

BOOL CSpaceEditorApp::InitInstance()
{
	new FileSystem;
	g_FileSystem.ParseDirectories();

	// InitCommonControlsEx() est requis sur Windows XP si le manifeste de l'application
	// spécifie l'utilisation de ComCtl32.dll version 6 ou ultérieure pour activer les
	// styles visuels.  Dans le cas contraire, la création de fenêtres échouera.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// À définir pour inclure toutes les classes de contrôles communs à utiliser
	// dans votre application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	new Interfaces;
	if(!g_Interfaces.Load(m_hInstance))
		return FALSE;

	new ToolMgr;
	new MeshLoader;
	new Selector;

	g_MeshLoader.Init();

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSpaceEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // fenêtre frame SDI principale
		RUNTIME_CLASS(CSpaceEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Activer les ouvertures d'exécution DDE
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Analyser la ligne de commande pour les commandes shell standard, DDE, ouverture de fichiers
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Commandes de dispatch spécifiées sur la ligne de commande. Retournent FALSE si
	// l'application a été lancée avec /RegServer, /Register, /Unregserver ou /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// La seule fenêtre a été initialisée et peut donc être affichée et mise à jour
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// appelle DragAcceptFiles uniquement s'il y a un suffixe
	//  Dans une application SDI, cet appel doit avoir lieu juste après ProcessShellCommand
	// Activer les ouvertures via glisser-déplacer
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}



// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

// Implémentation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// Commande App pour exécuter la boîte de dialogue
void CSpaceEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// gestionnaires de messages pour CSpaceEditorApp

