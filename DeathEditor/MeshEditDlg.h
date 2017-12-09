#pragma once
#include "afxwin.h"

#include "../DeathEngine/Core/Handles.h"
#include "../DeathEngine/Common.h"

#include <vector>

class ILoader;
class ICamera;
class IMaterial;
class IMesh;
class LMaterial;

// Boîte de dialogue CMeshEditDlg

class CMeshEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CMeshEditDlg)

public:
	CMeshEditDlg(bool bImportLOD=false, CWnd* pParent = NULL);   // constructeur standard
	virtual ~CMeshEditDlg();
	BOOL OnInitDialog();

// Données de boîte de dialogue
	enum { IDD = IDD_MESHEDITDLG };

	Core::HMesh m_hMesh;
	IMesh *m_pMesh;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

	DECLARE_MESSAGE_MAP()

	void ConvertMaterial(IMaterial *pDest, LMaterial *pSrc);
	void Draw();
	void Release();

	void BuildMesh();
	void CombineMesh();
	void BuildLODs();
	void ConstructMesh(int iIndex, IMesh *pMesh,uint32 dwLODDist=0);

	bool m_bInited;
	bool m_bImportLOD;
	CString m_strPath;
	CString m_strFileName;
	ILoader *m_pLoader;
	ICamera *m_pCamera;
	std::vector<IMaterial *> m_Materials;
	std::vector<uint32> m_LODDistances;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeMeditMeshselectcb();
	afx_msg void OnBnClickedMeditMultiselectrd();
	afx_msg void OnBnClickedMeditMultiselectrd2();
	afx_msg void OnBnClickedMeditMultiselectrd3();
	afx_msg void OnEnChangeMeditLoddisteb();
	afx_msg void OnCbnSelchangeMeditMatselectcb();
	afx_msg void OnBnClickedMeditEditmatbtn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_MeshCb;
	BOOL m_bImportOnlyMesh;
	int m_iMultiAction;
	UINT m_dwLODDist;
	CComboBox m_MatCb;
	CStatic m_Preview;
	afx_msg void OnEnChangeMeditName();
	CString m_strName;
	afx_msg void OnPaint();
	afx_msg void OnBnClickedMeditAnimck();
	afx_msg void OnBnClickedMeditLodck();
	afx_msg void OnBnClickedMeditImportmatbtn();
	BOOL m_bImportAnim;
	BOOL m_bLODManage;
	BOOL m_bCenter;
	afx_msg void OnBnClickedMeditCenterck();
};
