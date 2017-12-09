
#ifndef _PATCHSIDE_H_

#define _PATCHSIDE_H_

#include "HeightmapTexCoords.h"
#include "Variance.h"
#include "TriNodeMgr.h"

namespace Game
{

class PatchSide
{
	public:

		PatchSide(Heightmap *pMap,
			HeightmapTexCoords *pTexCoords,
			int iLeft, int iTop, int iWidth);
		~PatchSide();

		void ComputeVariance() { m_Variance.ComputeVariance(); };
		void Tesselate(uint32 dwCurrentVariance);
		void Split(TriNode *pNode);
		void Reset();
		void Draw();

		TriNode *GetTriNode() { return &m_BaseNode; };

	protected:

		void RecursRender(TriNode *pTri,
						  int iLeftX, int iLeftY,
						  int iRightX, int iRightY,
						  int iApexX, int iApexY,
						  int iNode);
		void RecursTessellate(TriNode *pTri,
							  int iLeftX, int iLeftY,
							  int iRightX, int iRightY,
							  int iApexX, int iApexY,
							  int iNode, uint32 dwCurrentVariance);

		Heightmap *m_pMap;
		TriNodeMgr *m_pTriNodeMgr;
		TriNode m_BaseNode;
		Variance m_Variance;
		int m_iLeft,m_iTop,m_iWidth;
		HeightmapTexCoords *m_pTexCoords;

};

}

#endif