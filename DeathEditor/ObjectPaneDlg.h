#pragma once
#include "afxcmn.h"

#include "MFC/CUpdateDialog.h"
#include <vector>

class CDeathEditorDoc;

namespace Game
{
class IObject;
}

// Boîte de dialogue CObjectPaneDlg

class CObjectPaneDlg : public CUpdateDialog
{
	DECLARE_DYNAMIC(CObjectPaneDlg)

public:
	CObjectPaneDlg(CDeathEditorDoc *pDoc, CWnd* pParent = NULL);   // constructeur standard
	virtual ~CObjectPaneDlg();
	virtual BOOL OnInitDialog();

	void Update(int iType, void *pParam=0);

// Données de boîte de dialogue
	enum { IDD = IDD_OBJECTS_PANE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	class TreeItem
	{
		public:

			HTREEITEM hTree;
			Game::IObject *pObject;

	};

	std::vector<TreeItem> m_Items;
	CImageList m_ImageList;

	int GetImageNumber(Game::IObject *pObj);
	HTREEITEM GetTreeParent(Game::IObject *pObj);

	HTREEITEM m_hMap;
	HTREEITEM m_hMeshes;
	HTREEITEM m_hMapObjects;

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
	afx_msg void OnNMClickObjTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkObjTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedObjTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickObjTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturnObjTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnObjRemove();
	afx_msg void OnObjProp();
};
