
#include "ShadersDefs.h"

float4x4 mViewProj : MAT_VIEWPROJ;
float4x4 mWorld : MAT_WORLD;

float4 cAmbient : COLOR_AMBIENT;
float4 cEmissive : COLOR_EMISSIVE;

struct VS_INPUT
{
	float4 vPos : POSITION;
	float4 vNorm : NORMAL;
	float4 cColor : COLOR0;
	float2 vTex0 : TEXCOORD0;
	float2 vTex1 : TEXCOORD1;
};

struct VS_OUTPUT
{
	float4 vPos : POSITION;
	float4 cColor : COLOR0;
	float2 vTex0 : TEXCOORD0;
};

#define PS_INPUT		VS_OUTPUT

texture tBase : TEXTURE0;
sampler base=sampler_state
{
	Texture=(tBase);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//	********************** 1.4 ************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);
	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.cColor=vsin.cColor * cAmbient + cEmissive;
	vsout.vTex0=vsin.vTex0;

	return vsout;
}

float4 PSMain_14(PS_INPUT psin) : COLOR
{
	return psin.cColor*tex2D(base,psin.vTex0);
}

//	********************** 2.0 ************************

VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);
	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.cColor=vsin.cColor * cAmbient + cEmissive;
	vsout.vTex0=vsin.vTex0;

	return vsout;
}

float4 PSMain_20(PS_INPUT psin) : COLOR
{
	return psin.cColor*tex2D(base,psin.vTex0);
}

//	********************** 3.0 ************************

VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);
	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.cColor=vsin.cColor * cAmbient + cEmissive;
	vsout.vTex0=vsin.vTex0;

	return vsout;
}

float4 PSMain_30(PS_INPUT psin) : COLOR
{
	return psin.cColor*tex2D(base,psin.vTex0);
}

