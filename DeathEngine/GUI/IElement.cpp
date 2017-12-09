
#include "IElement.h"
#include "IEnvironment.h"

namespace GUI
{

IElement::~IElement()
{
	std::list<IElement *>::iterator i;
	for(i=m_Children.begin();i!=m_Children.end();i++)
	{
		(*i)->m_pParent=0;
		m_pEnvironment->ReleaseElement(*i);
	}

	m_Children.clear();
}

void IElement::Clear()
{
	std::list<IElement *>::iterator i;
	for(i=m_Children.begin();i!=m_Children.end();i++)
	{
		(*i)->m_pParent=0;
		m_pEnvironment->ReleaseElement(*i);
	}

	m_Children.clear();

	m_pParent=0;
	m_pUserReceiver=0;
	m_vMaxSize=Core::Vector2i::NullVector;
	m_vMaxSize=Core::Vector2i(1,1);
	m_bIsVisible=true;
	m_bIsEnable=true;
	m_bIsSubElement=false;
	m_bNoClip=false;
	m_bIsTabStop=false;
	m_iTabOrder=-1;
	m_bIsTabGroup=false;
	m_AlignLeft=m_AlignRight=AL_UPPERLEFT;
	m_AlignTop=m_AlignBottom=AL_UPPERLEFT;
	m_dwID=INVALID_ID;
	m_strText="";
}

void IElement::RemoveChild(GUI::IElement *pChild, bool bDelete)
{
	std::list<IElement *>::iterator i;
	for(i=m_Children.begin();i!=m_Children.end();i++)
	if((*i)==pChild)
	{
		(*i)->m_pParent=0;

		if(bDelete)
			m_pEnvironment->ReleaseElement(*i);
	
		m_Children.erase(i);
		return;
	}
}

}