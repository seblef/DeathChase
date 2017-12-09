
#include "Shader.h"
#include "../MyAssert.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <string>

namespace Core
{

int Shader::m_siShaderVersion=0;

const char *szVShaderProfile[]={"vs_1_1","vs_2_0","vs_3_0"};
const char *szPShaderProfile[]={"ps_1_4","ps_2_0","ps_3_0"};

const char *szShaderProfileExt[]={"_14","_20","_30"};

Shader::Shader(IDirect3DDevice9 *pDevice) :
	m_pDevice(pDevice),m_pVShader(0),m_pPShader(0),
	m_pVSConstantTable(0),m_pPSConstantTable(0)
{
	m_szName[0]='\0';
}

Shader::~Shader()
{
	Release();
}

bool Shader::Load(const char *szName)
{
	MYASSERT(szName,"Bad shader name",true);

	std::string strTemp("Materials\\Shaders\\");
	strTemp+=szName;
	strTemp+=".hlsl";

	if(!LoadVShader(strTemp.c_str()))
		return false;

	if(!LoadPShader(strTemp.c_str()))
	{
		m_pVShader->Release();
		m_pVShader=0;
		return false;
	}

	strcpy(m_szName,szName);

	return true;
}

bool Shader::Set()
{
	if(m_pVShader && m_pPShader)
	{
		ASSERTRETURN(m_pDevice->SetVertexShader(m_pVShader),D3D_OK,"SetVertexShader");
		ASSERTRETURN(m_pDevice->SetPixelShader(m_pPShader),D3D_OK,"SetPixelShader");
	}

	return true;
}

void Shader::Release()
{
	if(m_pVShader) m_pVShader->Release();
	if(m_pPShader) m_pPShader->Release();

	if(m_pVSConstantTable) m_pVSConstantTable->Release();
	if(m_pPSConstantTable) m_pPSConstantTable->Release();

	m_pVShader=0;
	m_pPShader=0;

	m_pVSConstantTable=0;
	m_pPSConstantTable=0;

	m_szName[0]='\0';
}

bool Shader::LoadVShader(const char *szName)
{
	g_Output->AddMsg("! Compilation du vertex shader %s\n",szName);

	char szVSName[32];
	sprintf(szVSName,"VSMain%s",szShaderProfileExt[m_siShaderVersion]);

	LPD3DXBUFFER pCode=0;
	LPD3DXBUFFER pErrors=0;

	if(D3DXCompileShaderFromFile(szName,0,0,szVSName,
		D3DXGetVertexShaderProfile(m_pDevice),0,&pCode,&pErrors,&m_pVSConstantTable)!=D3D_OK)
	{
		g_Output->AddMsg("x Erreur lors de la compilation du vertex shader %s:\n%s\n",
			szName,pErrors ? pErrors->GetBufferPointer() : "");

		if(pErrors) pErrors->Release();
		if(pCode) pCode->Release();
		return false;
	}

	if(pErrors) pErrors->Release();

	if(m_pDevice->CreateVertexShader((DWORD *)pCode->GetBufferPointer(),&m_pVShader)!=D3D_OK)
	{
		g_Output->AddMsg("x Erreur lors de la creation du vertex shader %s\n",szName);
		pCode->Release();
		return false;
	}

	pCode->Release();
	return true;
}

bool Shader::LoadPShader(const char *szName)
{
	g_Output->AddMsg("! Compilation du pixel shader %s\n",szName);

	char szPSName[32];
	sprintf(szPSName,"PSMain%s",szShaderProfileExt[m_siShaderVersion]);

	LPD3DXBUFFER pCode=0;
	LPD3DXBUFFER pErrors=0;

	if(D3DXCompileShaderFromFile(szName,0,0,szPSName,
		D3DXGetPixelShaderProfile(m_pDevice),0,&pCode,&pErrors,&m_pPSConstantTable)!=D3D_OK)
	{
		g_Output->AddMsg("x Erreur lors de la compilation du pixel shader %s:\n%s\n",
			szName,pErrors->GetBufferPointer());

		if(pErrors) pErrors->Release();
		if(pCode) pCode->Release();
		return false;
	}

	if(pErrors) pErrors->Release();

	if(m_pDevice->CreatePixelShader((DWORD *)pCode->GetBufferPointer(),&m_pPShader)!=D3D_OK)
	{
		g_Output->AddMsg("x Erreur lors de la creation du pixel shader %s\n",szName);
		pCode->Release();
		return false;
	}

	pCode->Release();
	return true;
}

bool Shader::BeginRestart()
{
	if(m_pVShader) m_pVShader->Release();
	if(m_pPShader) m_pPShader->Release();

	if(m_pVSConstantTable) m_pVSConstantTable->Release();
	if(m_pPSConstantTable) m_pPSConstantTable->Release();

	m_pVShader=0;
	m_pPShader=0;

	m_pVSConstantTable=0;
	m_pPSConstantTable=0;

	return true;
}

bool Shader::EndRestart(IDirect3DDevice9 *pDevice)
{
	m_pDevice=pDevice;
	std::string temp(m_szName);

	return Load(temp.c_str());
}

bool Shader::SetConstant(bool bVertexShader, const char *szName,
						 const float *pConstant, int iCount)
{
	ID3DXConstantTable *pTable=bVertexShader ? m_pVSConstantTable : m_pPSConstantTable;
	if(!pTable)
		return false;

	D3DXHANDLE h=pTable->GetConstantByName(NULL,szName);
	if(!h)
	{
		g_Output->AddMsg("x Constante %s indéfinie dans le shader %s\n",szName,m_szName);
		return false;
	}

	if(pTable->SetFloatArray(m_pDevice,h,pConstant,iCount)!=D3D_OK)
	{
		g_Output->AddMsg("x Erreur lors de la mise a jour de la constante %s du shader %s\n",
			szName,m_szName);
		return false;
	}

	return true;
}

}