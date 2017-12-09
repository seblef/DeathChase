
#ifndef _PATCH_H_

#define _PATCH_H_

#include "PatchSide.h"
#include "../Core/BBox.h"

namespace Game
{

class Patch
{
	public:

		Patch(Heightmap *pMap, HeightmapTexCoords *pTexCoords,
			int _x, int _y, int iWidth);
		~Patch();

		void ComputeVariance();
		void Tesselate(uint32 dwCurrentVariance);
		void Reset();
		void Draw();

		TriNode *GetLeftTri() { return m_LeftSide.GetTriNode(); };
		TriNode *GetRightTri() { return m_RightSide.GetTriNode(); };
		Core::Vector3& GetMidPoint() { return m_vMidPoint; };

		bool &GetForceVariance() { return m_bForceVariance; };
		bool &GetRecalculate() { return m_bRecalculate; };
		bool &GetVisible() { return m_bVisible; };

		int GetX() { return x; };
		int GetY() { return y; };
		int GetWidth() { return m_iWidth; };

		Core::BBox& GetBBox() { return m_BBox; };

	protected:

		bool m_bForceVariance;
		bool m_bRecalculate;
		bool m_bVisible;
		int x,y,m_iWidth;

		PatchSide m_LeftSide,m_RightSide;
		Core::Vector3 m_vMidPoint;
		Core::BBox m_BBox;

};

}

#endif