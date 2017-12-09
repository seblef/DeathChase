
#ifndef DEEXPORT

#include "MainApp.h"
#include "GUI/GUITask.h"
#include "Core/TextOutput.h"
#include "Core/Kernel.h"
#include "Core/WindowApp.h"
#include "Core/Renderer.h"
#include "Core/Input.h"
#include "Core/Config.h"
#include "Core/Matrix.h"
#include "Core/FileSystem.h"
#include "Core/Timer.h"
#include "Core/MeshMgr.h"
#include "Game/GameState.h"
#include "Menu/MenuTask.h"
#include "Core/Sound.h"
#include "Core/ParticleEngine.h"
#include "Game/ExplosionFactory.h"

#ifdef DEDEV
#include "Dev/DevTask.h"
#include "Dev/MemAllocMgr.h"
#include "Dev/Console.h"
#include "Dev/MemDebugger.h"
#endif



bool MainApp::InitializeServices(HINSTANCE hInst)
{
	g_Output->Start("DeathChase.log",true);

	Core::Matrix4::InitTrigo();

#ifdef DEDEV
	new Dev::MemDebugger;
	new Dev::MemAllocMgr;
#endif

	new Core::Config;
	new Core::Kernel;
	new Core::FileSystem;
	new Core::Timer;

	bool bRet=g_Config.ParseFile("DeathChase.ini");
	MYASSERT(bRet,"Loading config file",true);

	return true;
}

bool MainApp::InitializeTasks(HINSTANCE hInst)
{
	new Core::Input(0,hInst);
	new Core::WindowApp(1,hInst);
	new Game::GameState(3);
	new Menu::MenuTask(4);

#ifdef DEDEV
	new Dev::DevTask(5);
#endif

	new GUI::GUITask(7);
	new Core::Sound(8);
	new Core::Renderer(2,hInst);

	g_Kernel.AddTask((Core::ITask *)Core::WindowApp::GetSingletonPtr());
	g_Kernel.AddTask((Core::ITask *)Core::Renderer::GetSingletonPtr());
	g_Kernel.AddTask((Core::ITask *)Core::Input::GetSingletonPtr());
	g_Kernel.AddTask((Core::ITask *)GUI::GUITask::GetSingletonPtr());
	g_Kernel.AddTask((Core::ITask *)Menu::MenuTask::GetSingletonPtr());
	g_Kernel.AddTask((Core::ITask *)Core::Sound::GetSingletonPtr());
#ifdef DEDEV	
	g_Kernel.AddTask((Core::ITask *)Dev::DevTask::GetSingletonPtr());
#endif

	g_GUI.Enable(true);
	g_WindowApp.SetCursorEnable(false);
	g_Menu.SetMenu(Menu::MENU_MAIN);

	new Core::ParticleEngine;
	new Game::ExplosionFactory;

	return true;
}

void MainApp::Run()
{
	const uint32 dwStartTime=timeGetTime();

	g_Kernel.Execute();

	const uint32 dwEndTime=timeGetTime();
	const float fTime=(float)(((double)(dwEndTime - dwStartTime)) / 1000.0f);
	const float fFps=((float)g_Kernel.GetNbFrames())*KINV(fTime);

	g_Output->AddMsg("<------------------- FPS: %f ----------------->\n",fFps);
}

void MainApp::Shutdown()
{
	g_Kernel.KillAllTasks();
	g_Renderer.Shutdown();
	g_Config.Destroy();
	g_Output->End();

#ifdef DEDEV
	g_MemDebugger.SaveResults();
#endif
}

#endif