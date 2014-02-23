//****************************************************************************
//**
//**    Logger.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 05/2010
//**
//****************************************************************************

#include <iostream>
#include <fstream>

#include "Logger.h"

#include "MemoryLeak.h"

namespace SWR
{		
	Logger::Logger()
		: m_consoleDump(false)
	{
		m_log.clear();
	}

	Logger::~Logger()
	{
	}

	Logger& Logger::Instance()
	{
		static Logger instance;
		return instance;
	}

	void Logger::EnableConsoleDumping(bool enable)
	{
		m_consoleDump = enable;
	}

	void Logger::AddToLogDump(LogItem item)
	{
		// Only do log dumping if we are in debug mode.
//#if defined(_DEBUG)
		m_log.push_back(item);
		static char buffer[1024] = {0};
		memset(buffer, 0, 1024);

		if (m_consoleDump)
		{
			sprintf(buffer,"%s %s", ToLogType(item.type), item.info.c_str());
			puts(buffer);
		}
//#endif // #if defined(_DEBUG)
	}

	void Logger::DumpLog()
	{
		std::ofstream file;
		file.open("SWRDebugLog.txt");

		// Dump the loggers contents to the file.
		for (unsigned int i = 0; i < m_log.size(); i++)
		{
			file << ToLogType(m_log[i].type) << m_log[i].info << std::endl;
		}

		file.close();
	}

	void Logger::EmptyLog()
	{
		m_log.clear();
	}

	char* Logger::ToLogType(LogType type)
	{
		if (type == LOG_Standard)	
		{
			return "Log     : ";
		}
		if (type == LOG_Warning)
		{
			return "Warning : ";
		}
		if (type == LOG_Error)	
		{
			return "Error   : ";
		}
		if (type == LOG_Init)	
		{
			return "Init    : ";
		}
		if (type == LOG_Shutdown)	
		{
			return "Shutdown: ";
		}

		return ""; // Shouldnt reach this.
	}

	
}; // End namespace SWR