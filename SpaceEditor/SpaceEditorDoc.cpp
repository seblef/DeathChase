// SpaceEditorDoc.cpp : implémentation de la classe CSpaceEditorDoc
//

#include "stdafx.h"
#include "SpaceEditor.h"

#include "SpaceEditorDoc.h"
#include "ShipPropDlg.h"

#include "Selector.h"
#include "../DeathEditor/Data/MeshLoader.h"
#include "../DeathEditor/FileSystem.h"
#include "../DeathEditor/Interfaces.h"
#include "../DeathEngine/Export/IMaterialMgr.h"
#include "../DeathEngine/Export/IMesh.h"
#include "../DeathEngine/Export/IMeshMgr.h"
#include "../DeathEngine/Export/ISpaceShip.h"
#include "../DeathEngine/Export/IFactory.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceEditorDoc

IMPLEMENT_DYNCREATE(CSpaceEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSpaceEditorDoc, CDocument)
	ON_COMMAND(ID_FICHIER_NOUVEAUVAISSEAU, &CSpaceEditorDoc::OnFichierNouveauvaisseau)
	ON_COMMAND(ID_FICHIER_OUVRIRVAISSEAU, &CSpaceEditorDoc::OnFichierOuvrirvaisseau)
	ON_COMMAND(ID_FICHIER_QUITTER, &CSpaceEditorDoc::OnFichierQuitter)
	ON_COMMAND(ID_FICHIER_SAUVEGARDER, &CSpaceEditorDoc::OnFichierSauvegarder)
	ON_COMMAND(ID_TEST_TESTER, &CSpaceEditorDoc::OnTestTester)
END_MESSAGE_MAP()


// construction ou destruction de CSpaceEditorDoc

CSpaceEditorDoc::CSpaceEditorDoc() : m_bCreated(false)
{
	// TODO : ajoutez ici le code d'une construction unique

}

CSpaceEditorDoc::~CSpaceEditorDoc()
{
}

BOOL CSpaceEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO : ajoutez ici le code de réinitialisation
	// (les documents SDI réutiliseront ce document)

	return TRUE;
}




// sérialisation de CSpaceEditorDoc

void CSpaceEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO : ajoutez ici le code de stockage
	}
	else
	{
		// TODO : ajoutez ici le code de chargement
	}
}


// diagnostics pour CSpaceEditorDoc

#ifdef _DEBUG
void CSpaceEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSpaceEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// commandes pour CSpaceEditorDoc

void CSpaceEditorDoc::OnFichierNouveauvaisseau()
{
	if(m_bCreated)
	{
		if(MessageBox(0,"Etes vous sur de vouloir abandonner le travail en cours?","Nouveau vaisseau", MB_YESNO | MB_ICONQUESTION)==IDNO)
			return;
	}

	m_bCreated=false;

	g_FileSystem.ChangeWorkingDirectory("Models\\");
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+=g_MeshLoader.GetExtPath().c_str();

	CFileDialog dialog(TRUE,"*.*",strPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,g_MeshLoader.GetExtensions().c_str());
	if(dialog.DoModal()==IDCANCEL)
	{
		g_FileSystem.ResetWorkingDirectory();
		return;
	}

	strPath=g_FileSystem.GetWorkingDirectory();
	strPath+="\\";
	CString strFileName(dialog.GetPathName());
	
	ILoader *pLoader=g_MeshLoader.LoadMesh(dialog.GetPathName(),0,dialog.GetFileExt());

	g_FileSystem.ResetWorkingDirectory();
	if(!pLoader)
	{
		MessageBox(0,"Erreur lors du chargement du mesh.","Erreur",MB_OK | MB_ICONERROR);
		return;
	}
	else if(pLoader->GetMeshCount()==0)
	{
		MessageBox(0,"Le fichier ne contient aucun objet!","Erreur",MB_OK | MB_ICONERROR);
		pLoader->Release();
		return;
	}

	BuildMeshes(pLoader);
	pLoader->Release();

	CShipPropDlg::Show();
	CShipPropDlg::Instance()->m_strName="Ship";
	CShipPropDlg::Instance()->m_iRSpeed=90;
	CShipPropDlg::Instance()->m_iTSpeed=20;
	CShipPropDlg::Instance()->UpdateData(FALSE);

	UpdateAllViews(0,UPDATE_SHIPCREATED);

	m_bCreated=true;
}

void CSpaceEditorDoc::OnFichierOuvrirvaisseau()
{
	if(m_Objects.size() > 0)
	{
		if(MessageBox(0,"Etes vous sur de vouloir abandonner le travail en cours?","Nouveau vaisseau",MB_YESNO | MB_ICONQUESTION)==IDNO)
			return;

		std::vector<SObject *>::iterator i;
		for(i=m_Objects.begin();i!=m_Objects.end();i++)
			delete *i;

		m_Objects.clear();
	}

	g_FileSystem.ChangeWorkingDirectory(DIR_SPACESHIPS);
	CString strPath2(g_FileSystem.GetWorkingDirectory());
	strPath2+="\\*.dship";
	CFileDialog dialog(TRUE,"*.dship",strPath2,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"DeathChase spaceship (*.dship)|*.dship||");
	g_FileSystem.ResetWorkingDirectory();
	if(dialog.DoModal()==IDCANCEL)
		return;

	UpdateAllViews(0,UPDATE_RESET);

	CString strFullName(dialog.GetPathName());
	CString strFileName(dialog.GetFileName());
	CString strPath(g_FileSystem.GetWorkingDirectory());
	strPath+="\\";

	g_FileSystem.ResetWorkingDirectory();

	ISpaceShip *pShip=(ISpaceShip *)g_Factory->CreateObject(EOBJ_SPACESHIP);
	if(!pShip || !pShip->Load(strFileName,strPath))
	{
		MessageBox(0,"Erreur lors du chargement du vaisseau.","Erreur",MB_OK | MB_ICONERROR);
		g_Factory->ReleaseObject(pShip);
		return;
	}

	SObject *pShipObj=new SObject(SOBJ_SPACESHIP);
	pShipObj->SetMesh(g_MeshMgr->LoadMesh(pShip->GetName(),""));
	pShipObj->SetName(pShip->GetName());

	CShipPropDlg::Show();
	CShipPropDlg::Instance()->m_strName=pShip->GetName();
	CShipPropDlg::Instance()->m_iRSpeed=(int)(pShip->GetRotationSpeed() * 180.0f / PI);
	CShipPropDlg::Instance()->m_iTSpeed=(int)(pShip->GetLinearSpeed());
	CShipPropDlg::Instance()->UpdateData(FALSE);

	m_Objects.push_back(pShipObj);

	Core::Vector3 vPoint;
	for(int i=0;i<pShip->GetMainFirePointCount();i++)
	{
		vPoint=pShip->GetMainFirePoint(i);
		SObject *pObj=new SObject(SOBJ_MAINFIRE);
		pObj->GetTransformation().SetPosition(vPoint);
		pObj->ConstructMesh();
		m_Objects.push_back(pObj);
	}

	for(int i=0;i<pShip->GetSecFirePointCount();i++)
	{
		vPoint=pShip->GetSecFirePoint(i);
		SObject *pObj=new SObject(SOBJ_SECFIRE);
		pObj->GetTransformation().SetPosition(vPoint);
		pObj->ConstructMesh();
		m_Objects.push_back(pObj);
	}

	for(int i=0;i<pShip->GetBackFirePointCount();i++)
	{
		vPoint=pShip->GetBackFirePoint(i);
		SObject *pObj=new SObject(SOBJ_BACKFIRE);
		pObj->GetTransformation().SetPosition(vPoint);
		pObj->ConstructMesh();
		m_Objects.push_back(pObj);
	}

	for(int i=0;i<pShip->GetDefensePointCount();i++)
	{
		vPoint=pShip->GetDefensePoint(i);
		SObject *pObj=new SObject(SOBJ_DEFENSE);
		pObj->GetTransformation().SetPosition(vPoint);
		pObj->ConstructMesh();
		m_Objects.push_back(pObj);
	}

	for(int i=0;i<pShip->GetBombDropPointCount();i++)
	{
		vPoint=pShip->GetBombDropPoint(i);
		SObject *pObj=new SObject(SOBJ_BOMBDROP);
		pObj->GetTransformation().SetPosition(vPoint);
		pObj->ConstructMesh();
		m_Objects.push_back(pObj);
	}

	g_Factory->ReleaseObject(pShip);

	m_bCreated=true;
	UpdateAllViews(0,UPDATE_SHIPCREATED);
}

void CSpaceEditorDoc::OnFichierQuitter()
{
	if(m_Objects.size() > 0)
	{
		if(MessageBox(0,"Etes vous sur de vouloir quitter le travail en cours?","Quitter",MB_YESNO | MB_ICONQUESTION)==IDNO)
			return;
	}

	PostQuitMessage(0);
}

void CSpaceEditorDoc::OnFichierSauvegarder()
{
	if(m_Objects.size()==0) return;

	Core::HMesh hShip;
	std::vector<SObject*>::iterator i;
	for(i=m_Objects.begin();i!=m_Objects.end();i++)
		if((*i)->GetType()==SOBJ_SPACESHIP)
		{
			hShip=(*i)->GetMesh();
			break;
		}

	if(hShip.IsNull()) return;

	ISpaceShip *pShip=(ISpaceShip *)g_Factory->CreateObject(EOBJ_SPACESHIP);
	if(!pShip) return;

	CShipPropDlg::Instance()->UpdateData(TRUE);

	pShip->SetMesh(hShip);
	pShip->SetName(CShipPropDlg::Instance()->m_strName);
	pShip->SetLinearSpeed((float)CShipPropDlg::Instance()->m_iTSpeed);
	pShip->SetRotationSpeed(((float)CShipPropDlg::Instance()->m_iRSpeed)*PI/180.0f);

	g_Output2->AddMsg("! Infos: %s, %f, %f\n",pShip->GetName(),pShip->GetLinearSpeed(),pShip->GetRotationSpeed());

	for(i=m_Objects.begin();i!=m_Objects.end();i++)
	{
		switch((*i)->GetType())
		{
			case SOBJ_MAINFIRE:
				pShip->AddMainFirePoint((*i)->GetTransformation().GetPosition());
				break;

			case SOBJ_SECFIRE:
				pShip->AddSecFirePoint((*i)->GetTransformation().GetPosition());
				break;

			case SOBJ_BACKFIRE:
				pShip->AddBackFirePoint((*i)->GetTransformation().GetPosition());
				break;

			case SOBJ_DEFENSE:
				pShip->AddDefensePoint((*i)->GetTransformation().GetPosition());
				break;

			case SOBJ_BOMBDROP:
				pShip->AddBombDropPoint((*i)->GetTransformation().GetPosition());
				break;
		}
	}

	CString strName(CShipPropDlg::Instance()->m_strName);
	if(strName.GetLength()==0)
		strName="Ship";
	
	pShip->Save(strName,0);
	g_Factory->ReleaseObject(pShip);
}

void CSpaceEditorDoc::BuildMeshes(ILoader *pLoader)
{
	for(int i=0;i<pLoader->GetMeshCount();i++)
	{
		Core::VertexPNDTT *pVert;
		int iVertCount=pLoader->GetVertices(i,&pVert);

		uint16 *pIndices;
		int iTriCount=pLoader->GetIndices(i,&pIndices);

		int iSubMeshesCount=pLoader->GetSubMeshCount(i);

		int iSTriCount,iOffset,iMatID;
		pLoader->GetSubMesh(i,0,iOffset,iSTriCount,iMatID);

		LMaterial *pMat=pLoader->GetMaterial(iMatID==-1 ? 0 : iMatID);
		Core::HMaterial hMat;

		if(pMat)
		{
			hMat=g_MaterialMgr->GetMaterialByName(pMat->m_szName);
			if(hMat.IsNull())
			{
				hMat=g_MaterialMgr->CreateEmptyMaterial(pMat->m_szName);
				IMaterial *pNMat=g_MaterialMgr->GetMaterial(hMat);
				g_MeshLoader.ConvertMaterial(pNMat,pMat);
			}
		}
		else
			hMat=Core::HMaterial();

		Core::HMesh hMesh=g_MeshMgr->CreateEmptyMesh(pLoader->GetMeshName(i));
		IMesh *pMesh=g_MeshMgr->GetMesh(hMesh);
		ISubMesh *pSub=g_MeshMgr->CreateNewSubMesh();
		
		pSub->Construct(iOffset,iSTriCount,hMat);
		pMesh->Construct(pLoader->GetMeshName(i),iVertCount,pVert,iTriCount,pIndices,pSub,0);

		for(int j=1;j<iSubMeshesCount;j++)
		{
			pSub=g_MeshMgr->CreateNewSubMesh();
			pLoader->GetSubMesh(i,j,iOffset,iSTriCount,iMatID);

			pMat=pLoader->GetMaterial(iMatID==-1 ? 0 : iMatID);

			if(pMat)
			{
				hMat=g_MaterialMgr->GetMaterialByName(pMat->m_szName);
				if(hMat.IsNull())
				{
					hMat=g_MaterialMgr->CreateEmptyMaterial(pMat->m_szName);
					IMaterial *pNMat=g_MaterialMgr->GetMaterial(hMat);
					g_MeshLoader.ConvertMaterial(pNMat,pMat);
				}
			}
			else
				hMat=Core::HMaterial();

			pSub->Construct(iOffset,iSTriCount,hMat);
			pMesh->AddSubMesh(pSub);
		}

		SObject *pObject=new SObject(SOBJ_SPACESHIP);
		pObject->SetMesh(hMesh);
		pObject->SetName(pMesh->GetName());

		m_Objects.push_back(pObject);
		g_Selector.RegisterObject(pObject);
	}
}

void CSpaceEditorDoc::OnTestTester()
{
	// TODO : ajoutez ici le code de votre gestionnaire de commande
}
