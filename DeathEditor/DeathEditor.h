// DeathEditor.h : fichier d'en-tête principal pour l'application DeathEditor
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux


// CDeathEditorApp:
// Consultez DeathEditor.cpp pour l'implémentation de cette classe
//

class CDeathEditorApp : public CWinApp
{
public:
	CDeathEditorApp();


// Substitutions
public:
	virtual BOOL InitInstance();

// Implémentation
	DECLARE_MESSAGE_MAP()
};

extern CDeathEditorApp theApp;

#define IMAGE_EXTENSIONS	"Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|TARGA (*.tga)|*.tga|PNG (*.png)|*.png||"
#define IMAGE_EXTPATH		"\\*.bmp; *.jpg; *.tga; *.png"