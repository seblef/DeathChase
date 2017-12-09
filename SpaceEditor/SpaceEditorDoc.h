// SpaceEditorDoc.h : interface de la classe CSpaceEditorDoc
//


#pragma once

#include "../../Loaders/SDK/Iloader.h"
#include "../DeathEngine/Core/Handles.h"
#include "SObject.h"
#include <vector>

#define	UPDATE_SHIPCREATED					1
#define UPDATE_RESET						2


class CSpaceEditorDoc : public CDocument
{
protected: // création à partir de la sérialisation uniquement
	CSpaceEditorDoc();
	DECLARE_DYNCREATE(CSpaceEditorDoc)

// Attributs
public:

	std::vector<Core::HMesh> m_Meshes;

// Opérations
public:

// Substitutions
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	std::vector<SObject*> m_Objects;

// Implémentation
public:
	virtual ~CSpaceEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	bool m_bCreated;

	void BuildMeshes(ILoader *pLoader);

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFichierNouveauvaisseau();
	afx_msg void OnFichierOuvrirvaisseau();
	afx_msg void OnFichierQuitter();
	afx_msg void OnFichierSauvegarder();
	afx_msg void OnTestTester();
};


