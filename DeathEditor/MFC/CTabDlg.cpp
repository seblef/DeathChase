
#include "CTabDlg.h"

BEGIN_MESSAGE_MAP( CTabDlg, CTabCtrl)
    //{{AFX_MSG_MAP( CMyWnd
    //}}AFX_MSG_MAP
END_MESSAGE_MAP( )


CTabDlg::CTabDlg() : m_iLast(-1)
{
}

CTabDlg::~CTabDlg()
{
}

bool CTabDlg::AddView(CUpdateDialog *pDialog,
						DWORD _dwDialogID,
						const char *_szTabName)
{
	if(_dwDialogID!=0)
		pDialog->Create(_dwDialogID,(CWnd *)this);

	RECT rect;
	GetClientRect(&rect);
	rect.left+=3;
	rect.right-=5;
	rect.top+=25;
	rect.bottom-=5;

	pDialog->SetWindowPos(0,rect.left,rect.top,
		rect.right-rect.left,
		rect.bottom-rect.top,0);
	pDialog->ShowWindow(SW_HIDE);

	m_Views.push_back(pDialog);
	InsertItem(GetItemCount(),_szTabName);

	return true;
}

void CTabDlg::Clear()
{
	std::vector<CUpdateDialog *>::iterator i;
	for(i=m_Views.begin();i!=m_Views.end();i++)
	{
		(*i)->DestroyWindow();
		delete *i;
	}

	m_Views.clear();
	DeleteAllItems();
}

bool CTabDlg::RemoveView(const int iIndex,
						   const bool _bDestroyDialog)
{
	if(iIndex < 0 || iIndex >= m_Views.size())
		return false;

	if(_bDestroyDialog)
	{
		m_Views[iIndex]->DestroyWindow();
		delete m_Views[iIndex];
	}

	m_Views.erase(m_Views.begin()+iIndex);
	DeleteItem(iIndex);

	return true;
}

bool CTabDlg::SetCurrentView(const int iIndex)
{
	if(iIndex < 0 || iIndex >= m_Views.size())
		return false;

	if(iIndex==m_iLast)
		return true;
	
	SetCurSel(iIndex);

	if(m_iLast!=-1)
		m_Views[m_iLast]->ShowWindow(SW_HIDE);

	m_Views[iIndex]->ShowWindow(SW_SHOW);
	m_iLast=iIndex;

	return true;
}

void CTabDlg::OnSize( UINT nType, int cx, int cy )
{
	CTabCtrl::OnSize(nType,cx,cy);
}

void CTabDlg::OnMove( int x, int y )
{
	RECT rect;
	GetClientRect(&rect);
	rect.left+=3;
	rect.right-=5;
	rect.top+=25;
	rect.bottom-=5;

	std::vector<CUpdateDialog *>::iterator i;
	for(i=m_Views.begin();i!=m_Views.end();i++)
		(*i)->SetWindowPos(0,rect.left,rect.top,
		rect.right-rect.left,
		rect.bottom-rect.top,SWP_NOZORDER);

	CTabCtrl::OnMove(x,y);
}

void CTabDlg::Update(int iType, void *pParam)
{
	std::vector<CUpdateDialog *>::iterator i;
	for(i=m_Views.begin();i!=m_Views.end();i++)
		(*i)->Update(iType,pParam);
}

