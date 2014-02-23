#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

//****************************************************************************
//**
//**    Application.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <windows.h>
#include <vector>

#include "DataTypes.h"

// Forward Declarations
namespace SWR
{
	class ApplicationSettings;
	class RenderDevice;
	class FrameListener;
	class Timer;
};

namespace SWR
{
	class Application
	{
	private:
		// Windows structures.
		HWND hWnd;
		WNDCLASS wc;

		bool m_exit;
		bool m_drawFPS;
		bool m_applicationRunning; // If the application is running. If the user tries to add a listener while it is running, the application will fail.
		
		// *****************************************************************************************
		// Application Core classes
		// *****************************************************************************************
		// The software based render device for the application. Used to draw to the screen / window.
		RenderDevice* m_device;
		ApplicationSettings* m_settings;
		Timer* m_timer;

		// The frame listeners for the application
		std::vector<FrameListener*> m_listeners;

		// Windows message handling function.
		static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		// Render device setup.
		SWR_ERR InitRenderDevice();

		// Functions for cleanly updating the application.
		void FrameStartTick();
		void RenderFrame();
		void FrameEndTick();

		Application();
	protected:
	public:
		~Application();

		// Singleton Access.
		static Application& Instance();

		SWR_ERR Startup(const ApplicationSettings &settings);
		void Shutdown();

		void SafeExit();
		void ForceExit();

		void AddFrameListener(FrameListener* listener);
		void Run();

		void ShowFPS();
		void HideFPS();
		bool IsShowingFPS() const;

		int GetFPS();

		RenderDevice* GetRenderDevice();
	};
	
}; // End namespace SWR.

#define ENGINE SWR::Application::Instance()

#endif // #ifndef APPLICATION_H