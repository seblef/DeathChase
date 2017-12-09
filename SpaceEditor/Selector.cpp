
#include "Selector.h"
#include "../DeathEngine/ColDet/coldet.h"
#include "../DeathEditor/Interfaces.h"
#include "../DeathEngine/Export/ICamera.h"
#include "../DeathEngine/Core/Plane.h"
#include "../DeathEngine/Export/IRenderer.h"
#include "../DeathEngine/Export/IShaderMgr.h"

Selector::Selector()
{
}

Selector::~Selector()
{
	g_ShaderMgr->ReleaseShader(m_hBBoxShader);
}

void Selector::RemoveObject(SObject *pObj)
{
	std::vector<SObject *>::iterator i;
	for(i=m_Objects.begin();i!=m_Objects.end();i++)
		if((*i)==pObj)
		{
			m_Objects.erase(i);
			return;
		}
}

int Selector::Pick(int x, int y, ICamera *pCam)
{
	ClearSelection();

	Core::Vector3 vOrigin,vDir;
	pCam->MakeRayFrom2D(x,y,vOrigin,vDir);

	float vCOrigin[3]={vOrigin.x,vOrigin.y,vOrigin.z};
	float vCDir[3]={vDir.x,vDir.y,vDir.z};
	float fTransform[16];

	std::vector<SObject *>::iterator i;
	for(i=m_Objects.begin();i!=m_Objects.end();i++)
	{
		if(Core::IntersectSphere(vOrigin,vDir,(*i)->GetBSphere().GetCenter(),
			(*i)->GetBSphere().GetRadius())!=-1.0f)
		{
			if((*i)->GetColData())
			{
				memcpy(fTransform,&(*i)->GetTransformation().GetMatrix(),16*sizeof(float));
				(*i)->GetColData()->setTransform(fTransform);

				if((*i)->GetColData()->rayCollision(vCOrigin,vCDir,true))
				{
					float vColPoint[3];

					(*i)->GetColData()->getCollisionPoint(vColPoint,false);

					SelectedItem item;
					item.fDistSq=(pCam->GetPosition() - (*i)->GetBSphere().GetCenter()).GetSquareLength();
					item.pObject=*i;
					m_Selection.push_back(item);
				}
			}
		}
	}

	SelectNearest();

	return m_Selection.size();
}

void Selector::SelectNearest()
{
	if(m_Selection.size()==0)
		m_Selected.pObject=0;
	else
	{
		std::vector<SelectedItem>::iterator nearest=m_Selection.end();
		std::vector<SelectedItem>::iterator i;

		float fmin_dist=1e23f;
		for(i=m_Selection.begin();i!=m_Selection.end();i++)
			if(nearest==m_Selection.end() || i->fDistSq < nearest->fDistSq)
			{
				nearest=i;
			}

		if(nearest==m_Selection.end())
			m_Selected.pObject=0;
		else
			m_Selected=*nearest;
	}
}

SObject *Selector::RotateSelection()
{
	if(m_Selection.size()==0) return 0;
	else if(m_Selection.size()==1) return m_Selected.pObject;
	else if(m_Selected.pObject==0)
	{
		SelectNearest();
		return m_Selected.pObject;
	}
	else
	{
		std::vector<SelectedItem>::iterator nearest=m_Selection.end();
		std::vector<SelectedItem>::iterator i;

		for(i=m_Selection.begin();i!=m_Selection.end();i++)
		{
			if(i->pObject!=m_Selected.pObject &&
			   (nearest==m_Selection.end() ||
			   (i->fDistSq >= m_Selected.fDistSq && i->fDistSq < nearest->fDistSq)))
			   nearest=i;
		}

		if(nearest!=m_Selection.end())
			m_Selected=*nearest;

		return m_Selected.pObject;
	}
}

void Selector::DrawSelected()
{
	if(m_Selected.pObject==0) return;

	if(m_hBBoxShader.IsNull())
		m_hBBoxShader=g_ShaderMgr->LoadShader("StdBBox");

	uint16 pIndices[]={0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7};
	const Core::Vector3 vCenter(m_Selected.pObject->GetBSphere().GetCenter());
	const float fSize(m_Selected.pObject->GetBSphere().GetRadius());

	m_pVertices[0]=Core::Vector3(vCenter.x - fSize, vCenter.y + fSize, vCenter.z - fSize);
	m_pVertices[1]=Core::Vector3(vCenter.x + fSize, vCenter.y + fSize, vCenter.z - fSize);
	m_pVertices[2]=Core::Vector3(vCenter.x + fSize, vCenter.y + fSize, vCenter.z + fSize);
	m_pVertices[3]=Core::Vector3(vCenter.x - fSize, vCenter.y + fSize, vCenter.z + fSize);
	m_pVertices[4]=Core::Vector3(vCenter.x - fSize, vCenter.y - fSize, vCenter.z - fSize);
	m_pVertices[5]=Core::Vector3(vCenter.x + fSize, vCenter.y - fSize, vCenter.z - fSize);
	m_pVertices[6]=Core::Vector3(vCenter.x + fSize, vCenter.y - fSize, vCenter.z + fSize);
	m_pVertices[7]=Core::Vector3(vCenter.x - fSize, vCenter.y - fSize, vCenter.z + fSize);
	
	g_ShaderMgr->SetShader(m_hBBoxShader);
	g_Renderer->SetTransformation(TRANS_MODEL,Core::Matrix4::Identity,0,false);
	g_Renderer->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_Renderer->RenderDynamicBuffers(D3DPT_LINELIST,8,m_pVertices,Core::VX_P,24,pIndices,D3DFMT_INDEX16);
}

void Selector::ForceSelection(SObject *pObj)
{
	if(!pObj) return;

	ClearSelection();
	SelectedItem item;
	item.fDistSq=0.0f;
	item.pObject=pObj;

	m_Selection.push_back(item);
	m_Selected=item;
}