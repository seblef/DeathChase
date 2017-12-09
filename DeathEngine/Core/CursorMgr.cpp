
#include "CursorMgr.h"
#include "Input.h"
#include "Renderer2D.h"
#include "ScriptFile.h"
#include "TextureMgr.h"

namespace Core
{

CursorMgr::Cursor::~Cursor()
{
}

CursorMgr::CursorMgr(IDirect3DDevice9 *pDevice) :
	m_pDevice(pDevice),m_bEnable(true),m_pCurrent(0)
{
	Load();
}

CursorMgr::~CursorMgr()
{
}

void CursorMgr::Release()
{
	std::map<std::string,Cursor>::iterator i;
	for(i=m_Cursors.begin();i!=m_Cursors.end();i++)
		g_TextureMgr.ReleaseTexture(i->second.hTexture);

	m_Cursors.clear();
}

bool CursorMgr::SetCursor(const char *szCursorName)
{
	MYASSERT(szCursorName,"Mauvais nom de curseur",true);

	std::map<std::string,Cursor>::iterator cur=m_Cursors.find(std::string(szCursorName));
	if(cur==m_Cursors.end())
		return false;

	m_pCurrent=&cur->second;
	g_TextureMgr.GetTextureSize(m_pCurrent->hTexture,m_iSizeX,m_iSizeY);

	return true;
}

bool CursorMgr::AddCursor(const char *szName, const char *szFile, int iPosX, int iPosY)
{
	MYASSERT(szName && szFile,"Mauvais noms de curseur",true);

	Cursor cur;
	cur.hTexture=g_TextureMgr.LoadTexture(szName,"");
	if(!cur.hTexture.IsNull())
	{
		cur.iPosX=iPosX;
		cur.iPosY=iPosY;
		m_Cursors.insert(std::make_pair(std::string(szName),cur));
		return true;
	}
	else
	{
		g_Output->AddMsg("x Impossible de charger le curseur %s\n",szFile);
		return false;
	}
}

void CursorMgr::Enable(bool bEnable)
{
	m_bEnable=bEnable;
}

void CursorMgr::Update()
{
	if(!m_bEnable || !m_pCurrent)
		return;

	Input::MouseState mState;
	g_Input.GetMouseState(mState);

	const int x=mState.iX-m_pCurrent->iPosX;
	const int y=mState.iY-m_pCurrent->iPosY;
	g_Renderer2D.Draw2DImage(m_pCurrent->hTexture,x,y,x+m_iSizeX,y+m_iSizeY);
}

void CursorMgr::Load()
{
	ScriptFile file;

	if(!file.Open("GUI\\Cursors\\cursor.ini"))
		return;

	char filename[1024];
	std::string name;
	const char *szToken=file.GetToken();
	while(szToken)
	{
		Cursor cur;

		name=szToken;
		szToken=file.GetToken();

		if(szToken)
		{
			sprintf(filename,"GUI\\Cursors\\%s",szToken);
			cur.hTexture=g_TextureMgr.LoadTexture(filename,"");
			if(!cur.hTexture.IsNull())
			{
				cur.iPosX=atoi(file.GetToken());
				cur.iPosY=atoi(file.GetToken());

				m_Cursors.insert(std::make_pair(name,cur));
			}
			else
			{
				file.GetToken();
				file.GetToken();
			}
		}

		szToken=file.GetToken();
	}

	file.Close();
}

}