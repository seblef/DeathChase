// DeathEditor.h�: fichier d'en-t�te principal pour l'application DeathEditor
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux


// CDeathEditorApp:
// Consultez DeathEditor.cpp pour l'impl�mentation de cette classe
//

class CDeathEditorApp : public CWinApp
{
public:
	CDeathEditorApp();


// Substitutions
public:
	virtual BOOL InitInstance();

// Impl�mentation
	DECLARE_MESSAGE_MAP()
};

extern CDeathEditorApp theApp;

#define IMAGE_EXTENSIONS	"Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|TARGA (*.tga)|*.tga|PNG (*.png)|*.png||"
#define IMAGE_EXTPATH		"\\*.bmp; *.jpg; *.tga; *.png"