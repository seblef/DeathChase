
#ifndef _BSPHERE_H_
#define _BSPHERE_H_

#include "Vector.h"

namespace Core
{

class BBox;

class BSphere
{
	public:

		BSphere() : m_vCenter(Vector3::NullVector), m_fRadius(0.0f), m_fRadiusSq(0.0f) {}
		BSphere(const Vector3& vCenter, float fRadius) :
			m_vCenter(vCenter), m_fRadius(fRadius), m_fRadiusSq(fRadius*fRadius) {}
		BSphere(float x, float y, float z, float fRadius) :
			m_vCenter(x,y,z), m_fRadius(fRadius), m_fRadiusSq(fRadius * fRadius) {}
		BSphere(const BSphere& sphere) :
			m_vCenter(sphere.m_vCenter), m_fRadius(sphere.m_fRadius), m_fRadiusSq(sphere.m_fRadiusSq) {}
		inline BSphere(const BBox *pBBox);
		~BSphere() {}

		const BSphere& operator=(const BSphere& sphere)
		{
			m_vCenter=sphere.m_vCenter;
			m_fRadius=sphere.m_fRadius;
			m_fRadiusSq=sphere.m_fRadiusSq;
			return *this;
		}

		BSphere operator+(const Vector3& vTranslate) const { return BSphere(m_vCenter+vTranslate,m_fRadius); }
		const BSphere& operator+=(const Vector3& vTranslate) { m_vCenter+=vTranslate; return *this; }

		BSphere operator-(const Vector3& vTranslate) const { return BSphere(m_vCenter-vTranslate,m_fRadius); }
		const BSphere& operator-=(const Vector3& vTranslate) { m_vCenter-=vTranslate; return *this; }

		BSphere operator*(const float fRadFact) const { return BSphere(m_vCenter,m_fRadius*fRadFact); }
		const BSphere& operator*(const float fRadFact) { m_fRadius*=fRadFact; m_fRadiusSq=m_fRadius*m_fRadius; }

		void SetCenter(const Vector3& vCenter) { m_vCenter=vCenter; }
		const Vector3& GetCenter() const { return m_vCenter; }

		void SetRadius(const float fRadius) { m_fRadius=fRadius; m_fRadiusSq=fRadius*fRadius; }
		float GetRadius() const { return m_fRadius; }
		float GetRadiusSq() const { return m_fRadiusSq; }

		void Adjust(const Vector3& v)
		{
			const float fSqLen=(v-m_vCenter).GetSquareLength();
			if(fSqLen > m_fRadiusSq)
			{
				m_fRadius=sqrtf(fSqLen);
				m_fRadiusSq=fSqLen;
			}
		}

		bool IsPointInside(const Vector3& v) const { return (v-m_vCenter).GetSquareLength() <= m_fRadiusSq; }
		bool IsSphereInside(const BSphere& sphere) const { return ((sphere.m_vCenter-m_vCenter).GetSquareLength() - sphere.m_fRadiusSq) <= m_fRadiusSq; }
		inline bool IsBoxInside(const BBox *pBBox) const;

		void ToBBox(BBox *pBBox) const;
		void FromBBox(const BBox *pBBox);

	protected:

		float m_fRadius;
		float m_fRadiusSq;
		Vector3 m_vCenter;

};

}

#endif