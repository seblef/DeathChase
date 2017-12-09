
#ifndef _TDYNAMICTAB_H_

#define _TDYNAMICTAB_H_

namespace Core
{


template<typename T>
class TDynamicTab
{
	public:

		TDynamicTab() { m_pBuffer=0; m_iItemCount=0; m_iItemAllocated=0; m_pCurrent=0; };
		~TDynamicTab() { Release(); };

		void Release()
		{
			if(m_pBuffer)
				delete[] m_pBuffer;

			m_pBuffer=0;
			m_iItemCount=0;
			m_iItemAllocated=0;
			m_pCurrent=0;
		}
		void Clear()
		{
			m_iItemCount=0;
			m_pCurrent=m_pBuffer;
		}

		void Add(const T& val)
		{
			if(m_iItemCount==m_iItemAllocated)
				Realloc();

			*m_pCurrent=val;
			m_pCurrent++;
			m_iItemCount++;
		}
		void Add(const T *val)
		{
			if(m_iItemCount==m_iItemAllocated)
				Realloc();

			*m_pCurrent=*val;
			m_pCurrent++;
			m_iItemCount++;
		}

		void AddByMemcpy(const T *val)
		{
			if(m_iItemCount==m_iItemAllocated)
				Realloc();

			memcpy(m_pCurrent,val,sizeof(T));
			m_pCurrent++;
			m_iItemCount++;
		}

		void AddTab(const T *tab, const int iCount)
		{
			while((m_iItemCount + iCount) > m_iItemAllocated)
				Realloc();

			memcpy(m_pCurrent,tab,sizeof(T) * iCount);
			m_pCurrent+=iCount;
			m_iItemCount+=iCount;
		}

		void PopBack()
		{
			if(m_iItemCount > 0)
				m_iItemCount--;
		}

		void Initialize(const int iCount)
		{
			while(m_iItemAllocated < iCount)
				Realloc();
		}

		int GetItemCount() const { return m_iItemCount; };
		int GetItemAllocated() const { return m_iItemAllocated; };
		T *GetBuffer() { return m_pBuffer; };

		
	protected:

		void Realloc()
		{
			if(!m_pBuffer)
			{
				m_pBuffer=new T[2];
				m_iItemAllocated=2;
				m_pCurrent=m_pBuffer;
			}
			else
			{
				T *new_buf=new T[m_iItemAllocated*2];
				memcpy(new_buf,m_pBuffer,m_iItemCount * sizeof(T));
				delete[] m_pBuffer;
				m_pBuffer=new_buf;

				m_pCurrent=m_pBuffer+m_iItemCount;
				m_iItemAllocated*=2;
			}
		}

		T* m_pBuffer;
		T* m_pCurrent;
		int m_iItemCount;
		int m_iItemAllocated;

};

}


#endif
