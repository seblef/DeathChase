
#include "ImageFileDialog.h"
#include "../Core/TextureMgr.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IStaticText.h"
#include "IFont.h"
#include "../Core/FileSystem.h"
#include "../Core/Renderer2D.h"

namespace GUI
{

ImageFileDialog::ImageFileDialog(const char *szTitle, IEnvironment *pEnvironment,
								 IElement *pParent, IEventReceiver *pUser,uint32 dwID) :
	FileDialog(szTitle,pEnvironment,pParent,pUser,dwID)
{
	Core::Rect rel(GetRelativePosition());
	const int xdist=m_pFileBox->GetRelativePosition().m_vUpperLeft.x;
	rel.m_vLowerRight.x+=128 - m_pOK->GetAbsolutePosition().GetWidth() + xdist;
	SetRelativePosition(rel);

	AddExtFilter(".bmp");
	AddExtFilter(".jpg");
	AddExtFilter(".png");
	AddExtFilter(".tga");

	m_Type=ET_IMAGEFILEDIALOG;
}

ImageFileDialog::~ImageFileDialog()
{
	g_TextureMgr.ReleaseTexture(m_hTexture);
}

void ImageFileDialog::Clear()
{
	FileDialog::Clear();
	g_TextureMgr.ReleaseTexture(m_hTexture);
	m_hTexture=Core::HTexture();
}

void ImageFileDialog::Reset(const char *szTitle, IEnvironment *pEnvironment, IElement *pParent,
							IEventReceiver *pUser, uint32 dwID)
{
	FileDialog::Reset(szTitle,pEnvironment,pParent,pUser,dwID);
}

bool ImageFileDialog::OnEvent(const GUI::Event &_Event)
{
	switch(_Event.EventType)
	{
		case ET_GUI:
			switch(_Event.sGUI.EventType)
			{
				case GE_FOCUSLOST:
					m_bDragging=false;
					break;

				case GE_BUTTONCLICKED:
					if(_Event.sGUI.pCaller==m_pClose ||
					   _Event.sGUI.pCaller==m_pCancel)
					{
						SendCancelEvent();
						Remove();
						return true;
					}
					else if(_Event.sGUI.pCaller==m_pOK && m_strFileName!="")
					{
						g_FileSystem.ResetWorkingDirectory();
						SendSelectedEvent();
						Remove();
						return true;
					}
					break;

				case GE_LISTBOXCHANGED:
					{
						const int selected=m_pFileBox->GetSelected();
						if(m_pFileList && selected!=-1)
						{
							if(m_pFileList->IsDirectory(selected))
							{
								m_hTexture=Core::HTexture();
								m_strFileName="";
							}
							else
							{
								m_strFileName=m_pFileList->GetFileName(selected);
								LoadTexture();
							}
						}
					}
					break;

				case GE_LISTBOXSELECTEDAGAIN:
					{
						const int selected=m_pFileBox->GetSelected();
						if(m_pFileList && selected!=-1)
						{
							if(m_pFileList->IsDirectory(selected))
							{
								g_FileSystem.ChangeWorkingDirectory(m_pFileList->GetFileName(selected));
								FillListBox();
								m_strFileName="";
								m_hTexture=Core::HTexture();
							}
							else
							{
								m_strFileName=m_pFileList->GetFileName(selected);
								LoadTexture();
								return true;
							}
						}
					}
					break;

				default:
					break;
			}
			break;

		case ET_MOUSE:
			switch(_Event.sMouse.EventType)
			{
				case ME_MOUSEWHEEL:
					return m_pFileBox->OnEvent(_Event);

				case ME_LMOUSEDOWN:
					m_vDragStart.x=(int)_Event.sMouse.dwX;
					m_vDragStart.y=(int)_Event.sMouse.dwY;
					m_bDragging=true;
					m_pEnvironment->SetFocus(this);
					return true;

				case ME_LMOUSEUP:
					m_bDragging=false;
					return true;

				case ME_MOVED:
					if(m_bDragging)
					{
						if(m_pParent)
							if((int)_Event.sMouse.dwX < m_pParent->GetAbsolutePosition().m_vUpperLeft.x +1 ||
							   (int)_Event.sMouse.dwY < m_pParent->GetAbsolutePosition().m_vUpperLeft.y +1 ||
							   (int)_Event.sMouse.dwX > m_pParent->GetAbsolutePosition().m_vLowerRight.x -1 ||
							   (int)_Event.sMouse.dwY > m_pParent->GetAbsolutePosition().m_vLowerRight.y -1)
							   return true;

						Move(Core::Vector2i((int)_Event.sMouse.dwX - m_vDragStart.x,(int)_Event.sMouse.dwY - m_vDragStart.y));
						m_vDragStart.x=(int)_Event.sMouse.dwX;
						m_vDragStart.y=(int)_Event.sMouse.dwY;
						return true;
					}
					break;

				default:
					break;
			}

		default:
			break;
	}

	return m_pParent ? m_pParent->OnEvent(_Event) : false;
}

void ImageFileDialog::Draw()
{
	if(!m_bIsVisible)
		return;

	ISkin *skin=m_pEnvironment->GetSkin();
	Core::Rect rect(m_AbsoluteRect);

	rect=skin->Draw3DWindowBackground(this,true,skin->GetColor(DC_ACTIVEBORDER),rect,false,&m_AbsoluteClippingRect);

	if(m_strText.size())
	{
		rect.m_vUpperLeft.x+=2;
		rect.m_vLowerRight.x-=skin->GetSize(DS_WINDOWBUTTONWIDTH) + 5;

		IFont *font=skin->GetFont(DF_WINDOW);
		if(font)
			font->Draw(m_strText.c_str(),rect,skin->GetColor(DC_ACTIVECAPTION),false,true,&m_AbsoluteClippingRect);
	}

	IElement::Draw();

	if(!m_hTexture.IsNull())
	{
		rect.m_vUpperLeft.x=m_pOK->GetAbsolutePosition().m_vLowerRight.x -128;
		rect.m_vLowerRight.x=rect.m_vUpperLeft.x+128;
		rect.m_vLowerRight.y=m_pFileBox->GetAbsolutePosition().m_vLowerRight.y;
		rect.m_vUpperLeft.y=rect.m_vLowerRight.y-128;
		g_Renderer2D.Draw2DImage(m_hTexture,rect.m_vUpperLeft.x,rect.m_vUpperLeft.y,rect.m_vLowerRight.x,rect.m_vLowerRight.y);
	}
}

bool ImageFileDialog::LoadTexture()
{
	g_TextureMgr.ReleaseTexture(m_hTexture);
	m_hTexture=g_TextureMgr.LoadTexture(m_strFileName.c_str(),m_strPath.c_str());

	return !m_hTexture.IsNull();
}

}