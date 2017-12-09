
#include "HeightmapTexCoords.h"

namespace Game
{

HeightmapTexCoords::HeightmapTexCoords() :
	m_pMap(0),m_pWEntries(0),m_pHEntries(0) {}

HeightmapTexCoords::~HeightmapTexCoords()
{
	if(m_pWEntries)
		delete[] m_pWEntries;

	if(m_pHEntries)
		delete[] m_pHEntries;
}

bool HeightmapTexCoords::Generate(Heightmap *pMap)
{
	MYASSERT(pMap,"Bad heightmap pointer",true);

	if(m_pWEntries) delete[] m_pWEntries;
	if(m_pHEntries) delete[] m_pHEntries;

	m_pWEntries=m_pHEntries=0;

	m_pMap=pMap;
	m_pWEntries=new float[pMap->GetSize()+1];
	m_pHEntries=new float[pMap->GetSize()+1];

	const float fInvMapSize=1.0f / float(m_pMap->GetSize());

	int i;
	for(i=0;i<=m_pMap->GetSize();i++)
		m_pWEntries[i]=m_pHEntries[i]=(float)i * fInvMapSize;

	return true;
}

}