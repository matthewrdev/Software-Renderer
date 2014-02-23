#pragma once

#ifndef FRAME_LISTENER_H
#define FRAME_LISTENER_H

//****************************************************************************
//**
//**    FrameListener.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************


namespace SWR
{
	class FrameListener
	{
	private:
	protected:
	public:
		FrameListener(){}
		virtual ~FrameListener(){}

		// When using a frame listener you should use these functions for your game set-up / clean up over constructors/destructors.
		virtual void Initilise() = 0;
		virtual void Cleanup() = 0;

		virtual void OnFrameStart(float FrameDelta) = 0;
		virtual void Render(float FrameDelta) = 0; // Putting render functionality in a listener will cause issues but will do for the meantime.
		virtual void OnFrameEnd(float FrameDelta) = 0;
	};
	
}; // End namespace SWR.

#endif // #ifndef FRAME_LISTENER_H