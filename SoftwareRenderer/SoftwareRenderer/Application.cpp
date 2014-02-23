//****************************************************************************
//**
//**    Application.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "Application.h"

#include "InputHandler.h"

#include "RenderDevice.h"
#include "ApplicationSettings.h"
#include "FrameListener.h"
#include "Timer.h"
#include "Logger.h"

#include "MemoryLeak.h"

namespace SWR
{
	bool inFocus = false;
	
	// Windows message handling function.
	LRESULT CALLBACK Application::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		//case WM_CREATE:
			//return 0;
		case WM_DESTROY:
		case WM_CLOSE:
			Application::Instance().SafeExit();
			return 0;

		case WM_ACTIVATE:
			if (inFocus)
				inFocus = false;
			else
				inFocus = true;
			return 0;

		// ----------------------------------------------------
		//						 MOUSE INPUT
		// Note: All mouse InputHandler messages occur when the
		// mouse is hit or lifted. It does not post the message
		// if the mouse is already down.
		// ----------------------------------------------------
			
		// Left Mouse Input Messages
        case WM_LBUTTONDOWN:
			INPUT_HANDLER->SetLeftMouse(true);
			return 0;
        case WM_LBUTTONUP:
			INPUT_HANDLER->SetLeftMouse(false);
			return 0;

		// Middle Mouse Input Messages
        case WM_MBUTTONDOWN:
			INPUT_HANDLER->SetMiddleMouse(true);
			return 0;
        case WM_MBUTTONUP:
			INPUT_HANDLER->SetMiddleMouse(false);
			return 0;

		// Right Mouse Input Messages
		case WM_RBUTTONDOWN:
			INPUT_HANDLER->SetRightMouse(true);
			return 0;
		case WM_RBUTTONUP:
			INPUT_HANDLER->SetRightMouse(false);
			return 0;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Application::Application()
		: m_timer(NULL)
		, m_device(NULL)
		, m_settings(NULL)
		, m_applicationRunning(false)
		, hWnd(NULL)
	{
		// Initlisation takes place in the Startup method.
	}

	Application::~Application()
	{
		// Clean up is done through the shutdown method.
	}

	// Singleton Access.
	Application& Application::Instance()
	{
		static Application instance;
		return instance;
	}

	SWR_ERR Application::Startup(const ApplicationSettings &settings)
	{
		gLogger.EnableConsoleDumping(true);

		m_settings = new ApplicationSettings(settings);
		inFocus = false;

		m_timer = new Timer();
		m_timer->Tick();

		ZeroMemory(&wc, sizeof(WNDCLASS));

		wc.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc    = (WNDPROC) WinProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra		= 0;
		wc.hInstance      = NULL; // hInstance;
		wc.hIcon          = NULL;
		wc.hCursor        = NULL;
		wc.hbrBackground  = NULL;
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = m_settings->m_appName.c_str();

		RegisterClass(&wc);

		DWORD style;

		// Readjust the window so that it has 1:1 screen to world coords.
//		SIZE size;
		RECT clientRect;

		if (m_settings->m_fullscreen)
		{
			style = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;	
		}
		else
		{

			style = WS_OVERLAPPEDWINDOW;
			clientRect.left = clientRect.top = 0;
			clientRect.right = m_settings->m_width - 1;
			clientRect.bottom = m_settings->m_height - 1;
			BOOL ok = AdjustWindowRect(&clientRect, style, FALSE);
			if (!ok)
			{
				LOG("Client window resize failed", LOG_Error);
				//exit(1);
			}
			else
			{
				LOG("Client window resize successfull", LOG_Init);
			}

			// Reset the back buffer size.
			//m_settings->m_width = size.cx;
			//m_settings->m_height = size.cy;
		}
		

		hWnd = CreateWindow(m_settings->m_appName.c_str(), 
							m_settings->m_appName.c_str(),
							style,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							(clientRect.right - clientRect.left) + 1,
							(clientRect.bottom - clientRect.top) + 1,
							NULL,
							NULL,
							NULL,
							NULL);

		if (hWnd == NULL)
		{
			LOG("Application startup has failed: Window creation has failed.", LOG_Error);
			return SWR_FAIL;
		}

		// Initilise the render device.
		SWR_ERR result = InitRenderDevice();
		if (result != SWR_OK)
		{
			LOG("Application startup has failed: Render Device configuration failure.", LOG_Error);
			return SWR_FAIL;
		}

		LOG("Application startup was successful.", LOG_Init);

		// Show and update the window.
		ShowWindow(hWnd, SW_SHOWNORMAL);
		UpdateWindow(hWnd);

		SetFocus(hWnd);
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		// Initilise the frame listeners.
		for (unsigned int i = 0; i < m_listeners.size(); i++)
		{
			m_listeners[i]->Initilise();
		}


		return SWR_OK;
	}

	SWR_ERR Application::InitRenderDevice()
	{
		m_device = new RenderDevice();
		SWRInitParams params;
		ZeroMemory(&params, sizeof(SWRInitParams));

		// Set up the parameters to configure the renderer.
		params.bufferHeight = m_settings->m_height;
		params.bufferWidth = m_settings->m_width;
		params.useZBuffer = true;
		params.texMapType = TEX_MAP_Affine;
		params.bitDepth = DBD_Bit32;

		return m_device->Initilise(params, hWnd);
	}

	void Application::Shutdown()
	{
		// Destroy the frame listeners
		for (unsigned int i = 0; i < m_listeners.size(); i++)
		{
			m_listeners[i]->Cleanup();
			delete m_listeners[i];
		}

		m_listeners.clear();

		// Destroy application structures.
		if (m_settings != NULL)
		{
			delete m_settings;
			m_settings = NULL;
		}

		if (m_timer != NULL)
		{
			delete m_timer;
			m_timer = NULL;
		}

		if (m_device != NULL)
		{
			m_device->Release(hWnd);
			delete m_device;
			m_device = NULL;
		}

		
		DestroyWindow(hWnd);
		hWnd = NULL;

		// Dump the logged contents.
		gLogger.DumpLog();
	}

	void Application::SafeExit()
	{
		m_exit = true;
	}

	void Application::ForceExit()
	{
		MessageBox(0,"Application has been forceably terminated, see output log for details",0, MB_OK);
		LOG("Application has been forceably terminated, see output log for details", LOG_Error);
		Shutdown();
		exit(1);
	}

	void Application::AddFrameListener(FrameListener* listener)
	{
		assert(!m_applicationRunning);
		m_listeners.push_back(listener);

	}

	void Application::Run()
	{
		if (m_exit == true)
			return;

		MSG msg;
		m_applicationRunning = true;
		while (!m_exit)
		{
			
			// Process windows messages.
            while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
            {
				if (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					return;
				}
            }

			m_timer->Tick();
			
			if (inFocus)
			{
				INPUT_HANDLER->UpdateInputDevices(hWnd);
			}

			// Update and render the applications.
			FrameStartTick();
			RenderFrame();
			FrameEndTick();
		}

	}

	void Application::FrameStartTick()
	{
		for (unsigned int it = 0; it < this->m_listeners.size(); it++)
		{
			m_listeners[it]->OnFrameStart(m_timer->getDeltaTime());
		}
	}

	void Application::RenderFrame()
	{
		// Start render.
		m_device->ClearBackBuffer(m_settings->m_clearColour.ToUINT32());
		m_device->ClearZBuffer();

		// Let frame listeners render.
		for (unsigned int it = 0; it < this->m_listeners.size(); it++)
		{
			m_listeners[it]->Render(m_timer->getDeltaTime());
		}

		// End the render.
		m_device->Present(hWnd);
	}

	void Application::FrameEndTick()
	{	
		for (unsigned int it = 0; it < this->m_listeners.size(); it++)
		{
			m_listeners[it]->OnFrameEnd(m_timer->getDeltaTime());
		}
	}


	void Application::ShowFPS()
	{
		m_drawFPS = true;
	}

	void Application::HideFPS()
	{
		m_drawFPS = false;
	}

	bool Application::IsShowingFPS() const
	{
		return m_drawFPS;
	}

	RenderDevice* Application::GetRenderDevice()
	{
		return m_device;
	}

	int Application::GetFPS()
	{
		return this->m_timer->GetFPS();
	}

}; // End namespace SWR.