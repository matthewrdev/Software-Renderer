//****************************************************************************
//**
//**    main.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************


#include <iostream>

#include "Application.h"
#include "ApplicationSettings.h"
#include "MainListener.h"
#include "Colour.h"

#include "MemoryLeak.h"

using namespace SWR;

#if defined(_CONSOLE)
int main()
#else // !defined(_CONSOLE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#endif // #if defined(_CONSOLE)
{
	// Setup memory leak checking.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 

	ApplicationSettings settings;
	settings.m_appName = "Software Renderer";
	settings.m_clearColour = Colour32::BLACK;
	settings.m_width = 800;
	settings.m_height = 600;
	settings.m_fullscreen = false;

	float p1 = -2;
	float p2 = -2;

	float p3 = p2/p1;

	MainListener* listener = new MainListener();

	ENGINE.AddFrameListener(listener);
	SWR_ERR result = ENGINE.Startup(settings);
	if (result != SWR_OK)
	{
		MessageBox(0,"Application startup has failed, see output log for details.", 0, MB_OK);
		return 1;
	}

	ENGINE.Run();

	ENGINE.Shutdown();

	// Dump memory leaks
	_CrtDumpMemoryLeaks();
	return 0;
}