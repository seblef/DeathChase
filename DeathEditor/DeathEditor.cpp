// DeathEditor.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "MainFrm.h"

#include "DeathEditorDoc.h"
#include "DeathEditorView.h"

#include "Interfaces.h"
#include "FileSystem.h"
#include "WindowMgr.h"

#include "Data/MeshLoader.h"
#include "Data/Selector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeathEditorApp

BEGIN_MESSAGE_MAP(CDeathEditorApp, CWinApp)
END_MESSAGE_MAP()


// construction CDeathEditorApp

CDeathEditorApp::CDeathEditorApp()
{
	// TODO : ajoutez ici du code de construction,
	// Placez toutes les initialisations significatives dans InitInstance
}


// Seul et unique objet CDeathEditorApp

CDeathEditorApp theApp;


// initialisation de CDeathEditorApp

BOOL CDeathEditorApp::InitInstance()
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

	new WindowMgr;
	new MeshLoader;
	new Selector;

	// Initialiser les bibliothèques OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	g_MeshLoader.Init();
	
	// Inscrire les modèles de document de l'application. Ces modèles
	//  lient les documents, fenêtres frame et vues entre eux
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDeathEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // fenêtre frame SDI principale
		RUNTIME_CLASS(CDeathEditorView));
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

	return TRUE;
}

// gestionnaires de messages pour CDeathEditorApp

