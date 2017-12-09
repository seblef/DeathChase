
#include "ShadersDefs.h"

float4x4 mViewProj : MAT_VIEWPROJ;
float4x4 mWorld :	MAT_WORLD;
float4 vCamPos : SHIELD_CAMPOS;
float4 vTime : COMMON_TIME;

float2 vTex0ScrollSpeed={0.5f,1.0f};
float2 vTex1ScrollSpeed={-1.0f,0.5f};

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
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//	************************* 1.4 **********************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);

	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.vTex0=vsin.vTex0 + vTex0ScrollSpeed * vTime.x;
	vsout.vTex1=vsin.vTex0 * 16.0f + vTex1ScrollSpeed * vTime.x;
	vsout.cColor=vsin.cColor;

//			Calcul de l'alpha en fct de la distance

	float3 vCPos=vCamPos.xyz;
	float fDist=length(vWorldPos.xyz - vCPos);

	vsout.cColor.a=saturate(0.7f - fDist * vCamPos.w);

	return vsout;
}


float4 PSMain_14(PS_INPUT psin) : COLOR
{
	float4 vCol=tex2D(base,psin.vTex0) * tex2D(detail,psin.vTex1);
	vCol*=psin.cColor;
	vCol.a=psin.cColor.a;
	return vCol;
}

//	************************* 2.0 **********************************


VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);

	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.vTex0=vsin.vTex0 + vTex0ScrollSpeed * vTime.x;
	vsout.vTex1=vsin.vTex0 * 16.0f + vTex1ScrollSpeed * vTime.x;
	vsout.cColor=vsin.cColor;

//			Calcul de l'alpha en fct de la distance

	float3 vCPos=vCamPos.xyz;
	float fDist=length(vWorldPos.xyz - vCPos);

	vsout.cColor.a=saturate(0.7f - fDist * vCamPos.w);

	return vsout;
}


float4 PSMain_20(PS_INPUT psin) : COLOR
{
	float4 vCol=tex2D(base,psin.vTex0) * tex2D(detail,psin.vTex1);
	vCol*=psin.cColor;
	vCol.a=psin.cColor.a;
	return vCol;
}

//	************************* 3.0 **********************************


VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);

	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.vTex0=vsin.vTex0 + vTex0ScrollSpeed * vTime.x;
	vsout.vTex1=vsin.vTex0 * 16.0f + vTex1ScrollSpeed * vTime.x;
	vsout.cColor=vsin.cColor;

//			Calcul de l'alpha en fct de la distance

	float3 vCPos=vCamPos.xyz;
	float fDist=length(vWorldPos.xyz - vCPos);

	vsout.cColor.a=saturate(0.7f - fDist * vCamPos.w);

	return vsout;
}


float4 PSMain_30(PS_INPUT psin) : COLOR
{
	float4 vCol=tex2D(base,psin.vTex0) * tex2D(detail,psin.vTex1);
	vCol*=psin.cColor;
	vCol.a=psin.cColor.a;
	return vCol;
}

