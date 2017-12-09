
#ifndef _HEIGHTMAPTEXCOORDS_H_

#define _HEIGHTMAPTEXCOORDS_H_

#include "Heightmap.h"
#include "../Core/Vector.h"
#include "../MyAssert.h"

namespace Game
{

class Heightmap;

class HeightmapTexCoords
{
	public:

		HeightmapTexCoords();
		~HeightmapTexCoords();

		bool Generate(Heightmap *pMap);

		inline float GetWidthEntry(int i) { MYASSERT(i>=0 && i<=m_pMap->GetSize(),"Bad width entry",true); return m_pWEntries[i]; };
		inline float GetHeightEntry(int i) { MYASSERT(i>=0 && i<=m_pMap->GetSize(),"Bad height entry",true); return m_pHEntries[i]; };

	protected:

		Heightmap *m_pMap;
		float *m_pWEntries;
		float *m_pHEntries;

};

}

#endif