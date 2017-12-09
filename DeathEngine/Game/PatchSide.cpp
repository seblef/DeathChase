
#include "PatchSide.h"
#include "HeightmapVertexBuffer.h"

namespace Game
{

PatchSide::PatchSide(Heightmap *pMap,
					 HeightmapTexCoords *pTexCoords,
					 int iLeft, int iTop, int iWidth) :
	m_pTriNodeMgr(TriNodeMgr::Instance()),
	m_pTexCoords(pTexCoords),
	m_pMap(pMap),m_iLeft(iLeft),m_iTop(iTop),m_iWidth(iWidth),
	m_Variance(pMap,iLeft,iTop,iWidth) {}

PatchSide::~PatchSide() {}

void PatchSide::Reset()
{
	m_BaseNode.LeftChild=0;
	m_BaseNode.RightChild=0;
	m_BaseNode.LeftNeighbor=0;
	m_BaseNode.RightNeighbor=0;
}

void PatchSide::Split(TriNode *tri)
{
	// We are already split, no need to do it again.
	if (tri->LeftChild)
		return;

	// If this triangle is not in a proper diamond, force split our base neighbor
	if(tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor!=tri))
		Split(tri->BaseNeighbor);

	// Create children and link into mesh
	TriNode *cl = m_pTriNodeMgr->GetNextTriNode();
	TriNode *cr = m_pTriNodeMgr->GetNextTriNode();

	// If creation failed, just exit.
	if ( !cl || !cr) return;

	tri->RightChild = cr;
	tri->LeftChild  = cl;

	// Fill in the information we can get from the parent (neighbor pointers)
	tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
	tri->LeftChild->LeftNeighbor  = tri->RightChild;

	tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
	tri->RightChild->RightNeighbor = tri->LeftChild;

	// Link our Left Neighbor to the new children
	if (tri->LeftNeighbor != 0)
	{
		if (tri->LeftNeighbor->BaseNeighbor == tri)
			tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->LeftNeighbor == tri)
			tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->RightNeighbor == tri)
			tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
		else
			;// Illegal Left Neighbor!
	}

	// Link our Right Neighbor to the new children
	if (tri->RightNeighbor != 0)
	{
		if (tri->RightNeighbor->BaseNeighbor == tri)
			tri->RightNeighbor->BaseNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->RightNeighbor == tri)
			tri->RightNeighbor->RightNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->LeftNeighbor == tri)
			tri->RightNeighbor->LeftNeighbor = tri->RightChild;
		else
			;// Illegal Right Neighbor!
	}

	// Link our Base Neighbor to the new children
	if (tri->BaseNeighbor != 0)
	{
		if ( tri->BaseNeighbor->LeftChild )
		{
			tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
			tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
			tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
			tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
		}
		else
			Split( tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
	}
	else
	{
		// An edge triangle, trivial case.
		tri->LeftChild->RightNeighbor = 0;
		tri->RightChild->LeftNeighbor = 0;
	}
}

void PatchSide::RecursTessellate( TriNode *tri,
							  int leftX,  int leftY,
							  int rightX, int rightY,
							  int apexX,  int apexY,
							  int node, uint32 currentVariance )
{
	int centerX = (leftX + rightX) >> 1; // Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY) >> 1; // Compute Y coord.

	uint32 triVariance = 100000;
	if((uint32) node < m_Variance.GetMaxPos() ) // If we are not below the variance tree
		triVariance = m_Variance.GetVariance(node);	// Take variance into consideration
	else
		triVariance = 1;

	if (triVariance >= currentVariance)
	{
		Split(tri);														// Split this triangle.

		if (tri->LeftChild &&											// If this triangle was split, try to split it's children as well.
			((abs(leftX - rightX) >= 2) || (abs(leftY - rightY) >= 2 )))	// Tessellate all the way down to one vertex per height field entry
		{
			RecursTessellate( tri->LeftChild,   apexX,  apexY, leftX, leftY, centerX, centerY,    node<<1 , currentVariance);
			RecursTessellate( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1), currentVariance);
		}
	}
}

void PatchSide::Tesselate(uint32 currentVariance)
{
	RecursTessellate(&m_BaseNode,
		m_iLeft+m_iWidth,m_iTop+m_iWidth,
		m_iLeft,m_iTop,
		m_iLeft+m_iWidth,m_iTop,1,currentVariance);
}

void PatchSide::RecursRender( TriNode *tri, 
							int leftX, int leftY, 
							int rightX, int rightY, 
							int apexX, int apexY, 
							int node)
{
	if(tri->LeftChild)
	{
		int centerX = (leftX + rightX) >> 1;
		int centerY = (leftY + rightY) >> 1;

		RecursRender( tri->LeftChild, apexX,   apexY, leftX, leftY, centerX, centerY, node<<1);
		RecursRender( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1+(node<<1));
	}
	else
	{
		Core::VertexPDT vert[3];

		vert[0].vPos.x=(float)leftX;
		vert[0].vPos.z=(float)leftY;
		m_pMap->GetNormalHeight(leftX,leftY,vert[0].vPos.y,vert[0].cColor.dwRGB);
//		vert[0].vPos.y=0.0f;
		vert[0].vUV.x=m_pTexCoords->GetWidthEntry(leftX);
		vert[0].vUV.y=m_pTexCoords->GetHeightEntry(leftY);

		vert[1].vPos.x=(float)rightX;
		vert[1].vPos.z=(float)rightY;
		m_pMap->GetNormalHeight(rightX,rightY,vert[1].vPos.y,vert[1].cColor.dwRGB);
//		vert[1].vPos.y=0.0f;
		vert[1].vUV.x=m_pTexCoords->GetWidthEntry(rightX);
		vert[1].vUV.y=m_pTexCoords->GetHeightEntry(rightY);

		vert[2].vPos.x=(float)apexX;
		vert[2].vPos.z=(float)apexY;
		m_pMap->GetNormalHeight(apexX,apexY,vert[2].vPos.y,vert[2].cColor.dwRGB);
//		vert[2].vPos.y=0.0f;
		vert[2].vUV.x=m_pTexCoords->GetWidthEntry(apexX);
		vert[2].vUV.y=m_pTexCoords->GetHeightEntry(apexY);

		HeightmapVertexBuffer::Instance()->Add(vert);
	}
}

void PatchSide::Draw()
{
	RecursRender(&m_BaseNode,
		m_iLeft+m_iWidth,m_iTop+m_iWidth,
		m_iLeft,m_iTop,
		m_iLeft+m_iWidth,m_iTop,1);
}

}
