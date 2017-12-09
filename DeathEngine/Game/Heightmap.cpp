
#include "Heightmap.h"
#include "../MyAssert.h"
#include "../Common.h"
#include "../Core/Color.h"
#include "../Core/TextureMgr.h"
#include <windows.h>
#include <stdio.h>


namespace Game
{

Heightmap::~Heightmap()
{
	if(m_pMap) delete[] m_pMap;
	if(m_pNormals) delete[] m_pNormals;

	g_TextureMgr.ReleaseTexture(m_hTexture);
}

bool Heightmap::Load(const char *szName, const char *szPathName)
{
	assert(szName);

	char szFullName[1024];
	if(szPathName)
		sprintf(szFullName,"%s%s",szPathName,szName);
	else
		sprintf(szFullName,"Maps/%s",szName);

	int len=strlen(szFullName);
	if(len > 4)
	{
		if(strcmp(&szFullName[len-4],".bmp")==0)
		{
			FILE *pFile=fopen(szFullName,"rb");
			if(!pFile)
				return false;

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

			if(m_pMap)
				delete[] m_pMap;

			m_fScale=0.0f;

			m_pMap=new float[(info.biWidth+1) * (info.biWidth+1)];
			if(!m_pMap)
			{
				fclose(pFile);
				return false;
			}

			if(info.biBitCount==24)
			{
				byte *pBuffer=new byte[info.biWidth*info.biWidth*3];
				if(!pBuffer)
				{
					delete[] m_pMap; m_pMap=0;
					fclose(pFile);
					return false;
				}

				fread(pBuffer,info.biWidth*info.biWidth*3,1,pFile);

				int x,y;
				float fx,fy;
				float ax=((float)(info.biWidth)) / ((float)info.biWidth+1);
				byte *pBuf;
				float *pMap=m_pMap;
				for(y=0,fy=0.0f;y<info.biWidth+1;y++,fy+=ax)
				{
					pBuf=pBuffer + ((int)(fy * (float)(info.biWidth+1)))*3;
					for(x=0,fx=0.0f;x<info.biWidth+1;x++,fx+=ax)
					{
						*pMap=(float)pBuf[((int)fx)*3];
						pMap++;
					}
				}

				delete[] pBuffer;
			}
			else if(info.biBitCount==8)
			{
				byte *pBuffer=new byte[info.biWidth*info.biWidth];
				if(!pBuffer)
				{
					delete[] m_pMap; m_pMap=0;
					fclose(pFile);
					return false;
				}

				fread(pBuffer,256*3,1,pFile);
				fread(pBuffer,info.biWidth*info.biWidth,1,pFile);

				int x,y;
				float fx,fy;
				float ax=((float)(info.biWidth)) / ((float)info.biWidth+1);
				byte *pBuf;
				float *pMap=m_pMap;
				for(y=0,fy=0.0f;y<info.biWidth+1;y++,fy+=ax)
				{
					pBuf=pBuffer + (int)(fy * (float)(info.biWidth+1));
					for(x=0,fx=0.0f;x<info.biWidth+1;x++,fx+=ax)
					{
						*pMap=(float)pBuf[(int)fx];
						pMap++;
					}
				}
//				for(i=0;i<info.biWidth*info.biWidth;i++)
//					m_pMap[i]=pBuffer[i];

				delete[] pBuffer;
			}

			fclose(pFile);
			m_iWidth=info.biWidth;
			m_iWidthPlusOne=info.biWidth+1;
		}
		else if(strcmp(&szFullName[len-4],".hgt")==0)
		{
			FILE *pFile=fopen(szFullName,"rb");
			if(!pFile)
				return false;

			m_fScale=0.0f;

			int iSize;
			bool bScaled;
			float fScale;
			fread((void *)&iSize,sizeof(int),1,pFile);
			fread((void *)&bScaled,sizeof(bool),1,pFile);
			fread((void *)&fScale,sizeof(float),1,pFile);

			if(bScaled) m_fScale=fScale;

			if(iSize!=256 && iSize!=512 && iSize!=1024 && iSize!=2048)
			{
				fclose(pFile);
				return false;
			}

			if(m_pMap)
				delete[] m_pMap;

			m_pMap=new float[(iSize+1)*(iSize+1)];

			if(!m_pMap)
			{
				fclose(pFile);
				return false;
			}

			fread(m_pMap,(iSize+1)*(iSize+1)*sizeof(float),1,pFile);

			fclose(pFile);

			m_iWidth=iSize;
			m_iWidthPlusOne=iSize+1;
		}
	}
	else
		return false;

	return true;
}

bool Heightmap::Save(const char *szFileName, const char *szPath)
{
	char szTemp[1024];
	if(szPath)
		sprintf(szTemp,"%s%s",szPath,szFileName);
	else
		strcpy(szTemp,szFileName);

	FILE *pFile=fopen(szTemp,"wb");
	if(!pFile) return false;

	fwrite(&m_iWidth,sizeof(int),1,pFile);
	bool bScaled=m_fScale!=0.0f;
	fwrite(&bScaled,sizeof(bool),1,pFile);
	fwrite(&m_fScale,sizeof(float),1,pFile);
	fwrite(m_pMap,(m_iWidth+1) * (m_iWidth+1) * sizeof(float),1,pFile);

	fclose(pFile);
	return true;
}

void Heightmap::Clear()
{
	if(m_pMap)
	{
		delete[] m_pMap;
		m_pMap=0;
	}

	if(m_pNormals)
	{
		delete[] m_pNormals;
		m_pNormals=0;
	}

	m_iWidth=0;
	m_iWidthPlusOne=1;
	m_fScale=0.0f;
}

bool Heightmap::CreateFlat(const int iSize)
{
	Clear();

	m_pMap=new float[(iSize+1)*(iSize+1)];
	memset(m_pMap,0,(iSize+1) * (iSize+1) * sizeof(float));

	m_iWidth=iSize;
	m_iWidthPlusOne=iSize+1;

	return m_pMap!=0;
}

bool Heightmap::ApplyMask(const int iOffset,
						  const char *szMask,
						  const char *szPathName)
{
	assert(szMask && m_pMap);

	char szFullName[1024];

	if(szPathName)
		sprintf(szFullName,"%s%s",szPathName,szMask);
	else
		sprintf(szFullName,"Maps/%s",szMask);

	FILE *pFile=fopen(szFullName,"rb");
	if(!pFile)
		return false;

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

	byte *pMap=new byte[info.biWidth * info.biWidth];
	if(!pMap)
	{
		fclose(pFile);
		return false;
	}

	int max_mask=0;

	if(info.biBitCount==24)
	{
		byte *pBuffer=new byte[info.biWidth*info.biWidth*3];
		if(!pBuffer)
		{
			delete[] pMap;
			fclose(pFile);
			return false;
		}

		fread(pBuffer,info.biWidth*info.biWidth*3,1,pFile);

		int i;
		for(i=0;i<info.biWidth*info.biWidth;i++)
		{
			pMap[i]=pBuffer[i*3];
			max_mask=MAX(max_mask,pMap[i]);
		}

		delete[] pBuffer;
	}
	else if(info.biBitCount==8)
	{
		byte *pBuffer=new byte[info.biWidth*info.biWidth];
		if(!pBuffer)
		{
			delete[] pMap;
			fclose(pFile);
			return false;
		}

		fread(pBuffer,256*3,1,pFile);
		fread(pBuffer,info.biWidth*info.biWidth,1,pFile);

		int i;
		for(i=0;i<info.biWidth*info.biWidth;i++)
		{
			pMap[i]=pBuffer[i];
			max_mask=MAX(max_mask,pMap[i]);
		}
	}

	fclose(pFile);
	int iWidth=info.biWidth;

	int i,j;
	for(i=0;i<m_iWidth;i++)
	{
		const int u=i*iWidth / m_iWidth;
		for(j=0;j<m_iWidth;j++)
		{
			const int index=i+j*m_iWidth;
			int new_height=(int)(m_pMap[index]+iOffset) * pMap[u + (j*iWidth / m_iWidth) * iWidth] / max_mask;
			m_pMap[index]=(float)MIN(new_height,255);
		}
	}
	return true;
}

bool Heightmap::Smooth()
{
	assert(m_pMap);

	int i,j;
	for(i=0;i<m_iWidthPlusOne;i++)
		for(j=0;j<m_iWidthPlusOne;j++)
		{
			int count=0;
			float total=0.0f;

			int u,v;
			for(u=MAX(i-1,0);u<=MIN(i+1,m_iWidth);u++)
				for(v=MAX(j-1,0);v<=MIN(j+1,m_iWidth);v++)
				{
					count++;
					total+=m_pMap[u+v*(m_iWidth+1)];
				}

			m_pMap[i+j*m_iWidthPlusOne]=total/(float)count;
		}

	return true;
}

bool Heightmap::Scale(const float fMaxHeight)
{
	assert(m_pMap);

	if(fMaxHeight==m_fScale) return true;
	if(m_fScale!=0.0f)
		Unscale();

	if(fMaxHeight==0.0f)
	{
		m_fScale=0.0f;
		return true;
	}

	const float fFact=fMaxHeight / 255.0f;
	int i;
	float *pMap=m_pMap;
	for(i=0;i<m_iWidthPlusOne*m_iWidthPlusOne;i++,pMap++)
		(*pMap)*=fFact;

	m_fScale=fMaxHeight;

	return true;
}

bool Heightmap::Unscale()
{
	assert(m_pMap);

	if(m_fScale==0.0f) return true;

	const float fFact=255.0f / m_fScale;
	int i;
	float *pMap=m_pMap;
	for(i=0;i<m_iWidthPlusOne*m_iWidthPlusOne;i++,pMap++)
		(*pMap)*=fFact;

	m_fScale=0.0f;

	return true;
}

#define HPOS(x,y)		vPos + ((x)+(y)*m_iWidthPlusOne)

bool Heightmap::BuildNormalMap()
{
	if(!m_pMap) return false;

	if(m_pNormals) delete[] m_pNormals;
	m_pNormals=0;

	Core::Vector3 *vPos=new Core::Vector3[m_iWidthPlusOne * m_iWidthPlusOne];
	m_pNormals=new uint32[m_iWidthPlusOne * m_iWidthPlusOne];

	float *pMap=m_pMap;
	Core::Vector3 *pos=vPos;

	for(int j=0;j<m_iWidthPlusOne;j++)
	{
		const float z=(float)j;
		for(int i=0;i<m_iWidthPlusOne;i++)
		{
			pos->x=(float)i;
			pos->y=*pMap;
			pos->z=z;

			pos++;
			pMap++;
		}
	}

	uint32 *pNormal=m_pNormals;

	for(int j=0;j<m_iWidthPlusOne;j++)
	{
		for(int i=0;i<m_iWidthPlusOne;i++)
		{
			Core::Vector3 vNormal(Core::Vector3::NullVector);
			const Core::Vector3 *a,*b,*c;
			int iCount=0;

			if(j > 0 && i > 0)
			{
				a=HPOS(i-1,j-1);
				b=HPOS(i-1,j);
				c=HPOS(i,j);
				
				Core::Vector3 vLocalNorm((*c - *a) ^ (*b - *a));
				vLocalNorm.Normalize();
				vNormal=vLocalNorm;

				b=HPOS(i,j-1);
				vLocalNorm=((*b - *a) ^ (*c - *a));
				vLocalNorm.Normalize();
				(vNormal)+=vLocalNorm;
				
				iCount++;
			}

			if(i > 0 && j < m_iWidth)
			{
				a=HPOS(i-1,j);
				b=HPOS(i-1,j+1);
				c=HPOS(i,j+1);
				Core::Vector3 vLocalNorm((*c - *a) ^ (*b - *a));
				vLocalNorm.Normalize();
				(vNormal)+=vLocalNorm;

				b=HPOS(i,j+1);
				c=HPOS(i,j);
				vLocalNorm=((*b - *a) ^ (*c - *a));
				vLocalNorm.Normalize();
				(vNormal)+=vLocalNorm;
				
				iCount++;
			}

			if(i < m_iWidth && j < m_iWidth)
			{
				a=HPOS(i,j+1);
				b=HPOS(i,j);
				c=HPOS(i+1,j+1);
				Core::Vector3 vLocalNorm((*c - *a) ^ (*b - *a));
				vLocalNorm.Normalize();
				(vNormal)+=vLocalNorm;

				b=HPOS(i+1,j+1);
				c=HPOS(i+1,j);
				vLocalNorm=((*b - *a) ^ (*c - *a));
				vLocalNorm.Normalize();
				(vNormal)+=vLocalNorm;
				
				iCount++;
			}

			if(i < m_iWidth && j > 0)
			{
				a=HPOS(i,j-1);
				b=HPOS(i,j);
				c=HPOS(i+1,j);
				Core::Vector3 vLocalNorm((*b - *a) ^ (*c - *a));
				vLocalNorm.Normalize();
				(vNormal)+=vLocalNorm;

				b=HPOS(i,j-1);
				c=HPOS(i+1,j-1);
				vLocalNorm=((*b - *a) ^ (*c - *a));
				vLocalNorm.Normalize();
				(vNormal)+=vLocalNorm;
				
				iCount++;
			}

			if(iCount > 0) vNormal.Normalize();
			else (vNormal)=Core::Vector3::YAxisVector;

			((Core::Color32 *)pNormal)->r=(byte)((vNormal.x + 1.0f) * 127.0f);
			((Core::Color32 *)pNormal)->g=(byte)((vNormal.y + 1.0f) * 127.0f);
			((Core::Color32 *)pNormal)->b=(byte)((vNormal.z + 1.0f) * 127.0f);

			pNormal++;
		}
	}

	delete[] vPos;

	return true;
}

bool Heightmap::BuildTexture()
{
	if(!m_pMap || m_iWidth==0) return false;

	if(m_hTexture)
		g_TextureMgr.ReleaseTexture(m_hTexture);

	const float fTexFact=(float)(m_iWidthPlusOne) / (float)m_iWidth;
	const float fHeightFact=m_fScale==0.0f ? 1.0f : (255.0f / m_fScale);
	Core::Color32 *pTexture=new Core::Color32[m_iWidth*m_iWidth];
	Core::Color32 *pTex=pTexture;

	if(m_pNormals)
	{
		for(int y=0;y<m_iWidth;y++)
		{
			float *pMap=m_pMap + y*m_iWidthPlusOne;
			Core::Color32 *pNorm=(Core::Color32 *)(m_pNormals + y*m_iWidthPlusOne);

			for(int x=0;x<m_iWidth;x++)
			{
				const int xf=(int)((float)x * fTexFact);

				*pTex=pNorm[xf];
				pTex->a=(byte)(pMap[xf] * fHeightFact);

				pTex++;
			}
		}
	}
	else
	{
		for(int y=0;y<m_iWidth;y++)
		{
			float *pMap=m_pMap + y*m_iWidthPlusOne;

			for(int x=0;x<m_iWidth;x++)
			{
				const int xf=(int)((float)x * fTexFact);

				pTex->r=0;
				pTex->g=127;
				pTex->b=0;
				pTex->a=(byte)(pMap[xf] * fHeightFact);

				pTex++;
			}
		}
	}

	m_hTexture=g_TextureMgr.CreateTexture(m_iWidth,m_iWidth,D3DFMT_A8R8G8B8,pTexture,"Heightmap_tex");
	g_TextureMgr.CompressTexture(m_hTexture);
	delete[] pTexture;

	return !m_hTexture.IsNull();
}

bool Heightmap::RayIntersect(const Core::Vector3 &vOrigin, const Core::Vector3 &vDir,
							 Core::Vector3& vIntersect)
{
	Core::Vector3 vStart(vOrigin);

	const int iSteps=m_iWidth*4;
	const float fWidth=(float)m_iWidth;
	const float fMaxY=m_fScale==0.0f ? 255.0f : m_fScale;

	for(int i=0;i<iSteps;i++)
	{
		const int xs=(int)vStart.x;
		const int ys=(int)vStart.z;
		const int xd=(int)(vStart.x + vDir.x);
		const int yd=(int)(vStart.z + vDir.z);

		const float hs=GetHeight(xs,ys);
		const float hd=GetHeight(xd,yd);
		const float ydir=vStart.y + vDir.y;

#ifdef DEEXPORT
		if(xs >= 0 && ys >= 0 && xd >= 0 && yd >= 0 &&
			xs <= m_iWidth && ys <= m_iWidth && xd <= m_iWidth && yd <= m_iWidth)
		{
#endif
		if((vStart.y < hs && ydir >= hd) ||
		   (vStart.y >= hs && ydir < hd))
		{
			vIntersect=vStart;
			return true;
		}
#ifdef DEEXPORT
		}
#endif

		if((vStart.x < 0.0f && vDir.x <= 0.0f) || (vStart.x > fWidth && vDir.x >= 0.0f)) return false;
		if((vStart.z < 0.0f && vDir.z <= 0.0f) || (vStart.z > fWidth && vDir.z >= 0.0f)) return false;
		if((vStart.y < 0.0f && vDir.y <= 0.0f) || (vStart.y > fMaxY && vDir.y >= 0.0f)) return false;

		vStart+=vDir;
	}

	return false;
}

float Heightmap::GetInterpolatedHeight(float x, float y)
{
	const int ix=(int)x;
	const int iy=(int)y;
	const int ix1=ix+1;
	const int iy1=iy+1;

	const float fx=x - (float)ix;
	const float fy=y - (float)iy;

	const float ha=GetHeight(ix,iy);
	const float hb=GetHeight(ix,iy1);
	const float hc=GetHeight(ix1,iy);
	const float hd=GetHeight(ix1,iy1);

	const float dab=hb-ha;
	const float dcd=hd-hc;

	const float he=ha + dab * fy;
	const float hf=hc + dcd * fy;

	const float def=hf-he;
	return he + def * fx;
}

void Heightmap::GetNormalByVector(const int x, const int y, Core::Vector3& v)
{
	Core::Color32 c;
	c.dwRGB=GetNormal(x,y);

#define NORMAL_FACTOR		(1.0f / 127.0f)

	v.x=((float)(((int)c.r) - 127)) * NORMAL_FACTOR;
	v.y=((float)(((int)c.g) - 127)) * NORMAL_FACTOR;
	v.z=((float)(((int)c.b) - 127)) * NORMAL_FACTOR;
}

void Heightmap::GetInterpolatedNormal(float x, float y, Core::Vector3& v)
{
	const int ix=(int)x;
	const int iy=(int)y;
	const int ix1=ix+1;
	const int iy1=iy+1;

	const float fx=x - (float)ix;
	const float fy=y - (float)iy;

	Core::Vector3 na,nb,nc,nd;
	GetNormalByVector(ix,iy,na);
	GetNormalByVector(ix,iy1,nb);
	GetNormalByVector(ix1,iy,nc);
	GetNormalByVector(ix1,iy1,nd);

	Core::Vector3 dab(nb); dab-=na;
	Core::Vector3 dcd(nd); dcd-=nc;

	Core::Vector3 ne(dab); ne*=fy; ne+=na; // ne=na+dab*fy
	Core::Vector3 nf(dcd); nf*=fy; nf+=nf; // nf=nc+dcd*fy

	v=nf; v-=ne;
	v*=fx; v+=ne; // v=he+(hf-he)*fx;
}

}