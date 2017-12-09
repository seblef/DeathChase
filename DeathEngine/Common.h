
#ifndef _COMMON_H_

#define _COMMON_H_

#define _CRT_SECURE_NO_DEPRECATE

//				Redéfinition des types


typedef unsigned short int uint16;
typedef unsigned long int uint32;
typedef signed short int sint16;
typedef signed long int sint32;
typedef unsigned char byte;
typedef __int64 uint64;

typedef struct _VectorString
{
	char szText[32];

} VectorString;


//				Macros


#define MYRGB(r,g,b)			((r << 16) | (g << 8) | (b))

#define KINV(x)					((x) != 0.0f ? (1.0f/(x)) : 0.0f)

#define MIN(x,y)				((x) <= (y) ? (x) : (y))
#define MAX(x,y)				((x) >= (y) ? (x) : (y))

#define KEYDOWN(k)				((k) & 0x80)

#define RAD_TO_DEG(x)			((x)*57.296f)

#define PI						3.14159265359f
#define DEGTORAD				(PI / 180.0f)
#define RADTODEG				(180.0f / PI)

#define INVALID_ID				0xffffffff

#define EPSILON       0.0001f
#define SQREPSILON    0.000001f

inline float FastInvSqrt( float fN )
{
	unsigned int iN;
	float fTemp;

	fTemp  = fN;
	iN     = *(long*)&fTemp;
	iN     = 0x5f3759df - ( iN >> 1 );
	fTemp  = *(float*)&iN;
	fTemp  = fTemp * ( 1.5f - ( fN * 0.5f * fTemp * fTemp ) );

	return fTemp;
}

inline void memset32 ( void * dest, const uint32 value, uint32 bytesize )
{
	uint32 *d=(uint32*) dest;
	uint32 i;

	i = bytesize >> ( 2 + 3 );
	while(i)
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		i -= 1;
	}

	i = (bytesize >> 2 ) & 7;
	while( i )
	{
		d[0] = value;
		d += 1;
		i -= 1;
	}

}


#define RAND			(float(rand()) / float(RAND_MAX))

#endif
