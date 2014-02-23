//****************************************************************************
//**
//**    Timer.cpp
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2009
//**
//****************************************************************************

#include <windows.h>
#include <iostream>

#include "Timer.h"

#include "MemoryLeak.h"

namespace SWR
{

	Timer::Timer()
	:	mSecondsPerCount (0.0f)
	,	mDeltaTime(-1.0f)
	,	mBaseTime(0)
	,	mPrevTime(0)
	,	mCurrTime(0)
	,   mAvSecsPerFrame(0)
	,   numFrames(0)
	,   timeElapsed(0.0f)
	{
		__int64 countsPerSec = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		mSecondsPerCount=1.0f/(double)countsPerSec;
	}

	void Timer::Tick()
	{
		//Get the current time for this frame
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrTime=currTime;

		//calculate the delta between current frame and last frame in seconds
		mDeltaTime=(mCurrTime-mPrevTime)*mSecondsPerCount;

		//set the currnt frame time to previous to prep for next frame
		mPrevTime=mCurrTime;

		//Force nonnegative. The DXSDK's CDXUTTimer mentions that if the
		//processor goes into a power save mode or we get shuffled to 
		//another processor, then mDeltaTime can be negative
		if(mDeltaTime<0)
		{
			mDeltaTime=0;
		}

		timeElapsed += this->getDeltaTime();
		numFrames++;

		if (timeElapsed >= 1.0f)
		{
			this->FPS = numFrames;
			mAvSecsPerFrame = (timeElapsed / numFrames);
			numFrames = 0;
			timeElapsed = 0.0f;
			/*static char buffer[256] = {0};
			ZeroMemory(&buffer, 256);
			sprintf(buffer, "FPS: %i", FPS);
			puts(buffer);*/
		}
	}

	float Timer::GetSecsPerFrame() const
	{
		return (float)mAvSecsPerFrame;
	}

	float Timer::getDeltaTime() const
	{
		return (float)mDeltaTime;
	}

	float Timer::getGameTime() const
	{
		return(float)((mCurrTime-mPrevTime)*mSecondsPerCount);		
	}

	int Timer::GetFPS() const
	{
		return FPS;
	}

}; // End namespace SWR