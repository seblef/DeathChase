
#ifndef _IRESOURCE_H_
#define _IRESOURCE_H_

namespace Core
{

class IResource
{
	public:

		IResource() : m_iRefCount(1) {}
		~IResource() {}

		int AddRef() { return ++m_iRefCount; }
		int RemRef() { return --m_iRefCount; }
		int GetRefCount() const { return m_iRefCount; }

	private:

		int m_iRefCount;
};

}

#endif
