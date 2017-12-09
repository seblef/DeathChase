
#ifndef _CONVERTCHAR_H_

#define _CONVERTCHAR_H_

#include <string.h>

inline wchar_t *ConvertChar(const char *szChar)
{
	static wchar_t wszTemp[1024];
	const size_t newsize=1024;
	const size_t length=strlen(szChar)+1;
	size_t temp;

	mbstowcs_s(&temp,wszTemp,length,szChar,_TRUNCATE);

	return wszTemp;
}

inline const char *FloatToChar(float f)
{
	static char szTemp[32];
	sprintf(szTemp,"%f",f);
	return szTemp;
}

inline const char *IntToChar(int i)
{
	static char szTemp[16];
	sprintf(szTemp,"%i",i);
	return szTemp;
}

#endif