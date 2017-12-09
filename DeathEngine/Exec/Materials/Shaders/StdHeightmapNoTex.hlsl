
#include "ShadersDefs.h"

float4x4 mViewProj : MAT_VIEWPROJ;
float fInvMaxHeight : register(c12);

struct VS_INPUT
{
	float4 vPos : POSITION;
	float4 cNorm : COLOR0;
	float2 vTex0 : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 vPos : POSITION;
	float4 cColor : COLOR0;
};

#define PS_INPUT		VS_OUTPUT


//	*********************** 1.4 ******************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
        vsout.cColor.a=1.0f;
	vsout.cColor.r=vsin.vPos.y * fInvMaxHeight;
	vsout.cColor.g=0.1f;
	vsout.cColor.b=0.1f;

	return vsout;
}

float4 PSMain_14(PS_INPUT psin) : COLOR
{
	return psin.cColor;
}

//	*********************** 2.0 ******************************


VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
        vsout.cColor.a=1.0f;
	vsout.cColor.r=vsin.vPos.y * fInvMaxHeight;
	vsout.cColor.g=0.1f;
	vsout.cColor.b=0.1f;

	return vsout;
}

float4 PSMain_20(PS_INPUT psin) : COLOR
{
	return psin.cColor;
}

//	*********************** 3.0 ******************************


VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
        vsout.cColor.a=1.0f;
	vsout.cColor.r=vsin.vPos.y * fInvMaxHeight;
	vsout.cColor.g=0.1f;
	vsout.cColor.b=0.1f;

	return vsout;
}

float4 PSMain_30(PS_INPUT psin) : COLOR
{
	return psin.cColor;
}

