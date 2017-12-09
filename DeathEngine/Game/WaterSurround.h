
#ifndef _WATERSURROUND_H_

#define _WATERSURROUND_H_

#include "../Core/Vertex.h"
#include "../Core/TDynamicTab.h"
#include "../Core/Handles.h"

struct IDirect3DIndexBuffer9;
struct IDirect3DVertexBuffer9;

namespace Game
{

class WaterSurround
{
	public:

		WaterSurround();
		~WaterSurround();

		void Generate(int iHeightmapWidth,
			int iWidth, int iHeight,
			int iStartX, int iStartY,
			int iWidthMult, int iHeightMult,
			float fHeight);
		void Clear();
		void Draw();

	protected:

		float m_fStartX, m_fStartY;
		int m_iWidth, m_iHeight;
		int m_iWidthMult,m_iHeightMult;
		int m_iHeightmapWidth;
		int m_iTriangles;
		float m_fHeight;

		IDirect3DVertexBuffer9 *m_pVBuf;
		IDirect3DIndexBuffer9 *m_pIBuf;

		int m_iVertCount;
		int m_iTriCount;

		void DrawPoint(Core::Vector3& vStart, Core::Vector3& vDiff,
			int iNumberX, int iMaxX,
			int iNumberY, int iMaxY,
			Core::TDynamicTab<Core::VertexPNDTT> *pVerts,
			Core::TDynamicTab<uint16> *pInd);
		void GenerateList();

		int m_iPointCount;
		Core::HShader m_hShader;

};

}

#endif