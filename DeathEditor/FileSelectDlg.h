#pragma once

#include "afxcmn.h"
#include "FileSystem.h"

#include <map>


// Boîte de dialogue CFileSelectDlg

class CFileSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileSelectDlg)

public:
	CFileSelectDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CFileSelectDlg();
	virtual BOOL OnInitDialog();
	
	void SetFileType(FileSystemType_ type) { m_Type=type; }

	CString m_strFileName;
	CString m_strPathName;

// Données de boîte de dialogue
	enum { IDD = IDD_FILESELECTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	void RebuildTree();
	void RecursRebuildTree(FileEntry *pEntry, HTREEITEM hParent);

	FileSystemType_ m_Type;
	FileEntry *m_pRoot;
	FileEntry *m_pSelected;
	std::map<HTREEITEM,FileEntry*> m_TreeMap;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTvnSelchangedFileTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedFileNew();
	afx_msg void OnBnClickedFileImport();
	afx_msg void OnBnClickedFileRefresh();
	CTreeCtrl m_Tree;
	afx_msg void OnBnClickedFileEditbtn();
};
