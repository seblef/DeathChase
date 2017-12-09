
#ifndef _SHADOWVOLUME_H_
#define _SHADOWVOLUME_H_

#include "Vertex.h"

namespace Core
{

class EdgeSet;

class ShadowVolume
{
	public:

		ShadowVolume();
		~ShadowVolume();

		void Clear();

		bool GenerateFromDirLight(EdgeSet *pEdges, const Vector3& vLightDir,
								const Matrix4& mInvWorld);
		bool GenerateFromPointLight(EdgeSet *pEdges, const Vector3& vLightPos,
								const Matrix4& mInvWorld);

		VertexP *GetVertexBuffer() { return m_pVBuf; }
		uint16 *GetIndexBuffer() { return m_pIBuf; }

		int GetVerticesCount() const { return m_iVertCount; }
		int GetTriCount() const { return m_iTriCount; }

	protected:

		bool Create(EdgeSet *pEdges);

		uint16 *m_pIBuf;
		VertexP *m_pVBuf;

		int m_iVertCount;
		int m_iTriCount;

		int m_iEdgesCount;
};

}

#endif