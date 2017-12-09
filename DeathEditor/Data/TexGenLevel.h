
#ifndef _TEXGENLEVEL_H_
#define _TEXGENLEVEL_H_

#include <string>
#include "../../DeathEngine/Core/Handles.h"

class TexGenLevel
{
	public:

		TexGenLevel() : iMin(0),iMax(255),strTexture(""),strPath("") {}
		TexGenLevel(int _iMin, int _iMax, const char *szTex, const char *szPath) : iMin(_iMin),iMax(_iMax),strTexture(szTex), strPath(szPath) {}
		TexGenLevel(const TexGenLevel& l) : iMin(l.iMin), iMax(l.iMax), strTexture(l.strTexture), strPath(l.strPath), hTexture(l.hTexture) {}
		~TexGenLevel() {}

		const TexGenLevel& operator=(const TexGenLevel& l)
		{
			iMin=l.iMin;
			iMax=l.iMax;
			strTexture=l.strTexture;
			strPath=l.strPath;
			hTexture=l.hTexture;

			return *this;
		}

		int iMin,iMax;
		std::string strTexture,strPath;
		Core::HTexture hTexture;
};


#endif