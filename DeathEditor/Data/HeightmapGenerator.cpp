
#include "IHeightMapGenerator.h"
#include <math.h>
#include "../stdafx.h"
#include <mmsystem.h>
#include "../../DeathEngine/Common.h"

const char *szPerlinGParamNames[]={"Fréquence","Octaves","Persistance"};
const char *szMidPointGParamNames[]={"Rough"};
const char *szFaultFormationGParamNames[]={"Iterations","Min","Max","Iterations filtre","Filtre"};

PerlinGenerator::PerlinGenerator() : m_fFrequency(4.0f),m_fOctaves(2.0f),m_fPersistance(20.0f)
{
	srand(timeGetTime());
}

PerlinGenerator::~PerlinGenerator() {}

bool PerlinGenerator::GenerateHeightMap(int iSize, float *pMap)
{
	const int iFrequency=(int)m_fFrequency;
	const int iOctaves=(int)m_fOctaves;
	const float fPersistance=m_fPersistance * 0.01f;

	iSize+=1;

	Mask random;
	if(!random.InitMask(iSize,1.0f))
		return false;
	random.Randomize();

	Mask c;
	c.InitMask(iSize,1.0f);

	int i,j,n,current_frequency;
	int a;
	float step,sum_persistances;

	step=(float)(iSize) / (float)iFrequency;
	float current_persistance=fPersistance;

	Mask *masks=new Mask[iOctaves];
	for(i=0;i<iOctaves;i++)
	{
		masks[i].InitMask(iSize,current_persistance);
		current_persistance*=fPersistance;
	}

	current_frequency=iFrequency;
	for(n=0;n<iOctaves;n++)
	{
		for(j=0;j<iSize;j++)
			for(i=0;i<iSize;i++)
			{
				a=InterpolatedValue(i,j,current_frequency,&random);
				masks[n].m_pMask[i+j*iSize]=a;
			}

		current_frequency*=iFrequency;
	}

	sum_persistances=0.0f;
	for(i=0;i<iOctaves;i++)
		sum_persistances+=masks[i].m_fPersistance;

	const float isum_persistances=1.0f / sum_persistances;

	for(j=0;j<iSize;j++)
	{
		int index=j*iSize;

		for(i=0;i<iSize;i++,index++)
		{
			for(n=0;n<iOctaves;n++)
				c.m_pMask[index]+=masks[n].m_pMask[index] * masks[n].m_fPersistance;

			pMap[index]=(float)((float)c.m_pMask[index] * isum_persistances);
		}
	}

	delete[] masks;

	return true;
}

int PerlinGenerator::InterpolatedValue(const int i, const int j,
								  const int iFrequency,
								  Mask *pMask)
{
    int borne1x, borne1y, borne2x, borne2y, q;
	float step=(float)pMask->m_iSize / (float)iFrequency;

	q=(float)i / step;
    borne1x=q*step;
    borne2x=(q+1)*step;
	borne2x=MIN(borne2x,pMask->m_iSize-1);

    q=(float)j / step;
    borne1y=q*step;
    borne2y=(q+1)*step;
	borne2y=MIN(borne2y,pMask->m_iSize-1);

    int b00,b01,b10,b11;
	b00=pMask->m_pMask[borne1x + borne1y * pMask->m_iSize];
	b01=pMask->m_pMask[borne1x + borne2y * pMask->m_iSize];
	b10=pMask->m_pMask[borne2x + borne1y * pMask->m_iSize];
	b11=pMask->m_pMask[borne2x + borne2y * pMask->m_iSize];
	
    int v1=Interpolate(b00, b01, borne2y-borne1y, j-borne1y);
	int v2=Interpolate(b10, b11, borne2y-borne1y, j-borne1y);
    int end=Interpolate(v1, v2, borne2x-borne1x , i-borne1x);

	return end;
}

int PerlinGenerator::Interpolate(const int y1, const int y2,
							const int n, const int delta)
{
    if(n==0)
        return y1;
    if(n==1)
        return y2;

    float a = (float)delta / (float)n;

	float fac1=3.0f * (1.0f-a) * (1.0f-a) - 2.0f * (1.0f-a) * (1.0f-a) * (1.0f-a);
	float fac2=3.0f * a * a - 2.0f * a * a * a;

	return (int)((float)y1*fac1 + (float)y2*fac2);
}

PerlinGenerator::Mask::~Mask()
{
	if(m_pMask)
		delete[] m_pMask;

	m_pMask=0;
	m_iSize=-1;
	m_fPersistance=1.0f;
}

bool PerlinGenerator::Mask::InitMask(const int iSize, const float fPersistance)
{
	m_pMask=new int[iSize*iSize];

	if(m_pMask)
	{
		m_iSize=iSize;
		m_fPersistance=fPersistance;

		memset(m_pMask,0,iSize*iSize*sizeof(int));
		return true;
	}
	else
		return false;
}

void PerlinGenerator::Mask::Randomize()
{
	if(m_pMask)
	{
		int i;
		for(i=0;i<m_iSize*m_iSize;i++)
			m_pMask[i]=(float)rand() / RAND_MAX * 256;
	}
}

bool PerlinGenerator::GetParameter(int iIndex, char *szName,
								   float **pValue, float &fRangeMin,
								   float &fRangeMax, float &fStep,
								   int &iDecimals)
{
	switch(iIndex)
	{
		case 0:
			*pValue=&m_fFrequency;
			fRangeMin=1.0f;
			fRangeMax=16000.0f;
			fStep=1.0f;
			iDecimals=0;
			break;

		case 1:
			*pValue=&m_fOctaves;
			fRangeMin=1.0f;
			fRangeMax=32.0f;
			fStep=1.0f;
			iDecimals=0;
			break;

		case 2:
			*pValue=&m_fPersistance;
			fRangeMin=1.0f;
			fRangeMax=200.0f;
			fStep=0.01f;
			iDecimals=2;
			break;

		default:
			return false;
	}

	strcpy(szName,szPerlinGParamNames[iIndex]);
	return true;
}

/* <---------------------------------------------------------------------------------------> */

MidPointGenerator::MidPointGenerator() : m_fRough(100.0f) {}
MidPointGenerator::~MidPointGenerator(){}

bool MidPointGenerator::GenerateHeightMap(int iSize,
								 float *pField)
{
	const float fRough=m_fRough * 0.01f;
	iSize+=1;
	int i,j,ni,nj,mi,mj,pmi,pmj,rectSize = iSize;
	float dh = (float)rectSize/2,r = (float)pow(2.0,-1.0*fRough);

	pField[0] = 0;
	while(rectSize > 0)
	{
		for (i=0;i<iSize;i+=rectSize)
		for (j=0;j<iSize;j+=rectSize)
		{
			ni = (i+rectSize)%iSize;
			nj = (j+rectSize)%iSize;

			mi = (i+rectSize/2);
			mj = (j+rectSize/2);

			pField[mi+mj*iSize]=(pField[i+j*iSize] + pField[ni+j*iSize] + pField[i+nj*iSize] + pField[ni+nj*iSize])/4 + RangedRandom(-dh/2,dh/2);
		}

		for (i=0;i<iSize;i+=rectSize)
		for (j=0;j<iSize;j+=rectSize)
		{
			ni = (i+rectSize)%iSize;
			nj = (j+rectSize)%iSize;

			mi = (i+rectSize/2);
			mj = (j+rectSize/2);

			pmi = (i-rectSize/2+iSize)%iSize;
			pmj = (j-rectSize/2+iSize)%iSize;

			pField[mi+j*iSize]=(pField[i+j*iSize] + pField[ni+j*iSize] + pField[mi+pmj*iSize] + pField[mi+mj*iSize])/4 + RangedRandom(-dh/2,dh/2);
			pField[i+mj*iSize]=(pField[i+j*iSize] + pField[i+nj*iSize] + pField[pmi+mj*iSize] + pField[mi+mj*iSize])/4 + RangedRandom(-dh/2,dh/2);
		}

		rectSize /= 2;
		dh *= r;
	}

	float maxVal,minVal;

	maxVal=pField[0];
	minVal=pField[0];

	for(i=1;i<iSize*iSize;i++)
	{
		if (pField[i] > maxVal) 
			maxVal=pField[i];
		else if(pField[i] < minVal) 
			minVal=pField[i];
	}

	dh=maxVal-minVal;
	for(i=0;i<iSize*iSize;i++)
		pField[i]=((pField[i]-minVal)*255.0f /dh);

	return true;
}

bool MidPointGenerator::GetParameter(int iIndex, char *szName,
									 float **pValue, float &fRangeMin,
									 float &fRangeMax, float &fStep, int &iDecimals)
{
	if(iIndex!=0)
		return false;

	*pValue=&m_fRough;
	fRangeMin=1.0f;
	fRangeMax=1000.0f;
	fStep=0.01f;
	iDecimals=2;
	strcpy(szName,szMidPointGParamNames[0]);

	return true;
}

/*  <----------------------------------------------------------------------------->  */

FaultFormationGenerator::FaultFormationGenerator() :
	m_fIterations(64.0f), m_fItFilter(5.0f), m_fMin(0.0f),m_fMax(255.0f),m_fFilter(50.0f) {}
FaultFormationGenerator::~FaultFormationGenerator() {}

bool FaultFormationGenerator::GenerateHeightMap(int iWidth,
									   float *pField)
{
	iWidth+=1;

	const int iIteration=(int)m_fIterations;
	const int iItFilter=(int)m_fItFilter;
	const float fFilter=m_fFilter * 0.01f;

	int x1,y1,x2,y2,dx1,dy1,dx2,dy2,i,dHeight;

	for (i=0;i<iWidth*iWidth;i++)
		pField[i] = 0.0f;

	for (i=0;i<iIteration;i++)
	{
		dHeight=m_fMin-((m_fMax-m_fMin)*i)/iIteration;

		x1 = rand()%iWidth;
		y1 = rand()%iWidth;
		do
		{
			x2 = rand()%iWidth;
			y2 = rand()%iWidth;
		}while (x2==x1 && y2==y1);
		
		dx1 = x2 - x1;
		dy1 = y2 - y1;
		
		for (x2 = 0;x2<iWidth;x2++)
		for (y2 = 0;y2<iWidth;y2++)
		{
			dx2 = x2-x1;
			dy2 = y2-y1;

			if (dx2*dy1 - dx1*dy2 > 0)
				pField[x2+iWidth*y2] += (float)(dHeight);
		}

		if (iItFilter!=0 && (i%iItFilter)==0)
			FilterHeightField(iWidth,fFilter,pField);
	}

	float maxVal,minVal,dh;

	maxVal=pField[0];
	minVal=pField[0];

	for(i=1;i<iWidth*iWidth;i++)
	{
		if (pField[i] > maxVal) 
			maxVal=pField[i];
		else if(pField[i] < minVal) 
			minVal=pField[i];
	}

	dh=maxVal-minVal;
	for(i=0;i<iWidth*iWidth;i++)
		pField[i]=((pField[i]-minVal)*255.0f /dh);

	return true;
}

void FaultFormationGenerator::FilterHeightBand(float *band,int stride,int count,float filter)
{
	int i,j=stride;
	float v = band[0];
	for (i=0;i<count-1;i++)
	{
		band[j] = filter*v + (1-filter)*band[j];
		v = band[j];
		j+=stride;
	}
}

void FaultFormationGenerator::FilterHeightField(int size,float filter, float *field)
{
	int i;

	for (i=0;i<size;i++)
		FilterHeightBand(&field[size*i],1,size,filter);
	
	for (i=0;i<size;i++)
		FilterHeightBand(&field[size*i+size-1],-1,size,filter);

	for (i=0;i<size;i++)
		FilterHeightBand(&field[i],size,size,filter);

	for (i=0;i<size;i++)
		FilterHeightBand(&field[size*(size-1)+i],-size,size,filter);
}

bool FaultFormationGenerator::GetParameter(int iIndex, char *szName, float **pValue,
										   float &fRangeMin, float &fRangeMax,
										   float &fStep, int &iDecimals)
{
	switch(iIndex)
	{
		case 0:
			*pValue=&m_fIterations;
			fRangeMin=1.0f;
			fRangeMax=1024.0f;
			fStep=1.0f;
			iDecimals=0;
			break;

		case 1:
			*pValue=&m_fMin;
			fRangeMin=-16000.0f;
			fRangeMax=1600.0f;
			fStep=0.1f;
			iDecimals=1;
			break;

		case 2:
			*pValue=&m_fMax;
			fRangeMin=-16000.0f;
			fRangeMax=16000.0f;
			fStep=0.1f;
			iDecimals=1;
			break;

		case 3:
			*pValue=&m_fItFilter;
			fRangeMin=0.0f;
			fRangeMax=256.0f;
			fStep=1.0f;
			iDecimals=0;
			break;

		case 4:
			*pValue=&m_fFilter;
			fRangeMin=0.0f;
			fRangeMax=6400.0f;
			fStep=0.1f;
			iDecimals=1;
			break;

		default:
			return false;
	}

	strcpy(szName,szFaultFormationGParamNames[iIndex]);
	return true;
}

