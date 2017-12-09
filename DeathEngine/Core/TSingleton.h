
#ifndef _TSINGLETON_H_

#define _TSINGLETON_H_

#include "../MyAssert.h"

namespace Core
{

#pragma warning(disable:4311)
#pragma warning(disable:4312)

template<typename T> class TSingleton
{
	static T* m_spSingleton;
	
	public:
	
		TSingleton()
		{
			MYASSERT(!m_spSingleton,"Singleton already instanced!",false);
			int offset=(int)(T*)1 - (int)(TSingleton <T>*)(T*)1;
			m_spSingleton=(T*)((int)(this + offset));
		}
		
		~TSingleton()
		{
			assert(m_spSingleton);
			m_spSingleton=0;
		}
		
		static T& GetSingletonRef()
		{
			MYASSERT(m_spSingleton,"Singleton not instanced!",true);
			return *m_spSingleton;
		}
		
		static T* GetSingletonPtr()
		{
			MYASSERT(m_spSingleton,"Singleton not instanced!",true);
			return m_spSingleton;
		}
};

template<typename T> T* TSingleton<T>::m_spSingleton=0;

#pragma warning(default:4311)
#pragma warning(default:4312)

}

#endif
