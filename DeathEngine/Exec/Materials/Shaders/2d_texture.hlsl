
struct VS_INPUT
{
	float4 vPosition	: POSITION;
	float4 vColor		: COLOR0;
	float2 vTexCoords	: TEXCOORD0;
};

#define VS_OUTPUT	VS_INPUT
#define PS_INPUT	VS_INPUT


sampler2D tex0;

//	************************* 1.4 ***************************

VS_OUTPUT VSMain_14(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	OUT.vPosition=IN.vPosition;
	OUT.vColor=IN.vColor;
	OUT.vTexCoords=IN.vTexCoords;

	return OUT;
}

float4 PSMain_14(PS_INPUT IN) : COLOR
{
	float4 vCol=tex2D(tex0,IN.vTexCoords);
	return vCol*IN.vColor;
}

//	************************* 2.0 ***************************

VS_OUTPUT VSMain_20(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	OUT.vPosition=IN.vPosition;
	OUT.vColor=IN.vColor;
	OUT.vTexCoords=IN.vTexCoords;

	return OUT;
}

float4 PSMain_20(PS_INPUT IN) : COLOR
{
	float4 vCol=tex2D(tex0,IN.vTexCoords);
	return vCol*IN.vColor;
}

//	************************* 3.0 ***************************


VS_OUTPUT VSMain_30(VS_INPUT IN)
{
	VS_OUTPUT OUT;

	OUT.vPosition=IN.vPosition;
	OUT.vColor=IN.vColor;
	OUT.vTexCoords=IN.vTexCoords;

	return OUT;
}

float4 PSMain_30(PS_INPUT IN) : COLOR
{
	float4 vCol=tex2D(tex0,IN.vTexCoords);
	return vCol*IN.vColor;
}
