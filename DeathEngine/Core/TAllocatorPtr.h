
#ifndef _TALLOCATORPTR_H_
#define _TALLOCATORPTR_H_


namespace Core
{

template<class T> class TMemoryAllocator;

template<typename T>
class TAllocatorPtr
{
	public:

		TAllocatorPtr() : m_iIndex(-1), m_pPtr(0) {}
		TAllocatorPtr(int iIndex, T *pPtr) : m_iIndex(iIndex), m_pPtr(pPtr) {}
		TAllocatorPtr(const TAllocatorPtr<T>& ptr) : m_iIndex(ptr.m_iIndex), m_pPtr(ptr.m_pPtr) {}
		~TAllocatorPtr() {}

		T* operator->() { return m_pPtr; }
		const T* operator->() const { return m_pPtr; }

		operator T*() { return m_pPtr; }
		operator const T*() const { return m_pPtr; }

		operator bool() { return m_pPtr!=0; }
		operator bool() const { return m_pPtr!=0; }

		template<typename Convert>
		operator Convert*() { return (Convert*)m_pPtr; }
		template<typename Convert>
		operator const Convert*() const { return (Convert*)m_pPtr; }

		const T& operator*() const { return *m_pPtr; }
		T& operator*() { return *m_pPtr; }

		bool operator!() const { return m_pPtr==0; }

		const TAllocatorPtr<T>& operator=(const TAllocatorPtr<T>& ptr)
		{
			m_pPtr=ptr.m_pPtr;
			m_iIndex=ptr.m_iIndex;
			return *this;
		}

		bool operator==(const TAllocatorPtr<T>& ptr) const { return m_pPtr==ptr.m_pPtr; }
		bool operator==(const T* ptr) const { return m_pPtr==ptr; }

		bool operator!=(const TAllocatorPtr<T>& ptr) const { return m_pPtr!=ptr.m_pPtr; }
		bool operator!=(const T* ptr) const { return m_pPtr!=ptr; }

		bool operator<(const TAllocatorPtr<T>& ptr) const { return m_pPtr.m_iIndex < ptr.m_iIndex; }
		bool operator<=(const TAllocatorPtr<T>& ptr) const { return m_pPtr.m_iIndex <= ptr.m_iIndex; }

		bool operator>(const TAllocatorPtr<T>& ptr) const { return m_pPtr.m_iIndex > ptr.m_iIndex; }
		bool operator>=(const TAllocatorPtr<T>& ptr) const { return m_pPtr.m_iIndex >= ptr.m_iIndex; }

		bool IsValid() const { return (m_pPtr!=0) && (m_iIndex>=0); }

	protected:

		friend class TMemoryAllocator<T>;

		void SetIndex(const int iIndex) { m_iIndex=iIndex; }
		int GetIndex() const { return m_iIndex; }

		void SetPtr(T* pPtr) { m_pPtr=pPtr; }
		T* GetPtr() const { return m_pPtr; }

	private:

		int m_iIndex;
		T *m_pPtr;

};

}

#endif