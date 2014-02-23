#pragma once

#ifndef ZDEPTH_BUFFER_H
#define ZDEPTH_BUFFER_H

//****************************************************************************
//**
//**    ZDepthBuffer
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "DataTypes.h"

// Forward Declarations
namespace SWR
{
	//class ...
};

namespace SWR
{
	// ------------------------------------------------------------------------
	//								ZDepthBuffer
	// ------------------------------------------------------------------------
	// Desc:
	// A buffer of signed shorts where -32767 = -1, 0 = 0 and 32767 = 1.
	// Used to represent the Z-Depth of pixels as they are plotted to a back-
	// buffer. 
	// ------------------------------------------------------------------------
	class ZDepthBuffer
	{
	private:
		U8* m_buffer;
		U16 m_width;
		U16 m_height;

		// These are precalculated buffer/chunk sizes to help optimise buffer clearing.
		U32 m_buffSize;
		U32 m_buffChunks; // The number of 2048 byte chunks in the buffer.
		U32 m_buffChunkSize;
		U32 m_buffSizeRemainder;
	protected:
	public:
		ZDepthBuffer();
		~ZDepthBuffer();

		SWR_ERR Initilise(U16 width, U16 height);
		void Release();

		void SetZDepth(U16 x, U16 y, S16 depth);
		bool ZDepthTest(U16 x, U16 y, S16 z);

		void Clear(S16 value);

		static const S16 MAX_Z_DEPTH = 32767;
	};
	
}; // End namespace SWR.

#endif // #ifndef ZDEPTH_BUFFER_H