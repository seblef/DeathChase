#include "ShadersDefs.h"

float4x4 mViewProj	: MAT_VIEWPROJ;

struct VS_INPUT
{
	float4 vPos : POSITION;
	float4 cColor : COLOR0;
};

struct VS_OUTPUT
{
	float4 vPos : POSITION;
};

#define PS_INPUT		VS_OUTPUT

//	********************** 1.4 ****************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;
	vsout.vPos=mul(vsin.vPos,mViewProj);

	return vsout;
}

float4 PSMain_14(PS_INPUT psin) : COLOR
{
	return float4(1.0f,0.0f,1.0f,0.0f);
}

//	********************** 2.0 ****************************


VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;
	vsout.vPos=mul(vsin.vPos,mViewProj);

	return vsout;
}

float4 PSMain_20(PS_INPUT psin) : COLOR
{
	return float4(1.0f,0.0f,1.0f,0.0f);
}

//	********************** 3.0 ****************************


VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;
	vsout.vPos=mul(vsin.vPos,mViewProj);

	return vsout;
}

float4 PSMain_30(PS_INPUT psin) : COLOR
{
	return float4(1.0f,0.0f,1.0f,0.0f);
}

