// SpaceEditor.h�: fichier d'en-t�te principal pour l'application SpaceEditor
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux


// CSpaceEditorApp:
// Consultez SpaceEditor.cpp pour l'impl�mentation de cette classe
//

class CSpaceEditorApp : public CWinApp
{
public:
	CSpaceEditorApp();


// Substitutions
public:
	virtual BOOL InitInstance();

// Impl�mentation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSpaceEditorApp theApp;