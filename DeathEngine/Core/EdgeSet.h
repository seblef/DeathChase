
#ifndef _EDGESET_H_
#define _EDGESET_H_

#include "Vertex.h"
#include "TDynamicTab.h"
#include <stdio.h>

namespace Core
{

class EdgeSet
{
	public:

		class Edge
		{
			public:

				Edge() {}
				Edge(const Vector3& _v1, const Vector3& _v2,
					uint16 _i1, uint16 _i2,
					const Vector3& _vNorm) :
					v1(_v1), v2(_v2), id1(_i1), id2(_i2), vNormal(_vNorm) {}
				Edge(const Edge& edge) : v1(edge.v1), v2(edge.v2),
					id1(edge.id1), id2(edge.id2), vNormal(edge.vNormal) {}

				const Edge& operator=(const Edge& e)
				{
					v1=e.v1; v2=e.v2;
					id1=e.id1; id2=e.id2;
					vNormal=e.vNormal;
					return *this;
				}

				Vector3 v1;
				Vector3 v2;
				uint16 id1;
				uint16 id2;
				Vector3 vNormal;

		};

		EdgeSet() : m_dwEdgesCount(0), m_pEdges(0),
			m_iSilhouetteEdgesCount(0), m_pSilhouette(0) {}
		~EdgeSet() { Clear(); }

		void Clear();

		bool Build(int iVertCount, VertexPNDTT *pVert,
			int iTriCount, uint16 *pInd);

		void SaveToFile(FILE *pFile);
		void LoadFromFile(FILE *pFile);

		int GetCount() const { return m_dwEdgesCount; }
		const Edge *GetEdges() const { return m_pEdges; }

		bool BuildSilhouetteFromPointLight(const Vector3& vLightPos, int& iEdgesCount, Edge **ppEdges);
		bool BuildSilhouetteFromDirLight(const Vector3& vLightDir, int& iEdgesCount, Edge **ppEdges);

	protected:

		void AddEdge(Edge *p, int& iCount);

		uint32 m_dwEdgesCount;

		Edge *m_pEdges;
		Edge *m_pSilhouette;
		
		int m_iSilhouetteEdgesCount;
};

}

#endif