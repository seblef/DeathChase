
#include "ShadersDefs.h"

float4x4 mViewProj	: MAT_VIEWPROJ;
float4x4 mWorld		: MAT_WORLD;

struct VS_INPUT
{
	float4 vPos : POSITION;
	float2 vTex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 vPos : POSITION;
	float2 vTex : TEXCOORD0;
};

#define PS_INPUT		VS_OUTPUT

sampler2D tex0;

//	********************* 1.4 **************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWPos=mul(vsin.vPos,mWorld);
	vsout.vPos=mul(vWPos,mViewProj);
	vsout.vTex=vsin.vTex;

	return vsout;
}


float4 PSMain_14(PS_INPUT psin) : COLOR
{
	return tex2D(tex0,psin.vTex);
}

//	********************* 2.0 **************************


VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWPos=mul(vsin.vPos,mWorld);
	vsout.vPos=mul(vWPos,mViewProj);
	vsout.vTex=vsin.vTex;

	return vsout;
}


float4 PSMain_20(PS_INPUT psin) : COLOR
{
	return tex2D(tex0,psin.vTex);
}

//	********************* 3.0 **************************


VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWPos=mul(vsin.vPos,mWorld);
	vsout.vPos=mul(vWPos,mViewProj);
	vsout.vTex=vsin.vTex;

	return vsout;
}


float4 PSMain_30(PS_INPUT psin) : COLOR
{
	return tex2D(tex0,psin.vTex);
}

