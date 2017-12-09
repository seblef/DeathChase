
#include "WindowMgr.h"
#include "resource.h"
#include "TreesDialog.h"
#include "NewMap.h"
#include "HeightmapDlg.h"
#include "TextureDlg.h"
#include "EnvironmentDlg.h"
#include "WaterDlg.h"

WindowMgr::WindowMgr() : m_pDocument(0)
{
	memset(m_pDialogs,0,sizeof(CDialog*)*WT_COUNT);
}

void WindowMgr::ShowDialog(StdWindowType _Type, bool bReinit)
{
	if(_Type < 0 || _Type >= WT_COUNT) return;
	if(m_pDialogs[_Type])
	{
		if(bReinit)
		{
			switch(_Type)
			{
				case WT_NEWMAP: ((NewMap *)m_pDialogs[_Type])->Reinit(); break;
				case WT_HEIGHTMAP: ((CHeightmapDlg *)m_pDialogs[_Type])->Reinit(); break;
				case WT_TEXTURE: ((CTextureDlg *)m_pDialogs[_Type])->Reinit(); break;
				case WT_ENVIRONMENT: ((CEnvironmentDlg *)m_pDialogs[_Type])->Reinit(); break;
				case WT_WATER: ((CWaterDlg *)m_pDialogs[_Type])->Reinit(); break;
				case WT_TREES: ((CTreesDialog *)m_pDialogs[_Type])->Reinit(); break;
				default: break;
			}
		}

		m_pDialogs[_Type]->ShowWindow(SW_SHOW);
	}
	else
	{

		switch(_Type)
		{
			case WT_NEWMAP:
				m_pDialogs[WT_NEWMAP]=new NewMap;
				m_pDialogs[WT_NEWMAP]->Create(IDD_NEWMAP);
				m_pDialogs[WT_NEWMAP]->ShowWindow(SW_SHOW);
				break;

			case WT_HEIGHTMAP:
				m_pDialogs[WT_HEIGHTMAP]=new CHeightmapDlg;
				m_pDialogs[WT_HEIGHTMAP]->Create(IDD_HEIGHTMAPDLG);
				m_pDialogs[WT_HEIGHTMAP]->ShowWindow(SW_SHOW);
				break;

			case WT_TEXTURE:
				m_pDialogs[WT_TEXTURE]=new CTextureDlg;
				m_pDialogs[WT_TEXTURE]->Create(IDD_TEXTUREDLG);
				m_pDialogs[WT_TEXTURE]->ShowWindow(SW_SHOW);
				break;

			case WT_ENVIRONMENT:
				m_pDialogs[WT_ENVIRONMENT]=new CEnvironmentDlg;
				m_pDialogs[WT_ENVIRONMENT]->Create(IDD_ENVIRONMENTDLG);
				m_pDialogs[WT_ENVIRONMENT]->ShowWindow(SW_SHOW);
				break;

			case WT_WATER:
				m_pDialogs[WT_WATER]=new CWaterDlg;
				m_pDialogs[WT_WATER]->Create(IDD_WATERDLG);
				m_pDialogs[WT_WATER]->ShowWindow(SW_SHOW);
				break;

			case WT_TREES:
				m_pDialogs[WT_TREES]=new CTreesDialog;
				m_pDialogs[WT_TREES]->Create(IDD_TREES);
				m_pDialogs[WT_TREES]->ShowWindow(SW_SHOW);
				break;

			default:
				break;
		}
	}
}

void WindowMgr::HideDialog(StdWindowType _Type)
{
	if(_Type < 0 || _Type >= WT_COUNT) return;

	if(m_pDialogs[_Type])
		m_pDialogs[_Type]->ShowWindow(SW_HIDE);
}

void WindowMgr::CloseAllDialogs()
{
	for(int i=0;i<WT_COUNT;i++)
		if(m_pDialogs[i])
			m_pDialogs[i]->ShowWindow(SW_HIDE);
}

void WindowMgr::DeleteAllDialogs()
{
	for(int i=0;i<WT_COUNT;i++)
		if(m_pDialogs[i])
		{
			delete m_pDialogs[i];
			m_pDialogs[i]=0;
		}
}

CDialog *WindowMgr::GetDialog(StdWindowType _Type)
{
	if(_Type >= 0 && _Type < WT_COUNT)
		return m_pDialogs[_Type];
	else
		return 0;
}

void WindowMgr::ReinitAllDialogs()
{
	if(m_pDialogs[WT_NEWMAP]) ((NewMap *)m_pDialogs[WT_NEWMAP])->Reinit();
	if(m_pDialogs[WT_HEIGHTMAP]) ((CHeightmapDlg *)m_pDialogs[WT_HEIGHTMAP])->Reinit();
	if(m_pDialogs[WT_TEXTURE]) ((CTextureDlg *)m_pDialogs[WT_TEXTURE])->Reinit();
	if(m_pDialogs[WT_ENVIRONMENT]) ((CEnvironmentDlg *)m_pDialogs[WT_ENVIRONMENT])->Reinit();
	if(m_pDialogs[WT_WATER]) ((CWaterDlg *)m_pDialogs[WT_WATER])->Reinit();
	if(m_pDialogs[WT_TREES]) ((CTreesDialog *)m_pDialogs[WT_TREES])->Reinit();
}