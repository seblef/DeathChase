
#include "HeightmapVertexBuffer.h"

namespace Game
{

HeightmapVertexBuffer *HeightmapVertexBuffer::m_pInstance=0;
HeightmapVertexBuffer *HeightmapVertexBuffer::Instance()
{
	if(!m_pInstance)
		m_pInstance=new HeightmapVertexBuffer;

	return m_pInstance;
}

HeightmapVertexBuffer::HeightmapVertexBuffer()
{
	m_Tab.Initialize(4096);
}

}