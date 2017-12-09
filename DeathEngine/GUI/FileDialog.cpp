
#include "FileDialog.h"
#include "ISkin.h"
#include "IEnvironment.h"
#include "IStaticText.h"
#include "IFont.h"
#include "../Core/FileSystem.h"

namespace GUI
{

const int FOD_WIDTH=350;
const int FOD_HEIGHT=250;

FileDialog::FileDialog(const char *szTitle, IEnvironment *pEnvironment,
					   IElement *pParent, IEventReceiver *pUser,uint32 dwID) :
IFileDialog(pEnvironment,pParent,pUser,dwID,Core::Rect((pParent->GetAbsolutePosition().GetWidth() - FOD_WIDTH) /2,
												 (pParent->GetAbsolutePosition().GetHeight() - FOD_HEIGHT) /2,
												 (pParent->GetAbsolutePosition().GetWidth() - FOD_WIDTH) /2 + FOD_WIDTH,
												 (pParent->GetAbsolutePosition().GetHeight() - FOD_HEIGHT) /2 + FOD_HEIGHT)),
  m_bDragging(false),m_pFileNameText(0),m_pFileList(0),m_bNoDirs(false)
{
	m_strText=szTitle;

	ISkin *skin=m_pEnvironment->GetSkin();
	ISpriteBank *sprites=0;
	Core::Color32 color(255,255,255,255);
	if(skin)
	{
		sprites=skin->GetSpriteBank();
		color=skin->GetColor(DC_WINDOWSYMBOL);
	}

	int buttonw=skin->GetSize(DS_WINDOWBUTTONWIDTH);
	int posx=m_RelativeRect.GetWidth() - buttonw - 4;

	m_pClose=m_pEnvironment->AddButton(Core::Rect(posx,3,posx + buttonw,3 + buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWCLOSE) : "Fermer");
	m_pClose->SetSubElement(true);
	m_pClose->SetTabStop(false);
	if (sprites)
	{
		m_pClose->SetSpriteBank(sprites);
		m_pClose->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWCLOSE),color);
		m_pClose->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWCLOSE),color);
	}
	m_pClose->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pOK=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth()-80,30,m_RelativeRect.GetWidth()-10,50),
		this,0,INVALID_ID,skin ? skin->GetDefaultText(DT_MSGBOXOK) : "OK");
	m_pOK->SetSubElement(true);
	m_pOK->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pCancel=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth()-80,55,m_RelativeRect.GetWidth()-10,75),
		this,0,INVALID_ID, skin ? skin->GetDefaultText(DT_MSGBOXCANCEL) : "Annuler");
	m_pCancel->SetSubElement(true);
	m_pCancel->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pFileBox=m_pEnvironment->AddListBox(Core::Rect(10,55,m_RelativeRect.GetWidth()-90,230),
		this,0,INVALID_ID,true);
	m_pFileBox->SetSubElement(true);
	m_pFileBox->SetAlignment(AL_UPPERLEFT,AL_UPPERLEFT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pFileNameText=m_pEnvironment->AddStaticText("",Core::Rect(10,30,m_RelativeRect.GetWidth()-90,50),
		true,false,this,0);
	m_pFileNameText->SetSubElement(true);
	m_pFileNameText->SetAlignment(AL_UPPERLEFT,AL_UPPERLEFT,AL_UPPERLEFT,AL_UPPERLEFT);

	UpdateAbsolutePosition();

	SetTabGroup(true);
	FillListBox();
}

FileDialog::~FileDialog()
{
	RemoveChild(m_pClose,true);
	RemoveChild(m_pOK,true);
	RemoveChild(m_pCancel,true);
	RemoveChild(m_pFileBox,true);
	RemoveChild(m_pFileNameText,true);
	if(m_pFileList) delete m_pFileList;
}

void FileDialog::Reset(const char *szTitle, IEnvironment *pEnvironment,
					   IElement *pParent, IEventReceiver *pUser, uint32 dwID)
{
	IElement::Reset(pEnvironment,pParent,pUser, dwID,Core::Rect((pParent->GetAbsolutePosition().GetWidth() - FOD_WIDTH) /2,
												 (pParent->GetAbsolutePosition().GetHeight() - FOD_HEIGHT) /2,
												 (pParent->GetAbsolutePosition().GetWidth() - FOD_WIDTH) /2 + FOD_WIDTH,
												 (pParent->GetAbsolutePosition().GetHeight() - FOD_HEIGHT) /2 + FOD_HEIGHT));
	
	m_strText=szTitle;

	ISkin *skin=m_pEnvironment->GetSkin();
	ISpriteBank *sprites=0;
	Core::Color32 color(255,255,255,255);
	if(skin)
	{
		sprites=skin->GetSpriteBank();
		color=skin->GetColor(DC_WINDOWSYMBOL);
	}

	int buttonw=skin->GetSize(DS_WINDOWBUTTONWIDTH);
	int posx=m_RelativeRect.GetWidth() - buttonw - 4;

	m_pClose=m_pEnvironment->AddButton(Core::Rect(posx,3,posx + buttonw,3 + buttonw),this,0,INVALID_ID,
		"",skin ? skin->GetDefaultText(DT_WINDOWCLOSE) : "Fermer");
	m_pClose->SetSubElement(true);
	m_pClose->SetTabStop(false);
	if (sprites)
	{
		m_pClose->SetSpriteBank(sprites);
		m_pClose->SetSprite(BS_UP,skin->GetIcon(DI_WINDOWCLOSE),color);
		m_pClose->SetSprite(BS_DOWN,skin->GetIcon(DI_WINDOWCLOSE),color);
	}
	m_pClose->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pOK=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth()-80,30,m_RelativeRect.GetWidth()-10,50),
		this,0,INVALID_ID,skin ? skin->GetDefaultText(DT_MSGBOXOK) : "OK");
	m_pOK->SetSubElement(true);
	m_pOK->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pCancel=m_pEnvironment->AddButton(Core::Rect(m_RelativeRect.GetWidth()-80,55,m_RelativeRect.GetWidth()-10,75),
		this,0,INVALID_ID, skin ? skin->GetDefaultText(DT_MSGBOXCANCEL) : "Annuler");
	m_pCancel->SetSubElement(true);
	m_pCancel->SetAlignment(AL_LOWERRIGHT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_UPPERLEFT);

	m_pFileBox=m_pEnvironment->AddListBox(Core::Rect(10,55,m_RelativeRect.GetWidth()-90,230),this,
		0,INVALID_ID,true);
	m_pFileBox->SetSubElement(true);
	m_pFileBox->SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);

	m_pFileNameText=m_pEnvironment->AddStaticText("",Core::Rect(10,30,m_RelativeRect.GetWidth()-90,50),true,false,this,0);
	m_pFileNameText->SetSubElement(true);
	m_pFileNameText->SetAlignment(AL_UPPERLEFT,AL_LOWERRIGHT,AL_UPPERLEFT,AL_LOWERRIGHT);

	SetTabGroup(true);
	FillListBox();
}

void FileDialog::Clear()
{
	RemoveChild(m_pClose,true);
	RemoveChild(m_pOK,true);
	RemoveChild(m_pCancel,true);
	RemoveChild(m_pFileBox,true);
	RemoveChild(m_pFileNameText,true);
	if(m_pFileList) delete m_pFileList;

	m_pClose=0;
	m_pOK=0;
	m_pCancel=0;
	m_pFileBox=0;
	m_pFileNameText=0;
	m_pFileList=0;

	m_bDragging=false;
	m_bNoDirs=false;
	m_pFileNameText=0;
	m_pFileList=0;
	m_pClose=0;
	m_pOK=0;
	m_pCancel=0;
	m_pFileBox=0;

	m_Filters.clear();

	IElement::Clear();
}

const char *FileDialog::GetFileName() const
{
	return m_strFileName.c_str();
}

bool FileDialog::OnEvent(const GUI::Event &_Event)
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
								m_strFileName="";
							else
								m_strFileName=m_pFileList->GetFileName(selected);
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
							}
							else
							{
								m_strFileName=m_pFileList->GetFileName(selected);
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

void FileDialog::Remove()
{
	g_FileSystem.ResetWorkingDirectory();

	if(m_pParent)
		m_pParent->RemoveChild(this,true);
}

void FileDialog::Draw()
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
}

void FileDialog::FillListBox()
{
	ISkin *skin=m_pEnvironment->GetSkin();

	if(!m_pFileBox || !skin)
		return;

	if(!m_pFileList)
		m_pFileList=new Core::FileList;
	else
		m_pFileList->Reset(&m_Filters,m_bNoDirs);

	m_pFileBox->ClearList();

	std::string s;
	uint32 i;

	for(i=0;i<m_pFileList->GetFileCount();i++)
	{
		s=m_pFileList->GetFileName(i);
		m_pFileBox->AddItem(s.c_str(),skin->GetIcon(m_pFileList->IsDirectory(i) ? DI_DIRECTORY : DI_FILE));
	}

	if(m_pFileNameText)
	{
		m_strPath=g_FileSystem.GetWorkingDirectory();
		m_strPath+="\\";
		m_pFileNameText->SetText(m_strPath.c_str());
	}
}

void FileDialog::SendSelectedEvent()
{
	Event e;
	e.EventType=ET_GUI;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;
	e.sGUI.EventType=GE_FILESELECTED;
	SendEvent(e);
}

void FileDialog::SendCancelEvent()
{
	Event e;
	e.EventType=ET_GUI;
	e.sGUI.pCaller=this;
	e.sGUI.pElement=0;
	e.sGUI.EventType=GE_FILECANCELLED;
	SendEvent(e);
}

void FileDialog::AddExtFilter(const char *szFilter)
{
	if(szFilter)
	{
		m_Filters.push_back(std::string(szFilter));
		FillListBox();
	}
}

const char *FileDialog::GetPath() const
{
	return m_strPath.c_str();
}

void FileDialog::SetNoDirs(const bool bNoDirs)
{
	m_bNoDirs=bNoDirs;
	FillListBox();
}

void FileDialog::SetPath(const char *szPath)
{
	g_FileSystem.ChangeWorkingDirectory(szPath);
	FillListBox();
}

}