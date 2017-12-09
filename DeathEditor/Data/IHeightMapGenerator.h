
#ifndef _IHEIGHTMAPGENERATOR_H_
#define _IHEIGHTMAPGENERATOR_H_

#include <stdlib.h>

class IHeightMapGenerator
{
	public:

		IHeightMapGenerator() {}
		virtual ~IHeightMapGenerator() {}

		virtual int GetParametersCount() const=0;
		virtual bool GetParameter(int iIndex, char *szName,
			float **pValue, float &fRangeMin, float &fRangeMax,
			float &fStep, int &iDecimals)=0;

		virtual bool GenerateHeightMap(int iSize, float *pMap)=0;

};

class PerlinGenerator : public IHeightMapGenerator
{
	public:

		PerlinGenerator();
		~PerlinGenerator();

		int GetParametersCount() const { return 3; }
		bool GetParameter(int iIndex, char *szName,
			float **pValue, float &fRangeMin, float &fRangeMax,
			float &fStep, int &iDecimals);

		bool GenerateHeightMap(int iSize, float *pMap);

	private:

		float m_fFrequency;
		float m_fOctaves;
		float m_fPersistance;

		class Mask
		{
			public:

				Mask() : m_pMask(0),m_iSize(-1),m_fPersistance(1.0f) {};
				~Mask();

				bool InitMask(const int iSize, const float fPersistance);
				void Randomize();

				int m_iSize;
				float m_fPersistance;
				int *m_pMask;
		};

		int Interpolate(const int y1, const int y2,
			const int n, const int delta);
		int InterpolatedValue(const int i, const int j,
			const int iFrequency, Mask *pMask);

};

class MidPointGenerator : public IHeightMapGenerator
{
	public:

		MidPointGenerator();
		~MidPointGenerator();

		int GetParametersCount() const { return 1; }
		bool GetParameter(int iIndex, char *szName,
			float **pValue, float &fRangeMin, float &fRangeMax,
			float &fStep, int &iDecimals);

		bool GenerateHeightMap(int iSize, float *pMap);


	private:

		float m_fRough;

		float RangedRandom(float v1,float v2)
		{
			return v1 + (v2-v1)*((float)rand())/((float)RAND_MAX);
		}
};

class FaultFormationGenerator : public IHeightMapGenerator
{
	public:

		FaultFormationGenerator();
		~FaultFormationGenerator();

		int GetParametersCount() const { return 5; }
		bool GetParameter(int iIndex, char *szName,
			float **pValue, float &fRangeMin, float &fRangeMax,
			float &fStep, int &iDecimals);

		bool GenerateHeightMap(int iSize, float *pMap);

	private:

		float m_fIterations;
		float m_fMin;
		float m_fMax;
		float m_fItFilter;
		float m_fFilter;

		float RangedRandom(float v1,float v2)
		{
			return v1 + (v2-v1)*((float)rand())/((float)RAND_MAX);
		}

		void FilterHeightBand(float *band,int stride,int count,float filter);
		void FilterHeightField(int size,float filter, float *field);

};

#endif