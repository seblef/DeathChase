
#ifndef _VARIANCE_H_

#define _VARIANCE_H_

#include "Heightmap.h"
#include "../Common.h"

namespace Game
{

class Variance  
{
	public:

		Variance(Heightmap *pMap, int iLeft, int iTop, int iWidth);
		~Variance();

		void ComputeVariance();
		int GetVDepth() const { return m_iVDepth; };
		uint32 GetVariance(const int iPos) const { return m_Variance[iPos]; };
		uint32 GetMaxPos() const { return m_dwMaxPos; };

	protected:

		uint32 RecursComputeVariance(int iLeftX, int iLeftY, float fLeftZ,
									 int iRightX, int iRightY, float fRightZ,
									 int iApexX, int iApexY, float fApexZ,
									 int iNode);

		int m_iLeft,m_iTop,m_iWidth;
		int m_iVDepth;
		uint32 m_dwMaxPos;
		uint32 *m_Variance;
		Heightmap *m_pMap;

};

}

#endif