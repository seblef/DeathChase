
#ifndef _CTABDLG_H_
#define _CTABDLG_H_

#include "CUpdateDialog.h"
#include <vector>

class CTabDlg : public CTabCtrl
{
	public:

		CTabDlg();
		~CTabDlg();

		bool AddView(CUpdateDialog *_pDialog,
					 DWORD _dwDialogID,
					 const char *_szTabName);
		bool RemoveView(const int iIndex,
						const bool _bDestroyDialog=true);
		bool SetCurrentView(const int iIndex);
		int GetViewCount() const { return m_Views.size(); }
		void Update(int iType, void *pParam);
		void Clear();

	protected:

		//{{AFX_MSG(CKTabCtrl)
		afx_msg void OnSize(UINT nType, int cx, int cy );
		afx_msg void OnMove(int cx, int cy);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		std::vector<CUpdateDialog *> m_Views;
		int m_iLast;

};

#endif