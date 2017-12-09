
#ifndef _TEXGENERATOR_H_
#define _TEXGENERATOR_H_

#include "TexGenLevel.h"
#include "../../DeathEngine/Common.h"
#include <vector>

class IHeightmap;

class TextureGenerator
{
	public:

		TextureGenerator(IHeightmap *pMap) : m_pMap(pMap) {};
		~TextureGenerator() {};

		bool Generate(Core::HTexture& hTex,
			const int iTextureSize,
			std::vector<TexGenLevel> *pLevels,
			int iSmoothCount=1);

	protected:

		class BaseTexture
		{
			public:
				BaseTexture() : iWidth(-1),iHeight(-1),pBitmap(0) {};
				~BaseTexture() { if(pBitmap) delete[] pBitmap; iWidth=-1; iHeight=-1; pBitmap=0; };

				int iWidth;
				int iHeight;
				byte *pBitmap;
		};

		void Smooth(const int iSize, byte *pBuffer);
		bool LoadBaseTexture(const char *szName, BaseTexture *pTex);
		void BuildHeightFactorsTable(float *pTable,
			std::vector<TexGenLevel> *pLevels);

		int m_iTextureSize;
		IHeightmap *m_pMap;

};


#endif