// DeathEditorDoc.h : interface de la classe CDeathEditorDoc
//

#include "Interfaces.h"

#pragma once

#define EDITOR_MAP				0

//  Views update

#define UPDATE_HEIGHTMAPCREATED				1
#define UPDATE_NEWMAP						2
#define UPDATE_OPENMAP						3
#define UPDATE_MAPCREATED					4

#define UPDATE_NEWOBJECT					5
#define UPDATE_MODIFYOBJECT					6
#define UPDATE_REMOVEOBJECT					7

#define UPDATE_RESET						8

class CDeathEditorDoc : public CDocument
{
protected: // création à partir de la sérialisation uniquement
	CDeathEditorDoc();
	DECLARE_DYNCREATE(CDeathEditorDoc)

public:

	int GetCurrentEditor() const { return m_iCurrentEditor; }
	void SetCurrentEditor(const int iEditor);
	void EnableMenus(bool bEnable);
	void SetDocumentName(const CString& strName);


// Substitutions
public:
	virtual BOOL OnNewDocument();

// Implémentation
public:
	virtual ~CDeathEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	int m_iCurrentEditor;
	int m_iCurrentMapState;
	CString m_strDocName;

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNewmap();
	afx_msg void OnOpenmap();
	afx_msg void OnQuit();
	afx_msg void OnSave();
	afx_msg void OnTestTester();
};


