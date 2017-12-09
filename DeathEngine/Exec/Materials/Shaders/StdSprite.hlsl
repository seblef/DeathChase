
#include "ShadersDefs.h"

float4x4 mViewProj : MAT_VIEWPROJ;

struct VS_INPUT
{
	float4 vPos : POSITION;
	float4 cColor : COLOR0;
	float2 vUV : TEXCOORD0;

};

struct VS_OUTPUT
{
	float4 vPos : POSITION;
	float4 cColor : COLOR0;
	float2 vUV : TEXCOORD0;

};

#define PS_INPUT	VS_OUTPUT


texture tBase : TEXTURE0;
sampler base=sampler_state
{
	Texture=(tBase);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// ********************************************* 1.4 ***************************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
	vsout.cColor=vsin.cColor;
	vsout.vUV=vsin.vUV;

	return vsout;
}

float4 PSMain_14(PS_INPUT psin) : COLOR
{
	return psin.cColor * tex2D(base,psin.vUV);
}


// ********************************************** 2.0 ********************************************

VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
	vsout.cColor=vsin.cColor;
	vsout.vUV=vsin.vUV;

	return vsout;
}

float4 PSMain_20(PS_INPUT psin) : COLOR
{
	return psin.cColor * tex2D(base,psin.vUV);
}


// *********************************************** 3.0 ***********************************************

VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
	vsout.cColor=vsin.cColor;
	vsout.vUV=vsin.vUV;

	return vsout;
}

float4 PSMain_30(PS_INPUT psin) : COLOR
{
	return psin.cColor * tex2D(base,psin.vUV);
}

