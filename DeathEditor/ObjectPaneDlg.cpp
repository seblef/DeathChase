// ObjectPaneDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "DeathEditor.h"
#include "ObjectPaneDlg.h"
#include "Data/Selector.h"

#include "MapObjectPropDlg.h"
#include "MeshObjPropDlg.h"

#include "Interfaces.h"
#include "../DeathEngine/Export/IMap.h"
#include "../DeathEngine/Export/IHeightmap.h"
#include "../DeathEngine/Export/IObjectMap.h"
#include "../DeathEngine/Export/IMapObject.h"
#include "../DeathEngine/Export/IMapObjectFactory.h"

#define MAP_IMG				2
#define MESH_IMG			0
#define PATCH_IMG			1
#define PARTICLE_IMG		3
#define WEAPON_IMG			4
#define AMMO_IMG			5
#define MAPOBJECT_IMG		6
#define WATER_IMG			7
#define SUN_IMG				8


// Boîte de dialogue CObjectPaneDlg

IMPLEMENT_DYNAMIC(CObjectPaneDlg, CDialog)

CObjectPaneDlg::CObjectPaneDlg(CDeathEditorDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CUpdateDialog(pDoc,CObjectPaneDlg::IDD, pParent)
{

}

CObjectPaneDlg::~CObjectPaneDlg()
{
}

void CObjectPaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBJ_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(CObjectPaneDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_OBJ_TREE, &CObjectPaneDlg::OnNMClickObjTree)
	ON_NOTIFY(NM_DBLCLK, IDC_OBJ_TREE, &CObjectPaneDlg::OnNMDblclkObjTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_OBJ_TREE, &CObjectPaneDlg::OnTvnSelchangedObjTree)
	ON_NOTIFY(NM_RCLICK, IDC_OBJ_TREE, &CObjectPaneDlg::OnNMRclickObjTree)
	ON_NOTIFY(NM_RETURN, IDC_OBJ_TREE, &CObjectPaneDlg::OnNMReturnObjTree)
	ON_COMMAND(ID_OBJ_REMOVE, &CObjectPaneDlg::OnObjRemove)
	ON_COMMAND(ID_OBJ_PROP, &CObjectPaneDlg::OnObjProp)
END_MESSAGE_MAP()


// Gestionnaires de messages de CObjectPaneDlg

BOOL CObjectPaneDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	m_ImageList.Create(IDB_TREELIST,16,1,RGB(255,255,255));
	m_Tree.SetImageList(&m_ImageList,TVSIL_NORMAL);

	m_hMap=m_Tree.InsertItem("Map",MAP_IMG,MAP_IMG);
	m_hMeshes=m_Tree.InsertItem("Objets",MESH_IMG,MESH_IMG);
	m_hMapObjects=m_Tree.InsertItem("Objets de terrain",MAPOBJECT_IMG,MAPOBJECT_IMG);

	m_Tree.Expand(m_hMap,TVE_EXPAND);
	m_Tree.Expand(m_hMeshes,TVE_EXPAND);
	m_Tree.Expand(m_hMapObjects,TVE_EXPAND);

	return TRUE;
}
void CObjectPaneDlg::OnNMClickObjTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	*pResult = 0;
}

void CObjectPaneDlg::OnNMDblclkObjTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	HTREEITEM hItem=m_Tree.GetSelectedItem();
	std::vector<TreeItem>::iterator i;
	Game::IObject *pObj=0;

	for(i=m_Items.begin();i!=m_Items.end();i++)
		if(i->hTree==hItem)
		{
			pObj=i->pObject;
			break;
		}

	if(!pObj) return;

	ObjectType type=pObj->GetObjectType();

	if(type==OBJ_MAP || type==OBJ_MESH)
		g_Selector.ForceSelection(pObj);

	m_pDoc->UpdateAllViews(0);
}

void CObjectPaneDlg::OnTvnSelchangedObjTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	*pResult = 0;
}

void CObjectPaneDlg::OnNMRclickObjTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem=m_Tree.GetSelectedItem();
	if(!hItem || hItem==m_hMap || hItem==m_hMeshes || hItem==m_hMapObjects) return;

	CMenu menu;
	if(!menu.LoadMenu(IDR_OBJCONTEXT)) return;

	CMenu *pContext=menu.GetSubMenu(0);
	if(!pContext) return;

	RECT rect;
	m_Tree.GetItemRect(hItem,&rect,true);
	m_Tree.ClientToScreen(&rect);

	pContext->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
								(rect.left+rect.right)/2,(rect.top + rect.bottom)/2, this);

	*pResult = 0;
}

void CObjectPaneDlg::OnNMReturnObjTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	*pResult = 0;
}

void CObjectPaneDlg::Update(int iType, void *pParam)
{
	if(iType==UPDATE_RESET || iType==UPDATE_NEWMAP)
	{
		m_Tree.DeleteAllItems();
		m_Items.clear();

		m_hMap=m_Tree.InsertItem("Map",MAP_IMG,MAP_IMG);
		m_hMeshes=m_Tree.InsertItem("Objets",MESH_IMG,MESH_IMG);
		m_hMapObjects=m_Tree.InsertItem("Objets de terrain",MAPOBJECT_IMG,MAPOBJECT_IMG);

		m_Tree.Expand(m_hMap,TVE_EXPAND);
		m_Tree.Expand(m_hMeshes,TVE_EXPAND);
		m_Tree.Expand(m_hMapObjects,TVE_EXPAND);
	}
	else if(iType==UPDATE_MAPCREATED || iType==UPDATE_OPENMAP)
	{
		std::vector<Game::IObject*> *obj=g_Map->CollectObjects();
		std::vector<Game::IObject*>::iterator i;

		for(i=obj->begin();i!=obj->end();i++)
		{
			TreeItem item;
			item.pObject=*i;
			int iImg=GetImageNumber(item.pObject);
			item.hTree=m_Tree.InsertItem(item.pObject->GetName(),iImg,iImg,GetTreeParent(item.pObject));
			m_Items.push_back(item);
		}

		m_Tree.Expand(m_hMap,TVE_EXPAND);
		m_Tree.Expand(m_hMeshes,TVE_EXPAND);
		m_Tree.Expand(m_hMapObjects,TVE_EXPAND);
	}
	else if(iType==UPDATE_NEWOBJECT)
	{
		Game::IObject *pObj=(Game::IObject *)pParam;
		if(pObj)
		{
			TreeItem item;
			item.pObject=pObj;
			int iImg=GetImageNumber(pObj);
			HTREEITEM hParent=GetTreeParent(pObj);
			item.hTree=m_Tree.InsertItem(pObj->GetName(),iImg,iImg,hParent);
			m_Items.push_back(item);
			m_Tree.Expand(hParent,TVE_EXPAND);
		}
	}
	else if(iType==UPDATE_MODIFYOBJECT)
	{
		Game::IObject *pObj=(Game::IObject *)pParam;

		std::vector<TreeItem>::iterator i;
		for(i=m_Items.begin();i!=m_Items.end();i++)
			if(i->pObject==pObj)
			{
				m_Tree.SetItemText(i->hTree,pObj->GetName());
				break;
			}
	}
	else if(iType==UPDATE_REMOVEOBJECT)
	{
		Game::IObject *pObj=(Game::IObject *)pParam;

		std::vector<TreeItem>::iterator i;
		for(i=m_Items.begin();i!=m_Items.end();i++)
			if(i->pObject==pObj)
			{
				m_Tree.DeleteItem(i->hTree);
				m_Items.erase(i);
				break;
			}
	}
}

int CObjectPaneDlg::GetImageNumber(Game::IObject *pObj)
{
	int iImg;
	switch(pObj->GetObjectType())
	{
		case OBJ_PATCHGRID:
			iImg=PATCH_IMG;
			break;

		case OBJ_WATER:
			iImg=WATER_IMG;
			break;

		case OBJ_EDITOR:
			iImg=SUN_IMG;
			break;

		case OBJ_MESH:
			iImg=MESH_IMG;
			break;

		case OBJ_MAP:
			iImg=MAPOBJECT_IMG;
			break;

		default:
			iImg=-1;
	}

	return iImg;
}

HTREEITEM CObjectPaneDlg::GetTreeParent(Game::IObject *pObj)
{
	HTREEITEM hParent;

	switch(pObj->GetObjectType())
	{
		case OBJ_MAP:
			hParent=m_hMapObjects;
			break;

		case OBJ_MESH:
			hParent=m_hMeshes;
			break;

		default:
			hParent=m_hMap;
	}

	return hParent;

}
void CObjectPaneDlg::OnObjRemove()
{
	HTREEITEM hItem=m_Tree.GetSelectedItem();
	std::vector<TreeItem>::iterator i;
	Game::IObject *pObj=0;

	for(i=m_Items.begin();i!=m_Items.end();i++)
		if(i->hTree==hItem)
		{
			pObj=i->pObject;
			break;
		}

	if(!pObj) return;

	ObjectType type=pObj->GetObjectType();

	if(type==OBJ_MAP || type==OBJ_MESH)
	{
		pObj->RemoveFromOctree();
		g_Selector.ClearSelection();

		m_pDoc->UpdateAllViews(0,UPDATE_REMOVEOBJECT,(CObject *)((void *)pObj));
		g_Map->GetObjectFactory()->ReleaseObject(pObj);
	}
}

void CObjectPaneDlg::OnObjProp()
{
	HTREEITEM hItem=m_Tree.GetSelectedItem();
	std::vector<TreeItem>::iterator i;

	Game::IObject *pObj=0;

	for(i=m_Items.begin();i!=m_Items.end();i++)
		if(i->hTree==hItem)
		{
			pObj=i->pObject;
			break;
		}

	if(!pObj) return;

	ObjectType type=pObj->GetObjectType();

	if(type==OBJ_MAP)
	{
		CMapObjectPropDlg dlg(pObj);
		dlg.DoModal();

		m_Tree.SetItemText(hItem,pObj->GetName());
	}
	else if(type==OBJ_MESH)
	{
		CMeshObjPropDlg dlg((IMeshObject *)pObj);
		dlg.DoModal();

		m_Tree.SetItemText(hItem,pObj->GetName());
	}
}
