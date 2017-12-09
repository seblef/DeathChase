
#include "Patch.h"

namespace Game
{

Patch::Patch(Heightmap *pMap, HeightmapTexCoords *pTexCoords,
			 int _x, int _y, int iWidth) :
	x(_x),y(_y),m_iWidth(iWidth),
	m_LeftSide(pMap,pTexCoords,x,y,iWidth),
	m_RightSide(pMap,pTexCoords,x+iWidth,y+iWidth,-iWidth),
	m_bForceVariance(false),m_bRecalculate(false),m_bVisible(false),
	m_vMidPoint((float)(x+iWidth/2),0.0f,(float)(y+iWidth/2))
{
	int u,v;
	float max_height=0.0f;

	for(u=0;u<iWidth;u++)
		for(v=0;v<iWidth;v++)
			max_height=MAX(max_height,pMap->GetHeight(x+u,y+u));

	m_BBox.SetMin(Core::Vector3((float)x-1,0.0f,(float)y-1));
	m_BBox.SetMax(Core::Vector3((float)x+iWidth+1,max_height+2.0f,(float)y+iWidth+1));
}

Patch::~Patch() {}

void Patch::Reset()
{
	m_LeftSide.Reset();
	m_RightSide.Reset();

	m_LeftSide.GetTriNode()->BaseNeighbor=m_RightSide.GetTriNode();
	m_RightSide.GetTriNode()->BaseNeighbor=m_LeftSide.GetTriNode();
}

void Patch::ComputeVariance()
{
	m_LeftSide.ComputeVariance();
	m_RightSide.ComputeVariance();
}

void Patch::Tesselate(uint32 dwCurrentVariance)
{
	m_LeftSide.Tesselate(dwCurrentVariance);
	m_RightSide.Tesselate(dwCurrentVariance);
}

void Patch::Draw()
{
	m_LeftSide.Draw();
	m_RightSide.Draw();
}

}