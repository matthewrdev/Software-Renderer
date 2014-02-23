#pragma once

#ifndef GAME_TIMER_H
#define GAME_TIMER_H

//****************************************************************************
//**
//**    Timer.h
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2009
//**
//****************************************************************************


namespace SWR
{
	class Timer
	{
	public:
		Timer();

		float getGameTime() const;	//returns game time in seconds
		float getDeltaTime() const;	//returns the change in time in seconds

		void Tick();

		int GetFPS() const;
		float GetSecsPerFrame() const;

	private:
		double mSecondsPerCount;
		double mDeltaTime;
		double mAvSecsPerFrame;

		int FPS;

		int numFrames;
		float timeElapsed;

		__int64 mBaseTime;
		__int64 mPrevTime;
		__int64 mCurrTime;
	};
	
}; // End namespace SWR.

#endif // #ifndef GAME_TIMER_H