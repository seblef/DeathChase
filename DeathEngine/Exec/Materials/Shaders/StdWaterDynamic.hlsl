
#include "ShadersDefs.h"

float4x4 mViewProj : MAT_VIEWPROJ;

float4 cAmbient : COLOR_AMBIENT;
float4 cSunColor : LIGHT_SUN_COLOR;
float4 vSunDir : LIGHT_SUN_DIRECTION;

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
	float4 vNorm : NORMAL;
	float4 cColor : COLOR0;
	float2 vTex0 : TEXCOORD0;
	float2 vTex1 : TEXCOORD1;
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

texture tDetail : TEXTURE1;
sampler detail=sampler_state
{
	Texture=(tDetail);
	MipFilter=LINEAR;
	MinFilter=LINEAR;
	MagFilter=LINEAR;
};

//	************************* 1.4 ***************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
	vsout.vNorm=vsin.vNorm;
	vsout.cColor=vsin.cColor * (cAmbient + saturate(cSunColor * dot(-vsout.vNorm,vSunDir)));
	vsout.vTex0=vsin.vTex0;
	vsout.vTex1=vsin.vTex1;

	return vsout;
}

float4 PSMain_14(PS_INPUT psin) : COLOR
{
	float4 vCol=tex2D(base,psin.vTex0) * tex2D(detail,psin.vTex1);
	vCol*=psin.cColor;
	return vCol;
}

//	************************* 2.0 ***************************


VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
	vsout.vNorm=vsin.vNorm;
	vsout.cColor=vsin.cColor * (cAmbient + saturate(cSunColor * dot(-vsout.vNorm,vSunDir)));
	vsout.vTex0=vsin.vTex0;
	vsout.vTex1=vsin.vTex1;

	return vsout;
}

float4 PSMain_20(PS_INPUT psin) : COLOR
{
	float4 vCol=tex2D(base,psin.vTex0) * tex2D(detail,psin.vTex1);
	vCol*=psin.cColor;
	return vCol;
}

//	************************* 3.0 ***************************


VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	vsout.vPos=mul(vsin.vPos,mViewProj);
	vsout.vNorm=vsin.vNorm;
	vsout.cColor=vsin.cColor * (cAmbient + saturate(cSunColor * dot(-vsout.vNorm,vSunDir)));
	vsout.vTex0=vsin.vTex0;
	vsout.vTex1=vsin.vTex1;

	return vsout;
}

float4 PSMain_30(PS_INPUT psin) : COLOR
{
	float4 vCol=tex2D(base,psin.vTex0) * tex2D(detail,psin.vTex1);
	vCol*=psin.cColor;
	return vCol;
}

