
#include "Variance.h"
#include <memory.h>

namespace Game
{

Variance::Variance(Heightmap *pMap, int iLeft, int iTop, int iWidth) :
	m_pMap(pMap), m_iLeft(iLeft), m_iTop(iTop), m_iWidth(iWidth)
{
	m_iVDepth=6;
	m_dwMaxPos=1 << (m_iVDepth+1);
	m_Variance=new uint32[m_dwMaxPos];
	memset(m_Variance,0,m_dwMaxPos * sizeof(uint32));
}

Variance::~Variance()
{
	if(m_Variance)
		delete[] m_Variance;
}

uint32 Variance::RecursComputeVariance( int leftX,  int leftY,  float leftZ,
											   int rightX, int rightY, float rightZ,
											   int apexX,  int apexY,  float apexZ,
											   int node)
{
	//        /||
	//      /  |  |
	//    /    |    |
	//  /      |      |
	//  ~~~~~~~*~~~~~~~  <-- Compute the X and Y coordinates of '*'
	//
	int centerX=(leftX + rightX) >>1;		// Compute X coordinate of center of Hypotenuse
	int centerY=(leftY + rightY) >>1;		// Compute Y coord.

	// Get the height value at the middle of the Hypotenuse
	float centerZ=m_pMap->GetHeight(centerX, centerY);

	// Variance of this triangle is the actual height at it's hypotenuse midpoint minus the interpolated height.
	// Use values passed on the stack instead of re-accessing the Height Field.
	float part = centerZ - ((leftZ + rightZ) / 2.0f);
	unsigned int myVariance = (unsigned int) (part < 0.0f?part * -50.0f:part * 50.0f);

	if ( ((leftX - rightX) >= 2) || ((leftX - rightX) <= -2) ||
		 ((leftY - rightY) >= 2) || ((leftY - rightY) <= -2))
	{
		// Final Variance for this node is the max of it's own variance and that of it's children.
		myVariance=MAX( myVariance, RecursComputeVariance( apexX,   apexY,  apexZ, leftX, leftY, leftZ, centerX, centerY, centerZ,    node<<1 ) );
		myVariance=MAX( myVariance, RecursComputeVariance( rightX, rightY, rightZ, apexX, apexY, apexZ, centerX, centerY, centerZ, 1+(node<<1)) );
	}

	// Store the final variance for this node.  Note Variance is never zero.
	if ((unsigned int) node < m_dwMaxPos) 
		m_Variance[node] = 1 + myVariance;

	return myVariance;
}

void Variance::ComputeVariance()
{
	memset(m_Variance, 0, m_dwMaxPos * sizeof(uint32));
	RecursComputeVariance(m_iLeft + m_iWidth, m_iTop + m_iWidth,m_pMap->GetHeight(m_iLeft+m_iWidth,m_iTop+m_iWidth),
						  m_iLeft, m_iTop, m_pMap->GetHeight(m_iLeft,m_iTop),
						  m_iLeft + m_iWidth,m_iTop, m_pMap->GetHeight(m_iLeft + m_iWidth, m_iTop),
							1);	
}

}