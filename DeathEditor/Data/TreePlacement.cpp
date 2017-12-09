
#include "TreePlacement.h"
#include "../Interfaces.h"
#include "../../DeathEngine/Export/IMap.h"
#include "../../DeathEngine/Export/IMapObjectFactory.h"
#include "../../DeathEngine/Export/IObjectMap.h"
#include "../../DeathEngine/Export/IHeightmap.h"
#include "../../DeathEngine/Game/TreeDefs.h"
#include "../../DeathEngine/Game/IMapObject.h"
#include "../../DeathEngine/Export/IWater.h"

#define MAX_TEST_COUNT				2

void TreePlacement::Clear()
{
	m_Templates.clear();
}

void TreePlacement::ClearMap()
{
	g_Map->GetObjectMap()->ClearMap();
}

void TreePlacement::AddTreeTemplate(const char *szName, int iMin, int iMax, float fScale)
{
	m_Templates.push_back(TreeTemplate(std::string(szName),iMin,iMax,fScale));
}

bool TreePlacement::Generate(int iCount)
{
	ClearMap();

	if(m_Templates.size()==0 || iCount==0) return true;

	IObjectMap *pMap=g_Map->GetObjectMap();
	IMapObjectFactory *pFactory=g_Map->GetObjectFactory();
	const float fHeightFact=g_Map->GetHeightmap()->GetScale()==0.0f ? 1.0f : 255.0f / g_Map->GetHeightmap()->GetScale();
	const float fInvRandMax=1.0f / (float)RAND_MAX;

	std::vector<int> templates;

	int i=0;
	int total_test=0;
	while(i< iCount && total_test < (iCount*2))
	{
		//		1: on cherche une place libre
		int x,y;
		if(pMap->GetFreePlace(TREE_RADIUS,x,y))
		{
			//		2: on cherche les arbres correspondant à la hauteur
			const int iHeight=(int)(g_Map->GetHeightmap()->GetHeight(x,y) * fHeightFact);

			for(int j=0;j<m_Templates.size();j++)
			{
				if(iHeight >= m_Templates[j].iMin && iHeight <= m_Templates[j].iMax)
					templates.push_back(j);
			}

			//		3: on en selectionne un au hasard
			if(templates.size() > 0)
			{
				const int index=(rand() * templates.size()) / RAND_MAX;
				assert(index >= 0 && index < templates.size());

				//		4: on crée et place l'arbre
				IMapObject *pObj=(IMapObject *)pFactory->CreateObject(OBJ_MAP);

				const float fx=(float)x - TREE_REALRADIUS * 0.5f + TREE_REALRADIUS*(float)rand()*fInvRandMax;
				const float fy=(float)y - TREE_REALRADIUS * 0.5f + TREE_REALRADIUS*(float)rand()*fInvRandMax;
				Core::Vector3 vPos(fx,0.0f,fy);
				const float fScale=m_Templates[templates[index]].fScale;

				if(pObj && pObj->Generate(m_Templates[templates[index]].strName.c_str(),0))
				{
					pObj->GetTransformation().SetPosition(vPos);
					pObj->GetTransformation().SetRotation(Core::Quaternion());
					pObj->GetTransformation().SetScale(Core::Vector3(fScale,fScale,fScale));
					pObj->GetTransformation().RebuildMatrix();
					pObj->SetMapPos(Core::Vector2i(x,y));
					pMap->AddObject(pObj);
					i++;
				}
				else
					g_Output2->AddMsg("! Failed creating map object\n");

				templates.clear();
			}
		}

		total_test++;
	}

	g_Output2->AddMsg("! Tree generate: %i\n",i);

	return true;
}