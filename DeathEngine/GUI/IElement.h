
#ifndef _IELEMENT_H_

#define _IELEMENT_H_

#include "IEventReceiver.h"
#include "ElementTypes.h"
#include "../Core/Rect.h"
#include "../Core/IReferenceCounter.h"
#include "../Core/TextOutput.h"
#include <list>

namespace GUI
{

class IEnvironment;

enum ALIGNMENT
{
	AL_UPPERLEFT=0,
	AL_LOWERRIGHT,
	AL_CENTER,
	AL_SCALE

};

const char * const AlignmentName[] = { "upperleft", "lowerright", "center", "scale", 0 };

class IElement : public IEventReceiver
{
	public:

		IElement(ELEMENTTYPE _Type,
			IEnvironment *pEnvironment,
			IElement *pParent,
			IEventReceiver *pUserReceiver,
			const uint32 dwID,
			Core::Rect rect) : m_pParent(0), m_RelativeRect(rect), m_AbsoluteRect(rect),
							m_AbsoluteClippingRect(rect),m_DesiredRect(rect),
							m_vMaxSize(0,0), m_vMinSize(rect.GetWidth(),rect.GetHeight()), m_bIsVisible(true), m_bIsEnable(true),
							m_bIsSubElement(false), m_bNoClip(false), m_dwID(dwID),
							m_bIsTabStop(false), m_iTabOrder(-1), m_bIsTabGroup(false),
							m_AlignLeft(AL_UPPERLEFT), m_AlignRight(AL_UPPERLEFT),
							m_AlignTop(AL_UPPERLEFT), m_AlignBottom(AL_UPPERLEFT),
							m_pEnvironment(pEnvironment), m_Type(_Type), m_pUserReceiver(pUserReceiver)
		{
			if(pParent)
				((IElement *)pParent)->AddChild(this);

			if(pParent)
			{
				m_LastParentRect=m_pParent->GetAbsolutePosition();
				m_AbsoluteRect+=m_LastParentRect.m_vUpperLeft;
				m_AbsoluteClippingRect=m_AbsoluteRect;
				m_AbsoluteClippingRect.ClipAgainst(m_pParent->m_AbsoluteClippingRect);
			}
		};

		virtual ~IElement();

		virtual void Reset(IEnvironment *pEnvironment, IElement *pParent, IEventReceiver *pUser,
			uint32 dwID, Core::Rect rect)
		{
			m_pEnvironment=pEnvironment;
			m_pUserReceiver=pUser;

			m_vMinSize.x=rect.GetWidth();
			m_vMinSize.y=rect.GetHeight();
			m_vMaxSize.x=0;
			m_vMaxSize.y=0;
			m_RelativeRect=rect;
			m_AbsoluteRect=rect;
			m_DesiredRect=rect;
			m_dwID=dwID;

			if(pParent)
				pParent->AddChild(this);

			if(pParent)
			{
				m_LastParentRect=m_pParent->GetAbsolutePosition();
				m_AbsoluteRect+=m_LastParentRect.m_vUpperLeft;
				m_AbsoluteClippingRect=m_AbsoluteRect;
				m_AbsoluteClippingRect.ClipAgainst(m_pParent->m_AbsoluteClippingRect);
			}
		}

		virtual void Clear();

		IElement *GetParent() const { return m_pParent; };

		Core::Rect GetRelativePosition() const { return m_RelativeRect; };
		void SetRelativePosition(const Core::Rect& rect)
		{
			if(m_pParent)
			{
				const Core::Rect r2(m_pParent->GetAbsolutePosition());

				Core::Vector2 d((float)(r2.GetSize().x),(float)(r2.GetSize().y));

				if(m_AlignLeft==AL_SCALE)   m_ScaleRect.m_vUpperLeft.x=(float)rect.m_vUpperLeft.x / d.x;
				if(m_AlignRight==AL_SCALE)  m_ScaleRect.m_vLowerRight.x=(float)rect.m_vLowerRight.x / d.x;
				if(m_AlignTop==AL_SCALE)    m_ScaleRect.m_vUpperLeft.y=(float)rect.m_vUpperLeft.y / d.y;
				if(m_AlignBottom==AL_SCALE) m_ScaleRect.m_vLowerRight.y=(float)rect.m_vLowerRight.y / d.y;
			}

			m_DesiredRect=rect;
			UpdateAbsolutePosition();
		};

		void SetRelativePosition(const Core::Rectf& rect)
		{
			if(!m_pParent) return;

			const Core::Vector2i& d=m_pParent->GetAbsolutePosition().GetSize();

			m_DesiredRect=Core::Rect((int)((float)d.x * rect.m_vUpperLeft.x),
									 (int)((float)d.y * rect.m_vUpperLeft.y),
									 (int)((float)d.x * rect.m_vLowerRight.x),
									 (int)((float)d.y * rect.m_vLowerRight.y));
			m_ScaleRect=rect;
			UpdateAbsolutePosition();
		};


		Core::Rect GetAbsolutePosition() const { return m_AbsoluteRect; };
		Core::Rect GetAbsoluteClippingRect() const { return m_AbsoluteClippingRect; };

		void SetNoClip(const bool bNoClip) { m_bNoClip=bNoClip; };
		bool IsNoClip() const { return m_bNoClip; };

		void SetMaxSize(const Core::Vector2i& vSize) { m_vMaxSize=vSize; UpdateAbsolutePosition(); };
		void SetMinSize(const Core::Vector2i& vSize)
		{
			m_vMinSize=vSize;
			m_vMinSize.x=MAX(m_vMinSize.x,1);
			m_vMinSize.y=MAX(m_vMinSize.y,1);
			UpdateAbsolutePosition();
		};

		void SetAlignment(const ALIGNMENT left, const ALIGNMENT right,
						  const ALIGNMENT top, const ALIGNMENT bottom)
		{
			m_AlignLeft=left; m_AlignRight=right;
			m_AlignTop=top; m_AlignBottom=bottom;

			if(m_pParent)
			{
				Core::Rect r(m_pParent->GetAbsolutePosition());
				Core::Vector2 d((float)r.GetSize().x, (float)r.GetSize().y);

				if(m_AlignLeft==AL_SCALE)   m_ScaleRect.m_vUpperLeft.x=(float)m_DesiredRect.m_vUpperLeft.x / d.x;
				if(m_AlignRight==AL_SCALE)  m_ScaleRect.m_vLowerRight.x=(float)m_DesiredRect.m_vLowerRight.x / d.x;
				if(m_AlignTop==AL_SCALE)    m_ScaleRect.m_vUpperLeft.y=(float)m_DesiredRect.m_vUpperLeft.y / d.y;
				if(m_AlignBottom==AL_SCALE) m_ScaleRect.m_vLowerRight.y=(float)m_DesiredRect.m_vLowerRight.y / d.y;
			}
		};

		virtual void UpdateAbsolutePosition()
		{
			Core::Rect pAbsolute(0,0,0,0);
			Core::Rect pAbsClip;
			int diffx, diffy;
			float fw=0.0f, fh=0.0f;

			if(m_pParent)
			{
				pAbsolute=m_pParent->m_AbsoluteRect;

				if(m_bNoClip)
				{
					IElement *p=this;
					while(p && p->m_bNoClip && p->m_pParent)
						p=p->m_pParent;

					if(p->m_pParent)
						pAbsClip=p->m_pParent->m_AbsoluteClippingRect;
					else
						pAbsClip=p->m_AbsoluteClippingRect;
				}
				else
					pAbsClip=m_pParent->m_AbsoluteClippingRect;
			}

			diffx=pAbsolute.GetWidth() - m_LastParentRect.GetWidth();
			diffy=pAbsolute.GetHeight() - m_LastParentRect.GetHeight();

			if(m_AlignLeft==AL_SCALE || m_AlignRight==AL_SCALE) fw=(float)pAbsolute.GetWidth();
			if(m_AlignTop==AL_SCALE || m_AlignBottom==AL_SCALE) fh=(float)pAbsolute.GetHeight();

			switch(m_AlignLeft)
			{
				case AL_UPPERLEFT:  break;
				case AL_LOWERRIGHT: m_DesiredRect.m_vUpperLeft.x+=diffx; break;
				case AL_CENTER:     m_DesiredRect.m_vUpperLeft.x+=diffx / 2; break;
				case AL_SCALE:      m_DesiredRect.m_vUpperLeft.x=(int)(m_ScaleRect.m_vUpperLeft.x * fw); break;
			}

			switch(m_AlignRight)
			{
				case AL_UPPERLEFT:  break;
				case AL_LOWERRIGHT: m_DesiredRect.m_vLowerRight.x+=diffx; break;
				case AL_CENTER:     m_DesiredRect.m_vLowerRight.x+=diffx / 2; break;
				case AL_SCALE:		m_DesiredRect.m_vLowerRight.x=(int)(m_ScaleRect.m_vLowerRight.x * fw); break;
			}

			switch(m_AlignTop)
			{
				case AL_UPPERLEFT:  break;
				case AL_LOWERRIGHT: m_DesiredRect.m_vUpperLeft.y+=diffy; break;
				case AL_CENTER:     m_DesiredRect.m_vUpperLeft.y+=diffy / 2; break;
				case AL_SCALE:		m_DesiredRect.m_vUpperLeft.y=(int)(m_ScaleRect.m_vUpperLeft.y * fh); break;
			}

			switch(m_AlignBottom)
			{
				case AL_UPPERLEFT:  break;
				case AL_LOWERRIGHT: m_DesiredRect.m_vLowerRight.y+=diffy; break;
				case AL_CENTER:		m_DesiredRect.m_vLowerRight.y+=diffy / 2; break;
				case AL_SCALE:		m_DesiredRect.m_vLowerRight.y=(int)(m_ScaleRect.m_vLowerRight.y * fh); break;
			}

			m_RelativeRect=m_DesiredRect;
			int w=m_RelativeRect.GetWidth();
			int h=m_RelativeRect.GetHeight();

			if(w < m_vMinSize.x) m_RelativeRect.m_vLowerRight.x=m_RelativeRect.m_vUpperLeft.x + m_vMinSize.x;
			if(h < m_vMinSize.y) m_RelativeRect.m_vLowerRight.y=m_RelativeRect.m_vUpperLeft.y + m_vMinSize.y;
			if(m_vMaxSize.x && w > m_vMaxSize.x) m_RelativeRect.m_vLowerRight.x=m_RelativeRect.m_vUpperLeft.x + m_vMaxSize.x;
			if(m_vMaxSize.y && w > m_vMaxSize.y) m_RelativeRect.m_vLowerRight.y=m_RelativeRect.m_vUpperLeft.y + m_vMaxSize.y;

			m_RelativeRect.Repair();
			m_AbsoluteRect=m_RelativeRect + pAbsolute.m_vUpperLeft;

			if(!m_pParent) pAbsClip=m_AbsoluteRect;

			m_AbsoluteClippingRect=m_AbsoluteRect;
			m_AbsoluteClippingRect.ClipAgainst(pAbsClip);

			m_LastParentRect=pAbsolute;

			std::list<IElement *>::iterator i;
			for(i=m_Children.begin(); i!=m_Children.end(); i++)
				(*i)->UpdateAbsolutePosition();
		};

		IElement *GetElementFromPoint(const Core::Vector2i& p)
		{
			IElement *pTarget=0;
			std::list<IElement *>::reverse_iterator i=m_Children.rbegin();

			if(m_bIsVisible)
				while(i!=m_Children.rend())
				{
					pTarget=(*i)->GetElementFromPoint(p);
					if(pTarget)
						return pTarget;

					i++;
				}

			if(m_bIsVisible && IsPointInside(p))
				pTarget=this;

			return pTarget;
		};

		virtual bool IsPointInside(const Core::Vector2i& p) const { return m_AbsoluteClippingRect.IsPointInside(p); };

		virtual void AddChild(IElement *pChild)
		{
			if(pChild)
			{
				pChild->Remove();
				pChild->m_LastParentRect=GetAbsolutePosition();
				pChild->m_pParent=this;
				m_Children.push_back(pChild);
			}
		};
		virtual void RemoveChild(IElement *pChild, bool bDelete=false);
		virtual void Remove() { if(m_pParent) m_pParent->RemoveChild(this); };
		
		virtual void Draw()
		{
			if(m_bIsVisible)
			{
				std::list<IElement *>::iterator i;
				for(i=m_Children.begin();i!=m_Children.end();i++)
					(*i)->Draw();
			}
		};

		virtual void OnPostRender(uint32 dwTime)
		{
			if(m_bIsVisible)
			{
				std::list<IElement *>::iterator i;
				for(i=m_Children.begin();i!=m_Children.end();i++)
					(*i)->OnPostRender(dwTime);
			}
		};

		virtual void Move(const Core::Vector2i& vMove) { SetRelativePosition(m_DesiredRect+vMove); };
		
		virtual bool IsVisible() const { return m_bIsVisible; };
		virtual void SetVisible(const bool bVisible) { m_bIsVisible=bVisible; };

		virtual bool IsSubElement() const { return m_bIsSubElement; };
		virtual void SetSubElement(const bool bSub) { m_bIsSubElement=bSub; };

		virtual bool IsTabStop() const { return m_bIsTabStop; };
		virtual void SetTabStop(const bool bTab) { m_bIsTabStop=bTab; };

		void SetTabOrder(int index)
		{
			if(index < 0)
			{
				m_iTabOrder=0;
				IElement *el=GetTabGroup();
				while(m_bIsTabGroup && el && el->m_pParent)
					el=el->m_pParent;

				IElement *pFirst=0,*pClosest=0;
				if(el)
				{
					el->GetNextElement(-1,true,m_bIsTabGroup,pFirst,pClosest,true);
					if(pFirst)
						m_iTabOrder=pFirst->GetTabOrder() + 1;
				}
			}
			else
				m_iTabOrder=index;
		};

		int GetTabOrder() const { return m_iTabOrder; };

		bool IsTabGroup() const { return m_bIsTabGroup; };
		void SetTabGroup(const bool bGroup) { m_bIsTabGroup=bGroup; };

		IElement *GetTabGroup()
		{
			IElement *pRet=this;
			while(pRet && !pRet->IsTabGroup())
				pRet=pRet->GetParent();

			return pRet;
		};

		virtual bool IsEnable() const { return m_bIsEnable; };
		virtual void SetEnable(const bool bEnable) { m_bIsEnable=bEnable; };

		virtual const char *GetText() const { return m_strText.c_str(); };
		virtual void SetText(const char *szText) { m_strText=szText; };

		virtual const char *GetToolTipText() const { return m_strToolTipText.c_str(); };
		virtual void SetToolTipText(const char *szText) { m_strToolTipText=szText; };
		virtual bool IsTextEmpty() const { return m_strText.empty(); };

		virtual uint32 GetID() const { return m_dwID; };
		virtual void SetID(const uint32 dwID) { m_dwID=dwID; };

		virtual bool OnEvent(const Event& _Event) { return m_pParent ? m_pParent->OnEvent(_Event) : false; };

		virtual bool BringToFront(IElement *pElement)
		{
			std::list<IElement *>::iterator i;
			for(i=m_Children.begin();i!=m_Children.end();i++)
			{
				if(pElement==(*i))
				{
					m_Children.erase(i);
					m_Children.push_back(pElement);
					return true;
				}
			}

			return false;

		};

		virtual const std::list<IElement *>& GetChildren() const { return m_Children; };

		virtual IElement *GetElementFromID(const uint32 dwID, bool bSearchChildren=false) const
		{
			IElement *e=0;
			std::list<IElement *>::const_iterator i;
			for(i=m_Children.begin();i!=m_Children.end();i++)
			{
				if((*i)->GetID()==dwID)
					return *i;

				if(bSearchChildren)
					e=(*i)->GetElementFromID(dwID,true);

				if(e)
					return e;
			}

			return e;
		};

		bool IsMyChild(IElement *pChild) const
		{
			if(!pChild)
				return false;

			do
			{
				if(pChild->m_pParent)
					pChild=pChild->m_pParent;
			} while(pChild->m_pParent && pChild!=this);

			return pChild==this;
		};

		bool GetNextElement(int iStartOrder, bool bReverse, bool bGroup,
			IElement*& pFirst, IElement*& pClosest, bool bIncludeInvisible=false) const
		{
			int wanted=iStartOrder + (bReverse ? -1 : 1);
			if(wanted==-2)
				wanted=1073741824;

			int iClosestOrder,iCurrentOrder;
			std::list<IElement *>::const_iterator i=m_Children.begin();

			while(i!=m_Children.end())
			{
				if(((*i)->IsVisible() || bIncludeInvisible) &&
					(bGroup || (*i)->IsTabGroup()==false))
				{
					if((*i)->IsTabStop() && ((*i)->IsTabGroup()==bGroup))
					{
						iCurrentOrder=(*i)->GetTabOrder();
						if(iCurrentOrder==wanted)
						{
							pClosest=*i;
							return true;
						}

						if(pClosest)
						{
							iClosestOrder=pClosest->GetTabOrder();
							if((bReverse && iCurrentOrder > iClosestOrder && iCurrentOrder < iStartOrder) ||
								(!bReverse && iCurrentOrder < iClosestOrder && iCurrentOrder > iStartOrder))
								pClosest=*i;
						}
						else if((bReverse && iCurrentOrder < iStartOrder) || (!bReverse && iCurrentOrder > iStartOrder))
							pClosest=*i;

						if(pFirst)
						{
							iClosestOrder=pFirst->GetTabOrder();
							if((bReverse && iClosestOrder < iCurrentOrder) || (!bReverse && iClosestOrder > iCurrentOrder))
								pFirst=*i;
						}
						else
							pFirst=*i;
					}

					if((*i)->GetNextElement(iStartOrder,bReverse,bGroup,pFirst,pClosest))
						return true;
				}

				i++;
			}

			return false;
		};

		ELEMENTTYPE GetType() const { return m_Type; };
		const char *GetTypeName() const { return ElementTypeNames[m_Type]; };

		virtual void SetUserEventReceiver(IEventReceiver *pRcv) { m_pUserReceiver=pRcv; };
		virtual void SendEvent(const Event& e)
		{
			if(m_pParent) m_pParent->OnEvent(e);
			if(m_pUserReceiver) m_pUserReceiver->OnEvent(e);
		}

	protected:

		std::list<IElement *> m_Children;
		IElement *m_pParent;

		Core::Rect m_RelativeRect;
		Core::Rect m_AbsoluteRect;
		Core::Rect m_AbsoluteClippingRect;
		Core::Rect m_DesiredRect;
		Core::Rect m_LastParentRect;
		Core::Rectf m_ScaleRect;

		Core::Vector2i m_vMinSize,m_vMaxSize;

		bool m_bIsVisible,m_bIsEnable;
		bool m_bIsSubElement;
		bool m_bNoClip;

		std::string m_strText;
		std::string m_strToolTipText;

		uint32 m_dwID;

		bool m_bIsTabStop;
		bool m_bIsTabGroup;
		int m_iTabOrder;

		ALIGNMENT m_AlignLeft,m_AlignRight;
		ALIGNMENT m_AlignTop,m_AlignBottom;

		IEventReceiver *m_pUserReceiver;
		IEnvironment *m_pEnvironment;
		ELEMENTTYPE m_Type;
};

}

#endif