
#include "TexGenerator.h"
#include "../Interfaces.h"
#include "../../DeathEngine/Export/IHeightmap.h"
#include "../../DeathEngine/Export/ITextureMgr.h"


bool TextureGenerator::Generate(Core::HTexture& hTex,
			const int iTextureSize,
			std::vector<TexGenLevel> *pLevels,
			int iSmoothCount)
{
	assert(pLevels);

	if(pLevels->size()==0)
		return false;

	BaseTexture *pTex=new BaseTexture[pLevels->size()];
	int i,j;
	for(i=0;i<pLevels->size();i++)
		if(!LoadBaseTexture(((*pLevels)[i].strPath + (*pLevels)[i].strTexture).c_str(),&pTex[i]))
		{
			delete[] pTex;
			return false;
		}

	float *fFactors=new float[256 * pLevels->size()];
	BuildHeightFactorsTable(fFactors,pLevels);

	const int level_count=pLevels->size();

	byte *pBuffer=new byte[iTextureSize * iTextureSize * 4];
	if(!pBuffer)
	{
		delete[] fFactors;
		delete[] pTex;
		return false;
	}

	const float txt_to_height=(float)(m_pMap->GetSize()+1) / (float)iTextureSize;
	float r,g,b;

	byte *pBuf=pBuffer;
	for(i=0;i<iTextureSize;i++)
	{
		const int xheightmap=(int)((float)i*txt_to_height);

		for(j=0;j<iTextureSize;j++)
		{
			int yheightmap=(int)((float)j*txt_to_height);
			int height=(int)m_pMap->GetHeight(xheightmap,yheightmap) +
				(rand()%20)-10;
			height=MIN(MAX(height,0),255);

			r=g=b=0.0f;
			int k;
			for(k=0;k<level_count;k++)
			{
				int indexuv=((xheightmap%pTex[k].iWidth) + (yheightmap%(pTex[k].iHeight))*pTex[k].iHeight)*3;
				r+=fFactors[height*level_count + k] * (float)pTex[k].pBitmap[indexuv];
				g+=fFactors[height*level_count + k] * (float)pTex[k].pBitmap[indexuv+1];
				b+=fFactors[height*level_count + k] * (float)pTex[k].pBitmap[indexuv+2];
			}

			r=MIN(r,255.0f);g=MIN(g,255.0f);b=MIN(b,255.0f);

			pBuffer[(i+j*iTextureSize)*4]=(byte)r;
			pBuffer[(i+j*iTextureSize)*4+1]=(byte)g;
			pBuffer[(i+j*iTextureSize)*4+2]=(byte)b;
		}
	}

	for(i=0;i<iSmoothCount;i++)
		Smooth(iTextureSize,pBuffer);

	g_TextureMgr->FillTexture(hTex,(void *)pBuffer,iTextureSize,iTextureSize,4);

	delete[] pTex;
	delete[] fFactors;

	if(!g_TextureMgr->CompressTexture(hTex))
		g_Output2->AddMsg("x Failed compressing terrain texture\n");

	return true;
}

void TextureGenerator::BuildHeightFactorsTable(float *pTable,
														std::vector<TexGenLevel> *pLevels)
{
	int level_count=pLevels->size();
	int i,j;
	for(i=0;i<256;i++)
	{
		int mix_level[2]={-1,-1};
		int stage=0;
		float *pFacts=pTable + i * level_count;

		for(j=0;j<level_count;j++)
		{
			if(i >= (*pLevels)[j].iMin && i < (*pLevels)[j].iMax && stage < 2)
			{
				mix_level[stage]=j;
				stage++;
			}

			pFacts[j]=0.0f;
		}

		if(stage==1)
			pFacts[mix_level[0]]=1.0f;
		else if(stage==2)
		{
			pFacts[mix_level[1]]=((float)(i - (*pLevels)[mix_level[1]].iMin)) /
				((float)((*pLevels)[mix_level[0]].iMax - (*pLevels)[mix_level[1]].iMin));
			pFacts[mix_level[0]]=1.0f - pFacts[mix_level[1]];
		}
	}
}

bool TextureGenerator::LoadBaseTexture(const char *szName,
							  BaseTexture *pTex)
{
	FILE *pFile=fopen(szName,"rb");
	if(!pFile)
	{
		g_Output2->AddMsg("x Impossible de charger la texture de base %s\n",szName);
		return false;
	}

	BITMAPFILEHEADER header;
	fread(&header,sizeof(header),1,pFile);

	if(header.bfType!=0x424d && header.bfType!=0x4d42)
	{
		fclose(pFile);
		return false;
	}

	BITMAPINFOHEADER info;
	fread(&info,sizeof(info),1,pFile);

	if(info.biCompression!=BI_RGB)
	{
		fclose(pFile);
		return false;
	}

	pTex->pBitmap=new byte[info.biWidth*info.biHeight*3];
	if(!pTex->pBitmap)
	{
		fclose(pFile);
		return false;
	}

	if(info.biBitCount==24)
		fread(pTex->pBitmap,info.biWidth*info.biHeight*3,1,pFile);
	else if(info.biBitCount==8)
	{
		byte *pPalette=new byte[256*3];
		if(!pPalette)
		{
			delete[] pTex->pBitmap;
			fclose(pFile);
			return false;
		}

		fread(pPalette,256*3,1,pFile);

		byte *pTemp=new byte[info.biWidth*info.biHeight];
		if(!pTemp)
		{
			delete[] pPalette;
			delete[] pTex->pBitmap;
			fclose(pFile);
			return false;
		}

		fread(pTemp,info.biWidth*info.biHeight,1,pFile);

		int i;
		for(i=0;i<info.biWidth*info.biHeight;i++)
		{
			pTex->pBitmap[i*3]=pPalette[pTemp[i]*3];
			pTex->pBitmap[i*3+1]=pPalette[pTemp[i]*3+1];
			pTex->pBitmap[i*3+2]=pPalette[pTemp[i]*3+2];
		}

		delete[] pTemp;
		delete[] pPalette;
	}

	pTex->iHeight=info.biHeight;
	pTex->iWidth=info.biWidth;

	return true;
}

void TextureGenerator::Smooth(const int iSize, byte *pBuffer)
{
	int i,j;
	for(j=0;j<iSize;j++)
		for(i=0;i<iSize;i++)
		{
			int totalr=0;
			int totalg=0;
			int totalb=0;
			int count=0;

			int u,v;
			for(v=MAX(0,j-1);v<=MIN(j,iSize-1);v++)
				for(u=MAX(0,i-1);u<=MIN(i,iSize-1);u++)
				{
					totalr+=pBuffer[(u+v*iSize)*4];
					totalg+=pBuffer[(u+v*iSize)*4+1];
					totalb+=pBuffer[(u+v*iSize)*4+2];
					count++;
				}

			pBuffer[(i+j*iSize)*4]=totalr / count;
			pBuffer[(i+j*iSize)*4+1]=totalg / count;
			pBuffer[(i+j*iSize)*4+2]=totalb / count;
		}
}
