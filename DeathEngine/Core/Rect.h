
#ifndef _RECT_H_

#define _RECT_H_

#include "Vector.h"

namespace Core
{

class Rect
{
	public:

		Rect() : m_vUpperLeft(0,0), m_vLowerRight(0,0) {};
		Rect(int x, int y, int x2, int y2) : m_vUpperLeft(x,y), m_vLowerRight(x2,y2) {};
		Rect(const Vector2i& vUpperLeft, const Vector2i& vLowerRight) : m_vUpperLeft(vUpperLeft), m_vLowerRight(vLowerRight) {};
		Rect(const Rect& _Rect) : m_vUpperLeft(_Rect.m_vUpperLeft), m_vLowerRight(_Rect.m_vLowerRight) {};

		Rect operator+(const Vector2i& v) const { return Rect(m_vUpperLeft + v, m_vLowerRight + v); };
		const Rect& operator+=(const Vector2i& v) { m_vUpperLeft+=v; m_vLowerRight+=v; return *this; };

		Rect operator-(const Vector2i& v) const { return Rect(m_vUpperLeft-v,m_vLowerRight-v); };
		const Rect& operator-=(const Vector2i& v) { m_vUpperLeft-=v; m_vLowerRight-=v; return *this; };

		bool operator==(const Rect& rect) const { return (m_vUpperLeft==rect.m_vUpperLeft && m_vLowerRight==rect.m_vLowerRight); };
		bool operator!=(const Rect& rect) const { return (m_vUpperLeft!=rect.m_vUpperLeft || m_vLowerRight!=rect.m_vLowerRight); };

		bool operator < (const Rect& rect) const { return GetArea() < rect.GetArea(); };
		bool operator <= (const Rect& rect) const { return GetArea() <= rect.GetArea(); };

		bool operator > (const Rect& rect) const { return GetArea() > rect.GetArea(); };
		bool operator >= (const Rect& rect) const { return GetArea() >= rect.GetArea(); };

		int GetArea() const { return GetWidth() * GetHeight(); };

		bool IsPointInside(const Vector2i& p) const { return (p.x >= m_vUpperLeft.x && p.y >= m_vUpperLeft.y &&
			p.x <= m_vLowerRight.x && p.y <= m_vLowerRight.y); };

		bool IsRectCollided(const Rect& rect) const
		{
			return (m_vLowerRight.y > rect.m_vUpperLeft.y &&
					m_vUpperLeft.y < rect.m_vLowerRight.y &&
					m_vLowerRight.x > rect.m_vUpperLeft.x &&
					m_vUpperLeft.x < rect.m_vLowerRight.x); };

		void ClipAgainst(const Rect& rect)
		{
			if(rect.m_vLowerRight.x < m_vLowerRight.x) m_vLowerRight.x=rect.m_vLowerRight.x;
			if(rect.m_vLowerRight.y < m_vLowerRight.y) m_vLowerRight.y=rect.m_vLowerRight.y;

			if(rect.m_vUpperLeft.x > m_vUpperLeft.x) m_vUpperLeft.x=rect.m_vUpperLeft.x;
			if(rect.m_vUpperLeft.y > m_vUpperLeft.y) m_vUpperLeft.y=rect.m_vUpperLeft.y;

			if(m_vUpperLeft.y > m_vLowerRight.y) m_vUpperLeft.y=m_vLowerRight.y;
			if(m_vUpperLeft.x > m_vLowerRight.x) m_vUpperLeft.x=m_vLowerRight.x;
		};

		bool ConstrainTo(const Rect& rect)
		{
			if(rect.GetWidth() < GetWidth() || rect.GetHeight() < GetHeight())
				return false;

			int diff=rect.m_vLowerRight.x - m_vLowerRight.x;
			if(diff < 0) { m_vLowerRight.x+=diff; m_vUpperLeft.x+=diff; }

			diff=rect.m_vLowerRight.y - m_vLowerRight.y;
			if(diff < 0) { m_vLowerRight.y+=diff; m_vUpperLeft.y+=diff; }

			diff=m_vUpperLeft.x - rect.m_vUpperLeft.x;
			if(diff < 0) { m_vUpperLeft.x-=diff; m_vLowerRight.x-=diff; }

			diff=m_vUpperLeft.y - rect.m_vUpperLeft.y;
			if(diff < 0) { m_vUpperLeft.y-=diff; m_vLowerRight.y-=diff; }

			return true;
		};

		int GetWidth() const { return m_vLowerRight.x - m_vUpperLeft.x; };
		int GetHeight() const { return m_vLowerRight.y - m_vUpperLeft.y; };

		void Repair()
		{
			if(m_vLowerRight.x < m_vUpperLeft.x)
			{
				int t=m_vLowerRight.x;
				m_vLowerRight.x=m_vUpperLeft.x;
				m_vUpperLeft.x=t;
			}

			if(m_vLowerRight.y < m_vUpperLeft.y)
			{
				int t=m_vLowerRight.y;
				m_vLowerRight.y=m_vUpperLeft.y;
				m_vUpperLeft.y=t;
			}
		};

		bool IsValid() const { return ((m_vLowerRight.x >= m_vUpperLeft.x) && (m_vLowerRight.y >= m_vUpperLeft.y)); };

		Vector2i GetCenter() const { return Vector2i((m_vUpperLeft.x + m_vLowerRight.x) / 2, (m_vUpperLeft.y + m_vLowerRight.y) / 2); };
		Vector2i GetSize() const { return Vector2i(GetWidth(),GetHeight()); };

		void AddPoint(const Vector2i& p) { AddPoint(p.x,p.y); };
		void AddPoint(const int x, const int y)
		{
			if(x > m_vLowerRight.x) m_vLowerRight.x=x;
			if(y > m_vLowerRight.y) m_vLowerRight.y=y;

			if(x < m_vUpperLeft.x) m_vUpperLeft.x=x;
			if(y < m_vUpperLeft.y) m_vUpperLeft.y=y;
		};

		Vector2i m_vUpperLeft;
		Vector2i m_vLowerRight;

};

class Rectf
{
	public:

		Rectf() : m_vUpperLeft(0.0f,0.0f), m_vLowerRight(0.0f,0.0f) {};
		Rectf(float x, float y, float x2, float y2) : m_vUpperLeft(x,y), m_vLowerRight(x2,y2) {};
		Rectf(const Vector2& vUpperLeft, const Vector2& vLowerRight) : m_vUpperLeft(vUpperLeft), m_vLowerRight(vLowerRight) {};
		Rectf(const Rectf& _Rect) : m_vUpperLeft(_Rect.m_vUpperLeft), m_vLowerRight(_Rect.m_vLowerRight) {};

		Rectf operator+(const Vector2& v) const { return Rectf(m_vUpperLeft + v, m_vLowerRight + v); };
		const Rectf& operator+=(const Vector2& v) { m_vUpperLeft+=v; m_vLowerRight+=v; return *this; };

		Rectf operator-(const Vector2& v) const { return Rectf(m_vUpperLeft-v,m_vLowerRight-v); };
		const Rectf& operator-=(const Vector2& v) { m_vUpperLeft-=v; m_vLowerRight-=v; return *this; };

		bool operator==(const Rectf& rect) const { return (m_vUpperLeft==rect.m_vUpperLeft && m_vLowerRight==rect.m_vLowerRight); };
		bool operator!=(const Rectf& rect) const { return (m_vUpperLeft!=rect.m_vUpperLeft || m_vLowerRight!=rect.m_vLowerRight); };

		bool operator < (const Rectf& rect) const { return GetArea() < rect.GetArea(); };
		bool operator <= (const Rectf& rect) const { return GetArea() <= rect.GetArea(); };

		bool operator > (const Rectf& rect) const { return GetArea() > rect.GetArea(); };
		bool operator >= (const Rectf& rect) const { return GetArea() >= rect.GetArea(); };

		float GetArea() const { return GetWidth() * GetHeight(); };

		bool IsPointInside(const Vector2& p) { return (p.x >= m_vUpperLeft.x && p.y >= m_vUpperLeft.y &&
			p.x <= m_vLowerRight.x && p.y <= m_vLowerRight.y); };

		bool IsRectCollided(const Rectf& rect) const
		{
			return (m_vLowerRight.y > rect.m_vUpperLeft.y &&
					m_vUpperLeft.y < rect.m_vLowerRight.y &&
					m_vLowerRight.x > rect.m_vUpperLeft.x &&
					m_vUpperLeft.x < rect.m_vLowerRight.x); };

		void ClipAgainst(const Rectf& rect)
		{
			if(rect.m_vLowerRight.x < m_vLowerRight.x) m_vLowerRight.x=rect.m_vLowerRight.x;
			if(rect.m_vLowerRight.y < m_vLowerRight.y) m_vLowerRight.y=rect.m_vLowerRight.y;

			if(rect.m_vUpperLeft.x > m_vUpperLeft.x) m_vUpperLeft.x=rect.m_vUpperLeft.x;
			if(rect.m_vUpperLeft.y > m_vUpperLeft.y) m_vUpperLeft.y=rect.m_vUpperLeft.y;

			if(m_vUpperLeft.y > m_vLowerRight.y) m_vUpperLeft.y=m_vLowerRight.y;
			if(m_vUpperLeft.x > m_vLowerRight.x) m_vUpperLeft.x=m_vLowerRight.x;
		};

		bool ConstrainTo(const Rectf& rect)
		{
			if(rect.GetWidth() < GetWidth() || rect.GetHeight() < GetHeight())
				return false;

			float diff=rect.m_vLowerRight.x - m_vLowerRight.x;
			if(diff < 0.0f) { m_vLowerRight.x+=diff; m_vUpperLeft.x+=diff; }

			diff=rect.m_vLowerRight.y - m_vLowerRight.y;
			if(diff < 0.0f) { m_vLowerRight.y+=diff; m_vUpperLeft.y+=diff; }

			diff=m_vUpperLeft.x - rect.m_vUpperLeft.x;
			if(diff < 0.0f) { m_vUpperLeft.x-=diff; m_vLowerRight.x-=diff; }

			diff=m_vUpperLeft.y - rect.m_vUpperLeft.y;
			if(diff < 0.0f) { m_vUpperLeft.y-=diff; m_vLowerRight.y-=diff; }

			return true;
		};

		float GetWidth() const { return m_vLowerRight.x - m_vUpperLeft.x; };
		float GetHeight() const { return m_vLowerRight.y - m_vUpperLeft.y; };

		void Repair()
		{
			if(m_vLowerRight.x < m_vUpperLeft.x)
			{
				float t=m_vLowerRight.x;
				m_vLowerRight.x=m_vUpperLeft.x;
				m_vUpperLeft.x=t;
			}

			if(m_vLowerRight.y < m_vUpperLeft.y)
			{
				float t=m_vLowerRight.y;
				m_vLowerRight.y=m_vUpperLeft.y;
				m_vUpperLeft.y=t;
			}
		};

		bool IsValid() const { return ((m_vLowerRight.x >= m_vUpperLeft.x) && (m_vLowerRight.y >= m_vUpperLeft.y)); };

		Vector2 GetCenter() const { return Vector2((m_vUpperLeft.x + m_vLowerRight.x) * 0.5f, (m_vUpperLeft.y + m_vLowerRight.y) * 0.5f); };
		Vector2 GetSize() const { return Vector2(GetWidth(),GetHeight()); };

		void AddPoint(const Vector2& p) { AddPoint(p.x,p.y); };
		void AddPoint(const float x, const float y)
		{
			if(x > m_vLowerRight.x) m_vLowerRight.x=x;
			if(y > m_vLowerRight.y) m_vLowerRight.y=y;

			if(x < m_vUpperLeft.x) m_vUpperLeft.x=x;
			if(y < m_vUpperLeft.y) m_vUpperLeft.y=y;
		};

		Vector2 m_vUpperLeft;
		Vector2 m_vLowerRight;

};

}

#endif

