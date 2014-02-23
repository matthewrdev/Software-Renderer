//****************************************************************************
//**
//**    BackBuffer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <assert.h>

#include "BackBuffer.h"
#include "Colour.h"

#include "SWRUtil.h"
#include "Logger.h"
#include "MemoryLeak.h"

// The amount of pixels we are copying to the back-buffer in a clear operation.
#define PIXELS_PER_WRITE 512

namespace SWR
{
	BackBuffer::BackBuffer()
		: bufDevContext(NULL)
		, parentDevContext(NULL)
		, bufferBmp(NULL)
		, defBmp(NULL)
		, bitBuffer(NULL)
	{
		// Constructor only initilises to default.
	}

	BackBuffer::~BackBuffer()
	{
		// All releasing is done through the release method.
	}

	SWR_ERR BackBuffer::CreateBuffer(U16 width, U16 height, HDC winDevContext)
	{
		// The following code is modified from an application called simple3D.
		BITMAPINFO	*bmpInfo;
		byte		*data;

		assert(bufferBmp == NULL);	
		assert(winDevContext != NULL);

		data = new byte[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
		memset(data,0,sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
 
		bmpInfo = (BITMAPINFO*)data;
		bmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		bmpInfo->bmiHeader.biWidth			= width;
		bmpInfo->bmiHeader.biHeight			= -((long)width);		// DIBs are initially flipped
		bmpInfo->bmiHeader.biBitCount		= 32;
		bmpInfo->bmiHeader.biPlanes			= 1;
		bmpInfo->bmiHeader.biCompression	= BI_RGB;
		bmpInfo->bmiHeader.biSizeImage		= 0;
		bmpInfo->bmiHeader.biXPelsPerMeter	= 0;
		bmpInfo->bmiHeader.biYPelsPerMeter	= 0;
		bmpInfo->bmiHeader.biClrUsed		= 256;
		bmpInfo->bmiHeader.biClrImportant	= 256;

		bufferBmp = CreateDIBSection(winDevContext,bmpInfo,DIB_RGB_COLORS,&bitBuffer,NULL,0);
		if(bufferBmp == NULL)
		{
			LOG("Back-buffer bitmap has failed to create.", LOG_Error);
			delete [] data;
			return SWR_FAIL;
		}

		bufDevContext = CreateCompatibleDC(winDevContext);
		if(bufDevContext == NULL)
		{
			LOG("Back-buffer device context has failed to create.", LOG_Error);
			delete [] data;
			return SWR_FAIL;
		}

		defBmp = (HBITMAP)SelectObject(bufDevContext,bufferBmp);
		if(defBmp == NULL)
		{
			LOG("Back-buffer: cannot select DIB on storage DC", LOG_Error);
			delete [] data;
			return SWR_FAIL;
		}
	
		delete [] data;

		parentDevContext = winDevContext;

		m_width	= width;
		m_height = height;
		//pitch	= width;

		m_buffSize = (U32)m_width * (U32)m_height;
		m_buffChunks = m_buffSize / (PIXELS_PER_WRITE * 4); // Calculate how many byte chunks we have that are within the buffer size.
		m_buffSizeRemainder = m_buffSize % m_buffChunks;
		m_buffChunkSize = m_buffSize - m_buffSizeRemainder;

		// Just for sanities sake, check to see if the user created a back-buffer with a size that isnt going to 
		// break the cache optimising technique. 
		if (m_buffSize < PIXELS_PER_WRITE *4)
		{
			char buff[256] = {0};
			sprintf(buff, "Back-buffer size is less that the optimised pixel cache size of: %i", PIXELS_PER_WRITE);
			LOG(buff, LOG_Error);
			return SWR_FAIL;

		}

		// Cast the bit buffer to the byteBuffer so we dont have to do a cast every time we want to plot a pixel.
		byteBuffer = (U8*)bitBuffer;

		LOG("Back-buffer initilisation successful.", LOG_Init);
		return SWR_OK;
	}
	
	void BackBuffer::ReleaseBuffer()
	{
		if(bufferBmp != NULL)
		{
			if(bufDevContext != NULL)
			{
				if(defBmp != NULL)
				{
					SelectObject(bufDevContext,defBmp);
					defBmp = NULL;
				}

				DeleteDC(bufDevContext);
				bufDevContext = NULL;
			}

			DeleteObject(bufferBmp);
			bufferBmp = NULL;
		}

		bitBuffer = NULL;
		byteBuffer = NULL;
	}

	void BackBuffer::Clear(U32 colour)
	{
		// SIMD or MMX instructions for transferring multiple data.
		// Painters Algorithm.

		/*for (U32 index = 0; index < m_buffSize; index++)
		{
			memcpy(&(byteBuffer[index << 2]), &colour, 4);
		} */

		// These are the statistics that I would get for clearing a 800x600 backbuffer on an i7 at 1.6-2.5ghz
		// in raw FPS without any other calculations for the different clearing techniques.
		// Using nested for loops and writing to the bufffer each iteration:      70fps.
		// Using a single for loop and writing to the bufffer each iteration:     130 fps.
		// Creating a 2048 byte pixel buffer and writing in chunks to the buffer: 1400 - 1600fps.

		// While this clearing solution is pretty ugly it is much faster than doing it in a nested or single
		// for loop.
		// By breaking the read and writes into larger power of 2 sizes it increases the cache hit rate and 
		// improves the performance drastically (very drastically).

		U32 index = 0;
		U32 byteIndex = 0;
		const U32 pixelsToCopy = PIXELS_PER_WRITE;
		unsigned char buffer[pixelsToCopy * 4];
		U32* clearBuffer = (U32*)buffer;

		// Copy the clear colour into the 128 byte buffer.
		for (U16 pixelByte = 0; pixelByte < pixelsToCopy * 4; pixelByte += 4)
		{
			memcpy(&buffer[pixelByte], &colour, 4);
		}

		// Cycle over the pixel buffer up to the closest power of 2 to the the buffer size and copy large
		// byte chunks into the buffer to help minimise individual write calls.
		for (index = 0; index < m_buffChunkSize; index += pixelsToCopy)
		{
			byteIndex = index << 2;
			memcpy(&(byteBuffer[byteIndex]), &buffer, pixelsToCopy * 4);
		}

		// Do a standard / sloooow copy into the buffer for the remainder.
		for (index = m_buffChunkSize; index < m_buffSize; index++)
		{
			byteIndex = index << 2;
			memcpy(&(byteBuffer[byteIndex]), &colour, 4); // Copy the 4 bytes of the clear colour into our buffer.
		}
	}

	// Uh yeah, this is extremely slow and should never be used.
	void BackBuffer::PlotPixel(U16 x, U16 y, U32 color)
	{
		// Calculate the index into the buffer.
		U32 index = (x + (y * m_width)) << 2; // or *4

		// Check for overflow.
		if (x + (y * m_width) > m_width * m_height)
			return;

		// Copy the 4 bytes of the clear colour into our buffer.
		memcpy(&(byteBuffer[index]), &color, 4);
	}

	HDC BackBuffer::GetDeviceContext()
	{
		return bufDevContext;
	}

	void* BackBuffer::GetBitBuffer()
	{
		return bitBuffer;
	}

	U8* BackBuffer::GetByteBuffer()
	{
		return byteBuffer;
	}
	
	U32 BackBuffer::GetWidth() const
	{
		return m_width;
	}

	U32 BackBuffer::GetHeight() const
	{
		return m_height;
	}
	
}; // End namespace SWR.