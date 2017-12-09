// Console.cpp : fichier d'implémentation
//


#include "stdafx.h"
#include "DeathEditor.h"
#include "Console.h"

#ifdef DEDEV

// Boîte de dialogue CConsole

IMPLEMENT_DYNAMIC(CConsole, CDialog)

CConsole::CConsole(CWnd* pParent /*=NULL*/)
	: CDialog(CConsole::IDD, pParent), m_bCreated(false)
{

}

CConsole::~CConsole()
{
}

void CConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONSOLELIST, m_List);
}


BEGIN_MESSAGE_MAP(CConsole, CDialog)
	ON_BN_CLICKED(IDC_CONSOLECLEAR, &CConsole::OnBnClickedConsoleclear)
END_MESSAGE_MAP()


// Gestionnaires de messages de CConsole

void CConsole::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CConsole::OnCancel()
{
	ShowWindow(SW_HIDE);
}

BOOL CConsole::OnInitDialog()
{
	if(CDialog::OnInitDialog())
	{
		m_bCreated=true;
		return TRUE;
	}
	else
		return FALSE;
}
void CConsole::OnBnClickedConsoleclear()
{
	if(m_bCreated)
		m_List.ResetContent();
}

void CConsole::AddLine(const char *szLine)
{
	if(m_bCreated)
	{
		m_List.AddString(szLine);
		m_List.SetCurSel(m_List.GetCount()-1);
	}
}

#endif