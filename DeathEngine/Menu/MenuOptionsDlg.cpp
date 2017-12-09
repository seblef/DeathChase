
#include "MenuOptionsDlg.h"
#include "../GUI/ITabControl.h"
#include "../GUI/IComboBox.h"
#include "../GUI/ICheckBox.h"
#include "../GUI/IKeyButton.h"
#include "../GUI/IWindow.h"
#include "../GUI/IButton.h"
#include "../Core/Renderer.h"
#include "../Core/WindowApp.h"
#include "../Core/Config.h"

namespace Menu
{

#define MENUOPTIONS_VIDEOIDSTART	1004
#define MENUOPTIONS_VIDEOIDEND		1017

#define MENUOPTIONS_KEYSIDSTART		1020
#define MENUOPTIONS_KEYSIDEND		1033

#define MOPT_CANCELBTN				1002
#define MOPT_OKBTN					1003

#define MOPT_SHADOWSCKB				1010
#define MOPT_RESCB					1011
#define MOPT_ANTIALIASCB			1012
#define MOPT_ANISOTROPCB			1013
#define MOPT_SHADERSCB				1014
#define MOPT_TERRAINCB				1015
#define MOPT_OBJECTCB				1016
#define MOPT_WINDOWEDCKB			1017

#define MOPT_RIGHTKEY				1027
#define MOPT_LEFTKEY				1028
#define MOPT_BACKKEY				1029
#define MOPT_MAINFIREKEY			1030
#define MOPT_SECFIREKEY				1031
#define MOPT_DEFKEY					1032
#define MOPT_BONUSKEY				1033

#define MOPT_TAB					1099

const char *szKeyNames[]={
	"RightKey",
	"LeftKey",
	"BackKey",
	"MainFireKey",
	"SecFireKey",
	"DefenseKey",
	"BonusKey" };

MenuOptionsDlg::MenuOptionsDlg() : m_bFinished(false)
{
}

MenuOptionsDlg::~MenuOptionsDlg()
{
}

void MenuOptionsDlg::OnCreate()
{
	ShowKeys(false);
	m_pWindow->GetCloseButton()->SetVisible(false);
	m_pWindow->GetCloseButton()->SetEnable(false);
	m_pWindow->GetMinimizeButton()->SetVisible(false);
	m_pWindow->GetMinimizeButton()->SetEnable(false);
	m_pWindow->GetMaximizeButton()->SetVisible(false);
	m_pWindow->GetMaximizeButton()->SetEnable(false);

	Core::Rect rel(m_pWindow->GetRelativePosition());
	Core::Rect new_pos;
	
	new_pos.m_vUpperLeft.x=(g_Renderer.GetViewportSize().x - rel.GetWidth()) / 2;
	new_pos.m_vUpperLeft.y=(g_Renderer.GetViewportSize().y - rel.GetHeight()) / 2;
	new_pos.m_vLowerRight=new_pos.m_vUpperLeft + rel.GetSize();

	m_pWindow->SetRelativePosition(new_pos);

	D3DCAPS9 caps;
	g_Renderer.GetDevice()->GetDeviceCaps(&caps);
	int selected;
	const char *szTemp;

// *********************** Anisotrop **********************************
	int anisotrop;
	anisotrop=g_Config.GetIParameter("Anisotropic",0);
	selected=0;

	GUI::IComboBox *combo=(GUI::IComboBox *)GetElementByID(MOPT_ANISOTROPCB);
	combo->AddItem("Aucun");

	if(caps.MaxAnisotropy>=2)
	{
		combo->AddItem("x2");
		if(anisotrop==2)
			selected=1;
	}
	if(caps.MaxAnisotropy>=4)
	{
		combo->AddItem("x4");
		if(anisotrop==4)
			selected=2;
	}
	if(caps.MaxAnisotropy>=8)
	{
		combo->AddItem("x8");
		if(anisotrop==8)
			selected=3;
	}

	combo->SetSelected(selected);

// *********************** Windowed ***********************************

	GUI::ICheckBox *ckb=(GUI::ICheckBox *)GetElementByID(MOPT_WINDOWEDCKB);
	bool bWindowed=g_Config.GetBParameter("Windowed",false);
	ckb->SetChecked(bWindowed);

// *********************** Dynamic Shadows ****************************

	ckb=(GUI::ICheckBox *)GetElementByID(MOPT_SHADOWSCKB);
	bool bShadows=g_Config.GetBParameter("DynamicShadows",false);
	ckb->SetChecked(bShadows);

// ************************ Antialias **********************************

	FillAntialiasCombo(bWindowed);

// ************************ Resolution *********************************

	FillResCombo(bWindowed);

// ************************ Shaders ************************************

	selected=0;
	szTemp=g_Config.GetSParameter("ShadersDetail","low");

	combo=(GUI::IComboBox *)GetElementByID(MOPT_SHADERSCB);
	combo->AddItem("Faibles");

	if(caps.PixelShaderVersion >= D3DPS_VERSION(2,0) && caps.VertexShaderVersion >= D3DVS_VERSION(2,0))
	{
		combo->AddItem("Moyens");
		if(strcmp(szTemp,"medium")==0)
			selected=1;
	}

	if(caps.PixelShaderVersion >= D3DPS_VERSION(3,0) && caps.VertexShaderVersion >= D3DVS_VERSION(3,0))
	{
		combo->AddItem("Elevés");
		if(strcmp(szTemp,"high")==0)
			selected=2;
	}

	combo->SetSelected(selected);

// ************************ Details ***********************************

	selected=0;
	szTemp=g_Config.GetSParameter("TerrainDetails","low");

	if(strcmp(szTemp,"medium")==0)
		selected=1;
	else if(strcmp(szTemp,"high")==0)
		selected=2;

	((GUI::IComboBox *)GetElementByID(MOPT_TERRAINCB))->SetSelected(selected);

	selected=0;
	szTemp=g_Config.GetSParameter("ObjectDetails","low");

	if(strcmp(szTemp,"medium")==0)
		selected=1;
	else if(strcmp(szTemp,"high")==0)
		selected=2;

	((GUI::IComboBox *)GetElementByID(MOPT_OBJECTCB))->SetSelected(selected);

// ******************** Keyboard *************************************

	int i,j=0;
	for(i=MOPT_RIGHTKEY;i<=MOPT_BONUSKEY;i++,j++)
		((GUI::IKeyButton *)GetElementByID(i))->SetKey(g_Config.GetIParameter(szKeyNames[j],0));
}

void MenuOptionsDlg::OnClick(const uint32 dwID)
{
	if(dwID==MOPT_OKBTN)
	{
		ApplyChanges();
		m_bFinished=true;
	}
	else if(dwID==MOPT_CANCELBTN)
		m_bFinished=true;
}

void MenuOptionsDlg::OnTab(const uint32 dwID)
{
	if(dwID==MOPT_TAB)
	{
		GUI::ITabControl *pTab=(GUI::ITabControl*)GetElementByID(dwID);
		switch(pTab->GetActiveTab())
		{
			case 0:
				ShowVideo(true);
				ShowKeys(false);
				break;

			case 1:
				ShowVideo(false);
				ShowKeys(true);
				break;

			default:
				break;
		}
	}
}

void MenuOptionsDlg::ShowVideo(bool bShow)
{
	uint32 i;
	for(i=MENUOPTIONS_VIDEOIDSTART;i<=MENUOPTIONS_VIDEOIDEND;i++)
		GetElementByID(i)->SetVisible(bShow);
}

void MenuOptionsDlg::ShowKeys(bool bShow)
{
	uint32 i;
	for(i=MENUOPTIONS_KEYSIDSTART;i<=MENUOPTIONS_KEYSIDEND;i++)
		GetElementByID(i)->SetVisible(bShow);
}

void MenuOptionsDlg::ApplyChanges()
{
// ****************************** Keys ************************************
	int i,j=0;
	for(i=MOPT_RIGHTKEY;i<=MOPT_BONUSKEY;i++,j++)
		g_Config.SetIParameter(szKeyNames[j],((GUI::IKeyButton *)GetElementByID(i))->GetKey());

// ****************************** Resolution / Windowed *******************

	bool bWindowed=((GUI::ICheckBox *)GetElementByID(MOPT_WINDOWEDCKB))->IsChecked();
	g_Config.SetBParameter("Windowed",bWindowed);
	int selected=((GUI::IComboBox *)GetElementByID(MOPT_RESCB))->GetSelected();
	int width,height;

	if(bWindowed)
	{
		switch(selected)
		{
			case 1: width=800; height=600; break;
			case 2: width=1024; height=768; break;
			default: width=640; height=480; break;
		}

		g_Config.SetIParameter("WindowWidth",width);
		g_Config.SetIParameter("WindowHeight",height);
	}
	else
	{
		IDirect3D9 *d3d=g_Renderer.GetD3D();
		D3DDISPLAYMODE dm;
		d3d->EnumAdapterModes(D3DADAPTER_DEFAULT,D3DFMT_X8R8G8B8,(UINT)selected,&dm);
		g_Config.SetIParameter("FullscreenWidth",(int)dm.Width);
		g_Config.SetIParameter("FullscreenHeight",(int)dm.Height);
	}

// ******************************* Multisample *******************************

	selected=((GUI::IComboBox *)GetElementByID(MOPT_ANTIALIASCB))->GetSelected();
	int multisample=0;
	switch(selected)
	{
		case 1: multisample=2; break;
		case 2: multisample=4; break;
		case 3: multisample=8; break;
	}

	g_Config.SetIParameter("MultiSample",multisample);

// ******************************* Anisotropic *******************************

	selected=((GUI::IComboBox *)GetElementByID(MOPT_ANISOTROPCB))->GetSelected();
	int anisotrop=0;
	switch(selected)
	{	
		case 1: anisotrop=2; break;
		case 2: anisotrop=4; break;
		case 3: anisotrop=8; break;
	}

	g_Config.SetIParameter("Anisotropic",anisotrop);

// ****************************** Dynamic Shadows ****************************

	g_Config.SetBParameter("DynamicShadows",((GUI::ICheckBox *)GetElementByID(MOPT_SHADOWSCKB))->IsChecked());

// ****************************** Shaders ************************************

	const char *details[]={"low","medium","high"};

	selected=((GUI::IComboBox *)GetElementByID(MOPT_SHADERSCB))->GetSelected();
	g_Config.SetSParameter("ShadersDetails",details[selected]);

// ****************************** Geom. details ******************************

	selected=((GUI::IComboBox *)GetElementByID(MOPT_TERRAINCB))->GetSelected();
	g_Config.SetSParameter("TerrainDetails",details[selected]);

	selected=((GUI::IComboBox *)GetElementByID(MOPT_OBJECTCB))->GetSelected();
	g_Config.SetSParameter("ObjectDetails",details[selected]);

	g_Config.Save("DeathChase.ini");

/*	if(bWindowed)
		g_WindowApp.Resize(width,height);

	g_Renderer.Restart();
*/
}

void MenuOptionsDlg::FillAntialiasCombo(bool bWindowed)
{
	D3DCAPS9 caps;
	g_Renderer.GetDevice()->GetDeviceCaps(&caps);
	IDirect3D9 *d3d=g_Renderer.GetD3D();

	GUI::IComboBox *combo=(GUI::IComboBox *)GetElementByID(MOPT_ANTIALIASCB);
	combo->ClearList();
	combo->AddItem("Aucun");

	D3DDISPLAYMODE d3d_dm;

	if(bWindowed)
		d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3d_dm);
	else
		d3d_dm.Format=D3DFMT_X8R8G8B8;
	DWORD quality;

	int multisample,selected=0;
	multisample=g_Config.GetIParameter("MultiSample",0);

	if(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,caps.DeviceType,d3d_dm.Format,bWindowed ? TRUE : FALSE,
		D3DMULTISAMPLE_2_SAMPLES,&quality)==D3D_OK)
	{
		combo->AddItem("2x");
		if(multisample==2)
			selected=1;
	}
	if(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,caps.DeviceType,d3d_dm.Format,bWindowed ? TRUE : FALSE,
		D3DMULTISAMPLE_4_SAMPLES,&quality)==D3D_OK)
	{
		combo->AddItem("4x");
		if(multisample==4)
			selected=2;
	}
	if(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,caps.DeviceType,d3d_dm.Format,bWindowed ? TRUE : FALSE,
		D3DMULTISAMPLE_8_SAMPLES,&quality)==D3D_OK)
	{
		combo->AddItem("8x");
		if(multisample==8)
			selected=3;
	}

	combo->SetSelected(selected);
}

void MenuOptionsDlg::OnCheck(const uint32 dwID)
{
	if(dwID==MOPT_WINDOWEDCKB)
	{
		GUI::ICheckBox *ckb=(GUI::ICheckBox *)GetElementByID(MOPT_WINDOWEDCKB);
		FillAntialiasCombo(ckb->IsChecked());
		FillResCombo(ckb->IsChecked());
	}
}

void MenuOptionsDlg::FillResCombo(bool bWindowed)
{
	GUI::IComboBox *combo=(GUI::IComboBox *)GetElementByID(MOPT_RESCB);
	combo->ClearList();

	int width,height;

	if(bWindowed)
	{
		combo->AddItem("640x480");
		combo->AddItem("800x600");
		combo->AddItem("1024x768");

		width=g_Config.GetIParameter("WindowWidth",640);
		if(width==800)
			combo->SetSelected(1);
		else if(width==1024)
			combo->SetSelected(2);
		else
			combo->SetSelected(0);

		return;
	}

	IDirect3D9 *d3d=g_Renderer.GetD3D();
	UINT count=d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT,D3DFMT_X8R8G8B8);
	UINT i;
	D3DDISPLAYMODE dm;
	char szTemp[64];
	int selected=0;
	width=g_Config.GetIParameter("FullscreenWidth",640);
	height=g_Config.GetIParameter("FullscreenHeight",480);

	for(i=0;i<count;i++)
	{
		d3d->EnumAdapterModes(D3DADAPTER_DEFAULT,D3DFMT_X8R8G8B8,i,&dm);
		sprintf(szTemp,"%ix%i %iHz",dm.Width,dm.Height,dm.RefreshRate);
		combo->AddItem(szTemp);

		if(width==dm.Width && height==dm.Height)
			selected=(int)i;
	}

	combo->SetSelected(selected);
}

}