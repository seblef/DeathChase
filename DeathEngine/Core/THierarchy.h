
#ifndef _THIERARCHY_H_

#define _THIERARCHY_H_

#include "Vector.h"
#include "../MyAssert.h"
#include <vector>

namespace Core
{

template<typename T>
class THierarchyItem
{
	public:

		THierarchyItem(const Matrix4& mOriginal, T *pObject) :
		  m_pParent(0),m_vRotation(Vector3::NullVector),m_mOriginal(mOriginal),m_pObject(pObject)
		  { ASSERTRETURN(mOriginal.Inverse(m_mInvOriginal),true,"Bad object hierarchy matrix"); };
		~THierarchyItem() { m_Children.clear(); };

		T* GetItem() { return m_pObject; };
		const Matrix4& GetOriginalMatrix() const { return m_mOriginal; };
		const Matrix4& GetInvOriginalMatrix() const { return m_mInvOriginal; };
		const Matrix4& GetFinalMatrix() const { return m_mFinal; };

		void AddChild(THierarchyItem<T> *pObject) { m_Children.push_back(pObject); };
		void RemoveChild(int iIndex);
		void RemoveChild(THierarchyItem<T> *pObject);
		THierarchyItem<T> *GetChild(const int iIndex);
		int GetChildrenCount() const { return m_Children.size(); };

		void SetParent(THierarchyItem<T> *pObject) { m_pParent=pObject; };
		THierarchyItem<T> *GetParent() { return m_pParent; };

		void SetPosition(const Vector3& vPos);
		void SetRotation(const Vector3& vRot) { m_vRotation=vRot; };
		void Rotate(const Vector3& vRot) { m_vRotation+=vRot; };
		const Vector3& GetRotation() const { return m_vRotation; };

		void RebuildMatrix();
		THierarchyItem<T> *Copy(std::vector<THierarchyItem<T> *> *pCollection);

	protected:

		Matrix4 m_mOriginal;
		Matrix4 m_mInvOriginal;
		Matrix4 m_mFinal;
		Vector3 m_vRotation;

		std::vector<THierarchyItem<T> *> m_Children;
		THierarchyItem *m_pParent;

		T *m_pObject;

};

template<typename T>
class THierarchy
{
	public:

		THierarchy() {};
		~THierarchy() { Release(); };

		int AddObject(T *pObject, const Matrix4& mOriginal,
					const char *szName, const int iParentIndex);
		int AddObject(T *pObject, const Matrix4& mOriginal,
					const char *szName, const char *szParentName);

		THierarchyItem<T> *GetItem(const int iIndex);
		int GetItemCount() const { return m_Items.size(); };

		THierarchyItem<T> *GetRoot(const int iIndex);
		int GetRootCount() const { return m_Roots.size(); };

		void RebuildMatrices();
		void SetPosition(const Vector3& vPos);

		void Release();
		THierarchy<T> *Copy();

	protected:

		std::vector<THierarchyItem<T> *> m_Items;
		std::vector<THierarchyItem<T> *> m_Roots;
		std::vector<VectorString> m_Names;

};

template<typename T>
void THierarchyItem<T>::RemoveChild(int iIndex)
{
	MYASSERT(iIndex >= 0 && iIndex < m_Children.size(),"Bad child index",true);
	m_Children.erase(m_Children.begin() + iIndex);
}

template<typename T>
void THierarchyItem<T>::RemoveChild(THierarchyItem<T> *pObject)
{
	std::vector<THierarchy<T> *>::iterator i;
	for(i=m_Children.begin();i!=m_Children.end();i++)
		if((*i)==pObject)
		{
			m_Children.erase(i);
			return;
		}
}

template<typename T>
THierarchyItem<T> *THierarchyItem<T>::GetChild(const int iIndex)
{
	MYASSERT(iIndex >= 0 && iIndex < m_Children.size(),"Bad child index",true);
	return m_Children[iIndex];
}

template<typename T>
void THierarchyItem<T>::RebuildMatrix()
{
	Matrix4 mRot;
	mRot.CreateRotateXYZ((int)m_vRotation.x,(int)m_vRotation.y,(int)m_vRotation.z);
	m_mFinal=mRot * m_mOriginal;

	if(m_pParent)
		m_mFinal=m_mFinal * m_pParent->GetInvOriginalMatrix() * m_pParent->GetFinalMatrix();

	std::vector<THierarchyItem<T> *>::iterator i;
	for(i=m_Children.begin();i!=m_Children.end();i++)
		(*i)->RebuildMatrix();
}

template<typename T>
void THierarchyItem<T>::SetPosition(const Vector3& vPos)
{
	m_mOriginal.m[3][0]+=vPos.x;
	m_mOriginal.m[3][1]+=vPos.y;
	m_mOriginal.m[3][2]+=vPos.z;
	m_mOriginal.Inverse(m_mInvOriginal);
}

template<typename T>
THierarchyItem<T> *THierarchyItem<T>::Copy(std::vector<THierarchyItem<T> *> *pCollection)
{
	THierarchyItem<T> *pItem=new THierarchyItem<T>(m_mOriginal,m_pObject);

	std::vector<THierarchyItem<T> *>::iterator i;
	for(i=m_Children.begin();i!=m_Children.end();i++)
	{
		THierarchyItem<T> *pChild=(*i)->Copy(pCollection);
		pChild->SetParent(pItem);
		pItem->AddChild(pChild);
	}

	pCollection->push_back(pItem);
	return pItem;
}


template<typename T>
int THierarchy<T>::AddObject(T *pObject, const Matrix4& mOriginal,
							 const char *szName, const int iParentIndex)
{
	THierarchyItem<T> *pItem=new THierarchyItem<T>(mOriginal,pObject);

	if(!pItem)
		return -1;

	bool bHasParent=false;

	if(iParentIndex!=-1)
	{
		MYASSERT(iParentIndex >= 0 && iParentIndex < m_Items.size(),"Bad parent index",true);
		pItem->SetParent(m_Items[iParentIndex]);
		m_Items[iParentIndex]->AddChild(pItem);
		bHasParent=true;
	}

	m_Items.push_back(pItem);
	VectorString vsName;
	strcpy(vsName.szText,szName);
	m_Names.push_back(vsName);

	if(!bHasParent)
		m_Roots.push_back(pItem);

	return m_Items.size()-1;
}

template<typename T>
int THierarchy<T>::AddObject(T *pObject, const Matrix4& mOriginal,
							const char *szName, const char *szParentName)
{
	THierarchyItem<T> *pItem=new THierarchyItem<T>(mOriginal,pObject);

	if(!pItem)
		return -1;

	bool bHasParent=false;

	if(szParentName)
	{
		std::vector<VectorString>::iterator i;
		int iIndex=0;
		for(i=m_Names.begin();i!=m_Names.end();i++,iIndex++)
		{
			if(strcmp(i->szText,szParentName)==0)
			{
				pItem->SetParent(m_Items[iIndex]);
				m_Items[iIndex]->AddChild(pItem);
				bHasParent=true;
				break;
			}
		}
	}

	m_Items.push_back(pItem);
	VectorString vsName;
	strcpy(vsName.szText,szName);
	m_Names.push_back(vsName);

	if(!bHasParent)
		m_Roots.push_back(pItem);

	return m_Items.size()-1;
}

template<typename T>
THierarchyItem<T> *THierarchy<T>::GetItem(const int iIndex)
{
	MYASSERT(iIndex >= 0 && iIndex < m_Items.size(),"Bad object index",true);
	return m_Items[iIndex];
}

template<typename T>
void THierarchy<T>::RebuildMatrices()
{
	std::vector<THierarchyItem<T> *>::iterator i;
	for(i=m_Roots.begin();i!=m_Roots.end();i++)
		(*i)->RebuildMatrix();
}

template<typename T>
void THierarchy<T>::Release()
{
	m_Names.clear();

	std::vector<THierarchyItem<T> *>::iterator i;
	for(i=m_Items.begin();i!=m_Items.end();i++)
		delete *i;

	m_Items.clear();
	m_Roots.clear();
}

template<typename T>
THierarchyItem<T> *THierarchy<T>::GetRoot(const int iIndex)
{
	MYASSERT(iIndex >= 0 && iIndex < m_Roots.size(),"Bad root index",true);
	return m_Roots[iIndex];
}

template<typename T>
void THierarchy<T>::SetPosition(const Vector3& vPos)
{
	std::vector<THierarchyItem<T> *>::iterator i;
	for(i=m_Items.begin();i!=m_Items.end();i++)
		(*i)->SetPosition(vPos);
}

template<typename T>
THierarchy<T> *THierarchy<T>::Copy()
{
	THierarchy<T> *pRet=new THierarchy<T>;

	if(!pRet)
		return 0;

	std::vector<VectorString>::iterator j;
	for(j=m_Names.begin();j!=m_Names.end();j++)
		pRet->m_Names.push_back(*j);

	std::vector<THierarchyItem<T> *>::iterator i;
	for(i=m_Roots.begin();i!=m_Roots.end();i++)
		pRet->m_Roots.push_back((*i)->Copy(&pRet->m_Items));

	return pRet;
}

}

#endif