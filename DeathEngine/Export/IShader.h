
#ifndef _ISHADER_H_
#define _ISHADER_H_

class IShader
{
	public:

		IShader() {}
		virtual ~IShader() {}

		virtual bool SetConstant(bool bVertexShader, const char *szName,
			const float *pConstant,int iCount)=0;
};

#endif
