
#ifndef _THANDLEMGR_H_

#define _THANDLEMGR_H_

#include "Handles.h"
#include <vector>

namespace Core
{

template<typename DATA, typename HANDLE>
class THandleMgr
{
	private:
	
		typedef std::vector<DATA> UserVec;
		typedef std::vector<unsigned int> MagicVec;
		typedef std::vector<unsigned int> FreeVec;
		
		UserVec m_UserData;
		MagicVec m_MagicNumbers;
		FreeVec m_FreeSlots;
		
	public:
	
		THandleMgr() {};
		~THandleMgr() {};
		
		DATA *Acquire(HANDLE& handle);
		void Release(HANDLE handle);
		
		DATA *Dereference(HANDLE handle);
		const DATA *Dereference(HANDLE handle) const;
		
		unsigned int GetUSedHandleCount() const { return m_MagicNumbers.size() - m_FreeSlots.size(); };
		bool HasUsedHandles() const { return (m_MagicNumbers.size() - m_FreeSlots().size())!=0; };
		
};

template <typename DATA, typename HANDLE>
DATA *THandleMgr<DATA,HANDLE>::Acquire(HANDLE& handle)
{
	unsigned int index;
	
	if(m_FreeSlots.empty())
	{
		index=m_MagicNumbers.size();
		handle.Init(index);
		m_UserData.push_back(DATA());
		m_MagicNumbers.push_back(handle.GetMagic());
	}
	else
	{
		index=m_FreeSlots.back();
		handle.Init(index);
		m_FreeSlots.pop_back();
		m_MagicNumbers[index]=handle.GetMagic();
	}
	
	return &m_UserData[index];
}

template <typename DATA,typename HANDLE>
void THandleMgr<DATA,HANDLE>::Release(HANDLE handle)
{
	unsigned int index=handle.GetIndex();
	
	assert(index < m_UserData.size());
	assert(m_MagicNumbers[index]==handle.GetMagic());
	
	m_MagicNumbers[index]=0;
	m_FreeSlots.push_back(index);
}

template <typename DATA,typename HANDLE>
inline DATA *THandleMgr<DATA,HANDLE>::Dereference(HANDLE handle)
{
	if(handle.IsNull())
		return 0;
		
	unsigned int index=handle.GetIndex();
	if(index >=m_UserData.size() ||
	   m_MagicNumbers[index]!=handle.GetMagic())
		return 0;
	
	return (DATA *)(&m_UserData[index]);
}

template <typename DATA,typename HANDLE>
inline const DATA *THandleMgr<DATA,HANDLE>::Dereference(HANDLE handle) const
{
	typedef THandleMgr<DATA,HANDLE> ThisType;
	return (const_cast<ThisType *> (this)->Dereference(handle));
}

}

#endif