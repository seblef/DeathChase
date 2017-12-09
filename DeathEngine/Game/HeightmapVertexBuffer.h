
#ifndef _HEIGHTMAPVERTEXBUFFER_H_

#define _HEIGHTMAPVERTEXBUFFER_H_

#include "../Core/TDynamicTab.h"
#include "../Core/Vertex.h"

namespace Game
{

class HeightmapVertexBuffer
{
	public:

		static HeightmapVertexBuffer* Instance();

		HeightmapVertexBuffer();
		~HeightmapVertexBuffer() {};

		void Add(Core::VertexPDT &v) { m_Tab.Add(v); };
		void Add(Core::VertexPDT *v) { m_Tab.AddTab(v,3); }
		int GetCount() { return m_Tab.GetItemCount(); };
		Core::VertexPDT *GetBuffer() { return (Core::VertexPDT *)m_Tab.GetBuffer(); };

		void Reset() { m_Tab.Clear(); };

	protected:

		static HeightmapVertexBuffer *m_pInstance;
		Core::TDynamicTab<Core::VertexPDT> m_Tab;

};

}

#endif