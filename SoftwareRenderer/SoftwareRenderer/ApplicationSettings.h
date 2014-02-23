#pragma once

#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

//****************************************************************************
//**
//**    ApplicationSettings.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <string>

#include "Colour.h"

namespace SWR
{
	SWR_ERR LoadFromIni(const char* config, ApplicationSettings &settings);

	class ApplicationSettings
	{
	private:
		bool m_logEnable; // If output logging is enabled.
		bool m_debugEnable;

		//friend LoadFromIni;
	protected:
	public:
		unsigned short m_width;
		unsigned short m_height;
		bool m_fullscreen;
		std::string m_appName;
		Colour32 m_clearColour;

		ApplicationSettings(){}
		ApplicationSettings(const ApplicationSettings &settings)
		{
			m_width = settings.m_width;
			m_height = settings.m_height;
			m_fullscreen = settings.m_fullscreen;
			m_appName =	settings.m_appName;
			m_clearColour = settings.m_clearColour;
		}

		~ApplicationSettings(){}

		void Init(unsigned short width, unsigned short height, bool fullscreen, char* name, Colour32 clearColour)
		{
			m_width = width;
			m_height = height;
			m_fullscreen = fullscreen;
			m_appName = name;
			m_clearColour = clearColour;
		}

	};

	
}; // End namespace SWR.

#endif // #ifndef APPLICATION_SETTINGS_H