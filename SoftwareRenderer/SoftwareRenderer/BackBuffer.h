#pragma once

#ifndef BUFFER_H
#define BUFFER_H

//****************************************************************************
//**
//**    BackBuffer
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <windows.h>
#include "DataTypes.h"

// Forward Declarations
namespace SWR
{
	class Colour32;
};

namespace SWR
{
	// ------------------------------------------------------------------------
	//								BackBuffer
	// ------------------------------------------------------------------------
	// Desc:
	// Represents the back-buffer that is presented to a window.
	// ------------------------------------------------------------------------
	class BackBuffer
	{
	private:
		// The windows device contexts.
		HDC bufDevContext;
		HDC parentDevContext;

		// Bitmap buffer.
		HBITMAP		bufferBmp;
		HBITMAP		defBmp;

		void* bitBuffer;
		U8* byteBuffer; // Just the bitbuffer casted so we dont have to cast every pixel we plot.
		U8* clearBytes;

		U16 m_width;
		U16 m_height;

		// These are precalculated buffer/chunk sizes to help optimise buffer clearing.
		U32 m_buffSize;
		U32 m_buffChunks; // The number of 2048 byte chunks in the buffer.
		U32 m_buffChunkSize;
		U32 m_buffSizeRemainder;

	protected:
	public:
		BackBuffer();
		~BackBuffer();

		SWR_ERR CreateBuffer(U16 width, U16 height, HDC winDevContext);
		void ReleaseBuffer();

		// Optimised clear function.
		// Rather than using a single for loop to clear the buffer, breaks it up to write large byte
		// chunks each loop iteration. Much more cache friendly...
		void Clear(U32 colour);

		// This function is not recomended for use. Is not pipeline or cache friendly.
		// Retrieve the byte buffer and write pixels that way, your code will perform better methinks.
		void PlotPixel(U16 x, U16 y, U32 color); 

		HDC GetDeviceContext();
		void* GetBitBuffer();
		U8* GetByteBuffer();

		U32 GetWidth() const;
		U32 GetHeight() const;
	};
	
}; // End namespace SWR.

#endif // #ifndef BUFFER_H