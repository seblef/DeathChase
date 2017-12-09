
#include "ShadersDefs.h"

float4x4 mViewProj : MAT_VIEWPROJ;
float4x4 mWorld :	MAT_WORLD;
float4 vCamPos : SHIELD_CAMPOS;

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
};

#define PS_INPUT		VS_OUTPUT

//	********************* 1.4 ***************************

VS_OUTPUT VSMain_14(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);

	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.cColor=vsin.cColor;

//			Calcul de l'alpha en fct de la distance

	float3 vCPos=vCamPos.xyz;
	float fDist=length(vWorldPos.xyz - vCPos);

	vsout.cColor.a=saturate(0.7f - fDist * vCamPos.w);

	return vsout;
}

float4 PSMain_14(PS_INPUT psin) : COLOR
{
	return psin.cColor;
}

//	********************* 2.0 ***************************


VS_OUTPUT VSMain_20(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);

	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.cColor=vsin.cColor;

//			Calcul de l'alpha en fct de la distance

	float3 vCPos=vCamPos.xyz;
	float fDist=length(vWorldPos.xyz - vCPos);

	vsout.cColor.a=saturate(0.7f - fDist * vCamPos.w);

	return vsout;
}

float4 PSMain_20(PS_INPUT psin) : COLOR
{
	return psin.cColor;
}

//	********************* 3.0 ***************************


VS_OUTPUT VSMain_30(VS_INPUT vsin)
{
	VS_OUTPUT vsout;

	float4 vWorldPos=mul(vsin.vPos,mWorld);

	vsout.vPos=mul(vWorldPos,mViewProj);
	vsout.cColor=vsin.cColor;

//			Calcul de l'alpha en fct de la distance

	float3 vCPos=vCamPos.xyz;
	float fDist=length(vWorldPos.xyz - vCPos);

	vsout.cColor.a=saturate(0.7f - fDist * vCamPos.w);

	return vsout;
}

float4 PSMain_30(PS_INPUT psin) : COLOR
{
	return psin.cColor;
}

