
#include "Dialog.h"
#include "GUITask.h"
#include "../Core/TextureMgr.h"
#include "IWindow.h"
#include "IButton.h"
#include "IKeyButton.h"
#include "ICheckBox.h"
#include "IComboBox.h"
#include "IEditBox.h"
#include "IImage.h"
#include "IListBox.h"
#include "IScrollBar.h"
#include "ISpinBox.h"
#include "IStaticText.h"
#include "ITabControl.h"
#include "IToolBar.h"

namespace GUI
{

Dialog::Dialog(bool bModal) : m_bShow(false),m_bMenuCreated(false),m_pWindow(0),m_bModal(bModal)
{
	m_pEnvironment=g_GUI.GetEnvironment();
}

Dialog::~Dialog()
{
	if(m_pWindow)
		m_pEnvironment->ReleaseElement((IElement *)m_pWindow);
}

bool Dialog::Create(const int x, const int y, const int width, const int height,
					 const char *szCaption, const bool bExecute,const uint32 dwID,
					 bool bOnCreateMsg)
{
	if(m_pWindow)
		return false;

	m_pWindow=m_pEnvironment->AddWindow(Core::Rect(x,y,x+width,y+height),m_bModal,szCaption,0,this,dwID);

	if(!m_pWindow)
		return false;

	m_pWindow->SetText(szCaption);

	strcpy(m_szName,szCaption);

	if(bOnCreateMsg)
		OnCreate();

	return true;
}

bool Dialog::Destroy()
{
	OnDestroy();

	if(m_pWindow)
		m_pEnvironment->ReleaseElement((IElement *)m_pWindow);
	m_pWindow=0;

	m_Controls.clear();

	return true;
}

void Dialog::SetCaption(const char *szCaption)
{
	if(m_pWindow && szCaption)
		m_pWindow->SetText(szCaption);
}

void Dialog::Show(const bool bShow)
{
	if(m_pWindow)
		m_pWindow->SetVisible(bShow);

	m_bShow=bShow;
}

void Dialog::Show()
{
	Show(true);
}

void Dialog::Hide()
{
	Show(false);
}

bool Dialog::OnEvent(const Event& e)
{
	switch(e.EventType)
	{
		case ET_MOUSE:
			switch(e.sMouse.EventType)
			{
				case ME_MOVED:
					OnMouseMove((int)e.sMouse.dwX,(int)e.sMouse.dwY);
					break;

				case ME_LMOUSEDOWN:
				case ME_RMOUSEDOWN:
					OnMouseDown((int)e.sMouse.dwX,(int)e.sMouse.dwY);
					break;

				default:
					break;
			}
			break;

		case ET_GUI:
			switch(e.sGUI.EventType)
			{
				case GE_BUTTONCLICKED:
					OnClick(e.sGUI.pCaller->GetID());
					break;

				case GE_CHECKBOXCHANGED:
					OnCheck(e.sGUI.pCaller->GetID());
					break;

				case GE_MOVED:
					OnMove();
					break;

				case GE_SCROLLBARCHANGED:
					OnScroll(e.sGUI.pCaller->GetID());
					break;

				case GE_LISTBOXCHANGED:
				case GE_COMBOBOXCHANGED:
				case GE_FILESELECTED:
					OnSelect(e.sGUI.pCaller->GetID());
					break;

				case GE_LISTBOXSELECTEDAGAIN:
					OnDblClick(e.sGUI.pCaller->GetID());
					break;

				case GE_MENUSELECTED:
					OnMenu(e.sGUI.pCaller->GetID());
					break;

				case GE_CLOSED:
					Show(false);
					break;

				case GE_MINIMIZE:
					OnMinimize();
					break;

				case GE_RESTORE:
					OnRestore();
					break;

				case GE_EDITBOXENTER:
					OnEditBoxEnter(e.sGUI.pCaller->GetID());
					break;

				case GE_FOCUSED:
					OnSetFocus();
					break;

				case GE_FOCUSLOST:
					OnKillFocus();
					break;

				case GE_TABCHANGED:
					OnTab(e.sGUI.pCaller->GetID());
					break;

				case GE_SPINBOXCHANGED:
					OnSpinBox(e.sGUI.pCaller->GetID());
					break;

				case GE_KEYSELECTED:
					OnKeySelected(e.sGUI.pCaller->GetID());
					break;

				default:
					break;
			}

		default:
			break;
	}

	return true;
}

void Dialog::RemoveControl(IElement *pControl)
{
	if(!pControl)
		return;

	std::vector<IElement *>::iterator i;
	for(i=m_Controls.begin();i!=m_Controls.end();i++)
		if((*i)==pControl)
		{
			m_Controls.erase(i);
			break;
		}

	pControl->Remove();
}

void Dialog::AddControl(IElement *pControl, bool bAddAsChild)
{
	if(!pControl)
		return;

	pControl->SetUserEventReceiver((IEventReceiver *)this);

	if(bAddAsChild)
		m_pWindow->AddChild(pControl);
	
	m_Controls.push_back(pControl);
}

IElement *Dialog::GetElementByID(const uint32 dwID)
{
	std::vector<IElement *>::iterator i;
	for(i=m_Controls.begin();i!=m_Controls.end();i++)
		if((*i)->GetID()==dwID)
			return *i;

	return 0;
}

bool Dialog::LoadTemplate(const char *szFileName)
{
	MYASSERT(szFileName,"Bad dialog template filename",true);

	char szTemp[1024]="GUI\\Templates\\";
	strcat(szTemp,szFileName);

	Core::ScriptFile file;

	if(!file.Open(szTemp))
	{
		g_Output->AddMsg("x La chargement du modele de dialog %s a echoue.\n",szFileName);
		return false;
	}

	const char *szToken=file.GetToken();
	if(!szToken || strcmp(szToken,"dialog")!=0)
	{
		g_Output->AddMsg("x Le modele de dialog %s ne commencer par le mot clé dialog\n",szFileName);
		file.Close();
		return false;
	}

	Core::Rect r;
	int i;

	if(!ParseSize(&file,r))
	{
		file.Close();
		return false;
	}

	szToken=file.GetToken();
	if(!szToken)
	{
		file.Close();
		return Create(r.m_vUpperLeft.x,r.m_vUpperLeft.y,r.GetWidth(),r.GetHeight(),"");
	}
	else if(!Create(r.m_vUpperLeft.x,r.m_vUpperLeft.y,r.GetWidth(),r.GetHeight(),szToken,false,INVALID_ID,false))
	{
		file.Close();
		g_Output->AddMsg("x Impossible de créer la boite de dialog %s\n",szToken);
		return false;
	}

	szToken=file.GetToken();
	if(strcmp(szToken,"{")!=0)
	{
		g_Output->AddMsg("x Erreur (token: %s) dans le modele de dialog %s: { attendu apres la definition de la boite\n",szToken,szFileName);
		file.Close();
		return true;
	}

	szToken=file.GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"button")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseButton(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"keybutton")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseKeyButton(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"checkbox")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseCheckBox(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"combobox")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseComboBox(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"editbox")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseEditBox(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"image")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseImage(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"listbox")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseListBox(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"menu")==0)
		{
			if(CheckBrace(&file))
			{
				if(!ParseMenu(&file))
					break;
			}
		}
		else if(strcmp(szToken,"scrollbar")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseScrollBar(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"spinbox")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseSpinBox(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"statictext")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseStaticText(&file,r))
					break;
			}
		}
		else if(strcmp(szToken,"tab")==0)
		{
			if(ParseSize(&file,r) && CheckBrace(&file))
			{
				if(!ParseTab(&file,r))
					break;
			}
		}
		else
			g_Output->AddMsg("x Token %s inconnu dans le modele de dialog %s\n",szToken,szFileName);

		szToken=file.GetToken();
	}


	file.Close();

	OnCreate();
	return true;
}

bool Dialog::ParseSize(Core::ScriptFile *file, Core::Rect& r)
{
	int sz[4];
	const char *szToken;
	int i;

	for(i=0;i<4;i++)
	{
		szToken=file->GetToken();
		if(!szToken)
		{
			g_Output->AddMsg("x Fin de fichier inattendu dans le modele de dialog %s\n",file->GetFileName());
			return false;
		}
		
		sz[i]=atoi(szToken);
	}

	r.m_vUpperLeft.x=sz[0];
	r.m_vUpperLeft.y=sz[1];
	r.m_vLowerRight.x=sz[0]+sz[2];
	r.m_vLowerRight.y=sz[1]+sz[3];

	if(!r.IsValid())
	{
		g_Output->AddMsg("x Taille de controle invalid (%i,%i,%i,%i) dans le modele de dialog %s\n",file->GetFileName());
		return false;
	}

	return true;
}

bool Dialog::CheckBrace(Core::ScriptFile *file)
{
	const char *szToken=file->GetToken();
	return (szToken && strcmp(szToken,"{")==0);
}

void Dialog::ParseAlignment(Core::ScriptFile *pFile, IElement *pElement)
{
	const char *szToken;
	ALIGNMENT align[4];
	int i;
	bool valid=true;

	for(i=0;i<4;i++)
	{
		szToken=pFile->GetToken();

		if(!szToken)
		{
			g_Output->AddMsg("x Fin de fichier inattendue dans le modele de dialog %s\n",pFile->GetFileName());
			valid=false;
			break;
		}
		else
		{
			if(strcmp(szToken,"upperleft")==0)
				align[i]=AL_UPPERLEFT;
			else if(strcmp(szToken,"lowerright")==0)
				align[i]=AL_LOWERRIGHT;
			else if(strcmp(szToken,"center")==0)
				align[i]=AL_CENTER;
			else if(strcmp(szToken,"scale")==0)
				align[i]=AL_SCALE;
			else
			{
				g_Output->AddMsg("x Commande d'alignement %s inconnue dans le modele de dialog %s\n",szToken,pFile->GetFileName());
				valid=false;
				break;
			}
		}
	}

	if(valid)
		pElement->SetAlignment(align[0],align[1],align[2],align[3]);
}

bool Dialog::ParseItems(Core::ScriptFile *pFile, std::vector<std::string> *pItems)
{
	const char *szToken=pFile->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		pItems->push_back(std::string(szToken));
		szToken=pFile->GetToken();
	}

	return true; //szToken!=0;
}

bool Dialog::ParseButton(Core::ScriptFile *file, Core::Rect& r)
{
	const char *szToken=file->GetToken();

	IButton *button=m_pEnvironment->AddButton(r,(IElement *)m_pWindow,this);
	if(!button)
		return false;

	AddControl((IElement *)button);

	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"text")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"font")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetOverrideFont(m_pEnvironment->GetFont(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"image")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetImage(g_TextureMgr.LoadTexture(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"pressedimage")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetPressedImage(g_TextureMgr.LoadTexture(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"spritebank")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetSpriteBank(m_pEnvironment->GetSpriteBank(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"sprite")==0)
		{
			szToken=file->GetToken();
			const char *szToken2=file->GetToken();
			if(szToken && szToken2)
			{
				if(strcmp(szToken,"up")==0)
					button->SetSprite(BS_UP,atoi(szToken2));
				else if(strcmp(szToken,"down")==0)
					button->SetSprite(BS_DOWN,atoi(szToken2));
				else
					g_Output->AddMsg("x Token %s inconnu apres la commande button sprite dans le modele de dialog %s\n", szToken2, 
						file->GetFileName());
			}
			else
				break;
		}
		else if(strcmp(szToken,"push")==0)
			button->SetIsPushButton(true);
		else if(strcmp(szToken,"pressed")==0)
			button->SetPressed(true);
		else if(strcmp(szToken,"alphause")==0)
			button->SetUseAlphaChannel(true);
		else if(strcmp(szToken,"noborder")==0)
			button->SetDrawBorder(false);
		else if(strcmp(szToken,"disable")==0)
			button->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			button->SetVisible(false);
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)button);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande button du modele de dialog %s\n",szToken,file->GetFileName());
		
		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseKeyButton(Core::ScriptFile *file, Core::Rect& r)
{
	const char *szToken=file->GetToken();

	IKeyButton *button=m_pEnvironment->AddKeyButton(r,(IElement *)m_pWindow,this);
	if(!button)
		return false;

	AddControl((IElement *)button);

	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				button->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"disable")==0)
			button->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			button->SetVisible(false);
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)button);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande button du modele de dialog %s\n",szToken,file->GetFileName());
		
		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseCheckBox(Core::ScriptFile *file, Core::Rect& r)
{
	ICheckBox *cb=m_pEnvironment->AddCheckBox(false,r,(IElement *)m_pWindow,this);
	if(!cb)
		return false;

	AddControl((IElement *)cb);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"checked")==0)
			cb->SetChecked(true);
		else if(strcmp(szToken,"text")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				cb->SetText(szToken);
			else break;
		}
		else if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				cb->SetID(atoi(szToken));
			else break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				cb->SetToolTipText(szToken);
			else break;
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)cb);
		else if(strcmp(szToken,"disable")==0)
			cb->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			cb->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande checkbox du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseComboBox(Core::ScriptFile *file, Core::Rect& r)
{
	IComboBox *cb=m_pEnvironment->AddComboBox(r,(IElement *)m_pWindow,this);
	if(!cb)
		return false;

	AddControl((IElement *)cb);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				cb->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				cb->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"items")==0)
		{
			if(CheckBrace(file))
			{
				std::vector<std::string> stringvec;
				if(ParseItems(file,&stringvec))
				{
					std::vector<std::string>::iterator i;
					for(i=stringvec.begin();i!=stringvec.end();i++)
						cb->AddItem(i->c_str());
				}
				else
					break;
			}
		}
		else if(strcmp(szToken,"selected")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				cb->SetSelected(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)cb);
		else if(strcmp(szToken,"disable")==0)
			cb->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			cb->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande combobox du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseEditBox(Core::ScriptFile *file, Core::Rect& r)
{
	IEditBox *eb=m_pEnvironment->AddEditBox("",r,true,(IElement *)m_pWindow,this);
	if(!eb)
		return false;

	AddControl((IElement *)eb);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				eb->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				eb->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"text")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				eb->SetText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"noborder")==0)
			eb->SetDrawBorder(false);
		else if(strcmp(szToken,"color")==0)
		{
			Core::Color32 c;
			if(file->ParseColor(c))
				eb->SetOverrideColor(c);
			else
				break;
		}
		else if(strcmp(szToken,"font")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				eb->SetOverrideFont(m_pEnvironment->GetFont(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"multiline")==0)
			eb->SetMultiLine(true);
		else if(strcmp(szToken,"password")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				eb->SetPasswordBox(true,szToken[0]);
			else
				break;
		}
		else if(strcmp(szToken,"textalign")==0)
		{
			const char *szToken2;
			ALIGNMENT align[2];
			int i;
			bool valid=true;

			for(i=0;i<2;i++)
			{
				szToken2=file->GetToken();

				if(!szToken2)
				{
					g_Output->AddMsg("x Fin de fichier inattendue dans le modele de dialog %s\n",file->GetFileName());
					valid=false;
					break;
					break;
				}
				else
				{
					if(strcmp(szToken2,"upperleft")==0)
						align[i]=AL_UPPERLEFT;
					else if(strcmp(szToken2,"lowerright")==0)
						align[i]=AL_LOWERRIGHT;
					else if(strcmp(szToken2,"center")==0)
						align[i]=AL_CENTER;
					else if(strcmp(szToken2,"scale")==0)
						align[i]=AL_SCALE;
					else
					{
						g_Output->AddMsg("x Commande d'alignement %s inconnue dans le modele de dialog %s\n",szToken,file->GetFileName());
						valid=false;
						break;
						break;
					}
				}
			}

			eb->SetTextAlignment(align[0],align[1]);
		}
		else if(strcmp(szToken,"wordwrap")==0)
			eb->SetWordWrap(true);
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)eb);
		else if(strcmp(szToken,"disable")==0)
			eb->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			eb->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande editbox du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseImage(Core::ScriptFile *file, Core::Rect& r)
{
	IImage *img=m_pEnvironment->AddImage(r,(IElement *)m_pWindow,this);

	if(!img)
		return false;

	AddControl((IElement *)img);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				img->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"image")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				img->SetImage(g_TextureMgr.LoadTexture(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				img->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"scale")==0)
		{
			img->SetImageScale(true);
			img->SetRelativePosition(r);
		}
		else if(strcmp(szToken,"alpha")==0)
			img->SetUseAlphaChannel(true);
		else if(strcmp(szToken,"color")==0)
		{
			Core::Color32 c;
			if(file->ParseColor(c))
				img->SetColor(c);
			else
				break;
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)img);
		else if(strcmp(szToken,"invisible")==0)
			img->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande image du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseListBox(Core::ScriptFile *file, Core::Rect& r)
{
	IListBox *lb=m_pEnvironment->AddListBox(r,(IElement *)m_pWindow,this);
	if(!lb)
		return false;

	AddControl((IElement *)lb);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				lb->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				lb->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"items")==0)
		{
			if(CheckBrace(file))
			{
				std::vector<std::string> stringvec;
				if(ParseItems(file,&stringvec))
				{
					std::vector<std::string>::iterator i;
					for(i=stringvec.begin();i!=stringvec.end();i++)
						lb->AddItem(i->c_str());
				}
				else
					break;
			}
			else
				break;
		}
		else if(strcmp(szToken,"noautoscroll")==0)
			lb->SetAutoScrollEnable(false);
		else if(strcmp(szToken,"spritebank")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				lb->SetSpriteBank(m_pEnvironment->GetSpriteBank(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)lb);
		else if(strcmp(szToken,"disable")==0)
			lb->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			lb->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande listbox du modele de dialog %s\n",szToken,file->GetFileName());
			
		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseMenu(Core::ScriptFile *file)
{
	return false;
}

bool Dialog::ParseScrollBar(Core::ScriptFile *file, Core::Rect& r)
{
	IScrollBar *sb=m_pEnvironment->AddScrollBar(false,r,(IElement *)m_pWindow,this);
	if(!sb)
		return false;

	AddControl((IElement *)sb);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"max")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetMax(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"smallstep")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetSmallStep(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"largestep")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetLargeStep(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"pos")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetPos(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)sb);
		else if(strcmp(szToken,"disable")==0)
			sb->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			sb->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande scrollbar du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseSpinBox(Core::ScriptFile *file, Core::Rect& r)
{
	ISpinBox *sb=m_pEnvironment->AddSpinBox("0",r,(IElement *)m_pWindow,this);
	if(!sb)
		return false;

	AddControl((IElement *)sb);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"range")==0)
		{
			szToken=file->GetToken();
			float min;
			if(szToken) min=atof(szToken);

			const char *szToken2=file->GetToken();

			if(szToken && szToken2)
			{
				float max=atof(szToken2);
				if(max < min)
					g_Output->AddMsg("x Mauvaises valeurs (%f,%f) pour l'écart de la spinbox du modele de dialog %s\n",
						min,max,file->GetFileName());
				else
					sb->SetRange(min,max);
			}
		}
		else if(strcmp(szToken,"step")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetStepSize(atof(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"value")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetValue(atof(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"decimals")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				sb->SetDecimalPlaces(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)sb);
		else if(strcmp(szToken,"disable")==0)
			sb->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			sb->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande spinbox du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseStaticText(Core::ScriptFile *file, Core::Rect& r)
{
	IStaticText *st=m_pEnvironment->AddStaticText("",r,false,true,(IElement *)m_pWindow,this);
	if(!st)
		return false;

	AddControl((IElement *)st);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				st->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"text")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				st->SetText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"font")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				st->SetOverrideFont(m_pEnvironment->GetFont(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"color")==0)
		{
			Core::Color32 c;
			if(file->ParseColor(c))
				st->SetOverrideColor(c);
			else
				break;
		}
		else if(strcmp(szToken,"border")==0)
			st->SetDrawBorder(true);
		else if(strcmp(szToken,"nowordwrap")==0)
			st->SetWordWrap(false);
		else if(strcmp(szToken,"backcolor")==0)
		{
			Core::Color32 c;
			if(file->ParseColor(c))
				st->SetBackgroundColor(c);
			else
				break;
		}
		else if(strcmp(szToken,"drawbackground")==0)
			st->SetDrawBackground(true);
		else if(strcmp(szToken,"textalign")==0)
		{
			const char *szToken2;
			ALIGNMENT align[2];
			int i;
			bool valid=true;

			for(i=0;i<2;i++)
			{
				szToken2=file->GetToken();

				if(!szToken2)
				{
					g_Output->AddMsg("x Fin de fichier inattendue dans le modele de dialog %s\n",file->GetFileName());
					valid=false;
					break;
					break;
				}
				else
				{
					if(strcmp(szToken2,"upperleft")==0)
						align[i]=AL_UPPERLEFT;
					else if(strcmp(szToken2,"lowerright")==0)
						align[i]=AL_LOWERRIGHT;
					else if(strcmp(szToken2,"center")==0)
						align[i]=AL_CENTER;
					else if(strcmp(szToken2,"scale")==0)
						align[i]=AL_SCALE;
					else
					{
						g_Output->AddMsg("x Commande d'alignement %s inconnue dans le modele de dialog %s\n",szToken,file->GetFileName());
						valid=false;
						break;
						break;
					}
				}
			}

			st->SetTextAlignment(align[0],align[1]);
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)st);
		else if(strcmp(szToken,"invisible")==0)
			st->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande static du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseTab(Core::ScriptFile *file, Core::Rect& r)
{
	ITabControl *tc=m_pEnvironment->AddTabControl(r,(IElement *)m_pWindow,this);
	if(!tc)
		return false;

	AddControl((IElement *)tc);

	const char *szToken=file->GetToken();
	while(szToken && strcmp(szToken,"}")!=0)
	{
		if(strcmp(szToken,"id")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				tc->SetID(atoi(szToken));
			else
				break;
		}
		else if(strcmp(szToken,"tooltip")==0)
		{
			szToken=file->GetToken();
			if(szToken)
				tc->SetToolTipText(szToken);
			else
				break;
		}
		else if(strcmp(szToken,"tabs")==0)
		{
			if(CheckBrace(file))
			{
				std::vector<std::string> stringvec;
				
				if(ParseItems(file,&stringvec))
				{
					std::vector<std::string>::iterator i;
					for(i=stringvec.begin();i!=stringvec.end();i++)
						tc->AddTab(i->c_str());
				}
				else
					break;
			}
			else
				break;
		}
		else if(strcmp(szToken,"align")==0)
			ParseAlignment(file,(IElement *)tc);
		else if(strcmp(szToken,"disable")==0)
			tc->SetEnable(false);
		else if(strcmp(szToken,"invisible")==0)
			tc->SetVisible(false);
		else
			g_Output->AddMsg("x Token %s inconnu apres la commande tab du modele de dialog %s\n",szToken,file->GetFileName());

		szToken=file->GetToken();
	}

	return true;
}

bool Dialog::ParseToolBar(const char *szFileName, IToolBar *pBar, ISpriteBank *pBank)
{
	Core::ScriptFile file;
	if(!file.Open(szFileName))
		return false;

	// 0=id, 1=tooltip, 2=spritenum, 3=push?, 4=visible?
	std::string params[5];

	const char *szToken=file.GetToken();
	while(szToken)
	{
		int i;
		for(i=0;i<5;i++)
		{
			params[i]=szToken;
			szToken=file.GetToken();
			if(!szToken && i < 4)
				return false;
		}

		IButton *button=pBar->AddButton(atoi(params[0].c_str()),"",params[1].c_str(),
			Core::HTexture(),Core::HTexture(),params[3]=="true",true);
		button->SetSpriteBank(pBank);
		int spriteno=atoi(params[2].c_str());
		button->SetSprite(BS_DOWN,spriteno);
		button->SetSprite(BS_UP,spriteno);
		button->SetVisible(params[4]=="true");
	}

	file.Close();

	return true;
}

}
