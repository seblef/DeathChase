
#ifndef _BBOX_H_

#define _BBOX_H_

#include "Vector.h"

namespace Core
{

class BSphere;

class BBox
{
	public:

		BBox() {};
		BBox(const BBox& Box) : m_vMin(Box.m_vMin),m_vMax(Box.m_vMax) {};
		BBox(const Vector3& vMin,
			const Vector3& vMax) : m_vMin(vMin),m_vMax(vMax) {};
		BBox(const BSphere *pSphere);
		~BBox() {};

		const Vector3& GetMin() const { return m_vMin; };
		const Vector3& GetMax() const { return m_vMax; };

		void SetMin(const Vector3& vMin) { m_vMin=vMin; };
		void SetMax(const Vector3& vMax) { m_vMax=vMax; };

		Vector3 GetSize() const { return m_vMax - m_vMin; }
		Vector3 GetCenter() const { return (m_vMax+m_vMin)*0.5f; }

		void Repair()
		{
			float fTemp;

			if(m_vMin.x > m_vMax.x) { fTemp=m_vMax.x; m_vMax.x=m_vMin.x; m_vMin.x=fTemp; }
			if(m_vMin.y > m_vMax.y) { fTemp=m_vMax.y; m_vMax.y=m_vMin.y; m_vMin.y=fTemp; }
			if(m_vMin.z > m_vMax.z) { fTemp=m_vMax.z; m_vMax.z=m_vMin.z; m_vMin.z=fTemp; }
		}

		void Adjust(const Vector3& v)
		{
			if(v.x < m_vMin.x) m_vMin.x=v.x;
			if(v.x > m_vMax.x) m_vMax.x=v.x;

			if(v.y < m_vMin.y) m_vMin.y=v.y;
			if(v.y > m_vMax.y) m_vMax.y=v.y;

			if(v.z < m_vMin.z) m_vMin.z=v.z;
			if(v.z > m_vMax.z) m_vMax.z=v.z;
		}
		void Adjust(const BBox& box)
		{
			Adjust(box.GetMin());
			Adjust(box.GetMax());
		}

		bool IsIn(const Vector3& v) const
		{
			return (v.x >= m_vMin.x && v.y >= m_vMin.y && v.z >= m_vMin.z &&
					v.x <= m_vMax.x && v.y <= m_vMax.y && v.z <= m_vMax.z);
		}

		bool IsIn(const Vector3& v1,
			const Vector3& v2,
			const Vector3& v3) const
		{
			if((v1.x < m_vMin.x && v2.x < m_vMin.x && v3.x < m_vMin.x) ||
			   (v1.x > m_vMax.x && v2.x > m_vMax.x && v3.x > m_vMax.x) ||
			   (v1.y < m_vMin.y && v2.y < m_vMin.y && v3.y < m_vMin.y) ||
			   (v1.y > m_vMax.y && v2.y > m_vMax.y && v3.y > m_vMax.y) ||
			   (v1.z < m_vMin.z && v2.z < m_vMin.z && v3.z < m_vMin.z) ||
			   (v1.z > m_vMax.z && v2.z > m_vMax.z && v3.z > m_vMax.z))
			   return false;
			else
				return true;
		}

		bool IsIn(const BBox& Box, bool bCompletly=false) const
		{
			if(bCompletly)
				return IsIn(Box.m_vMin) && IsIn(Box.m_vMax);
			else
			{
				if(Box.GetMax().x < m_vMin.x || Box.GetMin().x > m_vMax.x ||
				   Box.GetMax().y < m_vMin.y || Box.GetMin().y > m_vMax.y ||
				   Box.GetMax().z < m_vMin.z || Box.GetMin().z > m_vMax.z)
					return false;
				else
					return true;
			}
		}

		bool IsIn(const Vector3& vOrigin, const Vector3& vTo) const
		{
			if((vOrigin.x < m_vMin.x && vTo.x < m_vMin.x) ||
			   (vOrigin.x > m_vMax.x && vTo.x > m_vMax.x))
			   return false;

			if((vOrigin.y < m_vMin.y && vTo.y < m_vMin.y ) ||
			   (vOrigin.y > m_vMax.y && vTo.y > m_vMax.y))
			   return false;

			if((vOrigin.z < m_vMin.z && vTo.z < m_vMin.z) ||
			   (vOrigin.z > m_vMax.z && vTo.z > m_vMax.z))
			   return false;

			return true;
		}

		bool IsIn(const BSphere *pSphere, bool bCompletely=false) const;

		void Init() { m_vMin=Vector3(1e23f,1e23f,1e23f); m_vMax=Vector3(-1e23f,-1e23f,-1e23f); };
		void Init(const Vector3& v) { m_vMin=v; m_vMax=v; };

		inline void ToSphere(BSphere *pSphere) const;
		inline void FromSphere(const BSphere *pSphere);

		Vector3 m_vMin;
		Vector3 m_vMax;

};

class BBox2D
{
	public:

		BBox2D() {};
		BBox2D(const BBox2D& Box) : m_vMin(Box.m_vMin), m_vMax(Box.m_vMax) {};
		BBox2D(const Vector2& vMin,
			const Vector2& vMax) : m_vMin(vMin),m_vMax(vMax) {};
		~BBox2D() {};

		const Vector2& GetMin() const { return m_vMin; };
		const Vector2& GetMax() const { return m_vMax; };

		void SetMin(const Vector2& vMin) { m_vMin=vMin; };
		void SetMax(const Vector2& vMax) { m_vMax=vMax; };

		void Adjust(const Vector2& v)
		{
			if(v.x < m_vMin.x) m_vMin.x=v.x;
			if(v.x > m_vMax.x) m_vMax.x=v.x;

			if(v.y < m_vMin.y) m_vMin.y=v.y;
			if(v.y > m_vMax.y) m_vMax.y=v.y;
		}

		bool IsIn(const Vector2& v) const
		{
			return (v.x >= m_vMin.x && v.y >= m_vMin.y &&
					v.x <= m_vMax.x && v.y <= m_vMax.y);
		}
		bool IsOut(const Vector2& v) const { return !IsIn(v); };

		bool IsIn(const BBox2D& Box, bool bCompletly=false)
		{
			if(bCompletly)
				return IsIn(Box.m_vMin) && IsIn(Box.m_vMax);
			else
				return false;		//		TODO: implémenter l'autre cas
		}
		bool IsOut(const BBox2D& Box) { return !IsIn(Box); };

		Vector2 m_vMin;
		Vector2 m_vMax;

};

}

#endif