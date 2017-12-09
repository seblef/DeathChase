
#ifndef _HANDLES_H_

#define _HANDLES_H_

#include "../MyAssert.h"

#define MAX_BITS_INDEX				16
#define MAX_BITS_MAGIC				16

#define MAX_INDEX					(1 << MAX_BITS_INDEX) - 1
#define MAX_MAGIC					(1 << MAX_BITS_MAGIC) - 1

namespace Core
{

template<typename TAG>
class THandle
{
	union
	{
		struct 
		{
			unsigned m_Index : MAX_BITS_INDEX;
			unsigned m_Magic : MAX_BITS_MAGIC;
		};
		
		unsigned int m_Handle;
	};
	
	public:
	
		THandle() : m_Handle(0) {}
		
		void Init(unsigned int index);
		
		unsigned int GetIndex() const { return m_Index; };
		unsigned int GetMagic() const { return m_Magic; };
		unsigned int GetHandle() const { return m_Handle; };
		bool IsNull() const { return !m_Handle; };
		
		operator unsigned int() const { return m_Handle; };
};

template <typename TAG>
void THandle<TAG>::Init(unsigned int index)
{
	MYASSERT(IsNull(),"Reinitializing an handle",false);
	MYASSERT(index <= MAX_INDEX,"Bad handle index",true);
	
	static unsigned int s_AutoMagic=0;
	
	if(++s_AutoMagic > MAX_MAGIC)
		s_AutoMagic=1;
		
	m_Index=index;
	m_Magic=s_AutoMagic;
}

template <typename TAG>
inline bool operator!=(THandle<TAG> l, THandle<TAG> r)
{
	return l.GetHandle()!=r.GetHandle();
}

template <typename TAG>
inline bool operator==(THandle<TAG> l, THandle<TAG> r)
{
	return l.GetHandle()==r.GetHandle();
}

struct tagFont{};
typedef THandle<tagFont> HFont;

struct tagMesh{};
typedef THandle<tagMesh> HMesh;

struct tagShader{};
typedef THandle<tagShader> HShader;

struct tagTexture{};
typedef THandle<tagTexture> HTexture;

struct tagSound{};
typedef THandle<tagSound> HSound;

struct tagMaterial{};
typedef THandle<tagMaterial> HMaterial;

}

#endif