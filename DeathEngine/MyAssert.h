
#ifndef _MYASSERT_H_

#define _MYASSERT_H_

#include "Core/TextOutput.h"
#include <assert.h>

//#define NDEBUG

#ifdef DEDEBUG
#define MYASSERT(exp,desc,bBreak)	\
	{ if(!(exp)) {\
		g_Output->AddMsg("> x Assertion failed:\n>    Expression: %s\n",#exp); \
		g_Output->AddMsg(">    Description: %s\n>    File: %s, Line: %i\n",(char *)desc,(char *)__FILE__,(int)__LINE__); \
		if(bBreak) assert(exp); } }


#define ASSERTRETURN(exp,ret,desc) \
	{ if((exp)!=(ret)) {\
	g_Output->AddMsg("> o Bad return value: %s, %s\n>    Description: %s\n>    File: %s, Line: %i\n",#exp,#ret,(char *)desc,(char *)__FILE__,(int)__LINE__); }}
#else
#define MYASSERT(exp,desc,bBreak)
#define ASSERTRETURN(exp,ret,desc) exp
#endif


#endif