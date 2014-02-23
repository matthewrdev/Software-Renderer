#pragma once

#ifndef LOGGER_H
#define LOGGER_H

//****************************************************************************
//**
//**    Logger.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 05/2010
//**
//****************************************************************************

#include <vector>
#include <string>

namespace SWR
{

	enum LogType
	{						// What will be printed out...
		LOG_Standard,		// Log      :
		LOG_Warning,		// Warning  : 
		LOG_Error,			// Error    :
		LOG_Init,           // Init     :
		LOG_Shutdown,		// Shutdown :
	};

	struct LogItem;
	
	// ------------------------------------------------------------------------
	//								Logger
	// ------------------------------------------------------------------------
	// Desc:
	// The logger is designed to log events as they happen at certain stages of
	// the applications lifetime to help provide an insight into how the 
	// application ran and its possible problems.
	// ------------------------------------------------------------------------
	class Logger
	{
	public:
		static Logger& Instance();
		~Logger();

		// When a log item is submitted this will also dump an item that is inputed to the logger to the console.
		void EnableConsoleDumping(bool enable);

		void AddToLogDump(LogItem item);
		void DumpLog();
		void EmptyLog();

	protected:
	private:
		Logger();
		bool m_consoleDump;

		std::vector<LogItem> m_log;

		char* ToLogType(LogType type);

	};

	struct LogItem
	{
		std::string info;
		LogType type;
		LogItem(){}
		LogItem(std::string infoIn, LogType typeIn)
		{
			info = infoIn;
			type = typeIn;
		}
	};
	
}; // End namespace SWR

// Macros to help logging
#define gLogger SWR::Logger::Instance()
#define LOG(text, type)  SWR::Logger::Instance().AddToLogDump(SWR::LogItem(std::string(text), type))

#endif // #ifndef B3D_LOGGER_H