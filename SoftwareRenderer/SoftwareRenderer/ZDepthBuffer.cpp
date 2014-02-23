//****************************************************************************
//**
//**    ZDepthBuffer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include "ZDepthBuffer.h"

#include "Logger.h"
#include "MemoryLeak.h"

#define PIXELS_TO_CLEAR 1024

namespace SWR
{
	ZDepthBuffer::ZDepthBuffer()
	{
		m_buffer = NULL;
	}

	ZDepthBuffer::~ZDepthBuffer()
	{
	}

	SWR_ERR ZDepthBuffer::Initilise(U16 width, U16 height)
	{
		if (m_buffer != NULL)
		{
			Release();
		}

		m_buffer = (U8*) malloc(sizeof(S16) * width * height);//[width * height];
		
		m_width	= width;
		m_height = height;
		//pitch	= width;

		m_buffSize = (U32)m_width * (U32)m_height;
		m_buffChunks = m_buffSize / (PIXELS_TO_CLEAR * 2); // Calculate how many byte chunks we have that are within the buffer size.
		m_buffSizeRemainder = m_buffSize % m_buffChunks;
		m_buffChunkSize = m_buffSize - m_buffSizeRemainder;

		return SWR_OK;
	}

	void ZDepthBuffer::Release()
	{
		if (m_buffer != NULL)
		{
			free(m_buffer);
			m_buffer = NULL;
		}
	}

	void ZDepthBuffer::SetZDepth(U16 x, U16 y, S16 depth)
	{
	}

	bool ZDepthBuffer::ZDepthTest(U16 x, U16 y, S16 z)
	{
		S16 value = m_buffer[x + (y * m_width)];
		if (value > z && value < 0)
		{
			return false;
		}
		return true;
	}

	void ZDepthBuffer::Clear(S16 value)
	{
		U32 index = 0;
		U32 byteIndex = 0;
		const U32 pixelsToCopy = PIXELS_TO_CLEAR;
		unsigned char buffer[pixelsToCopy * 2];

		// Copy the clear colour into the byte buffer.
		for (U16 pixelByte = 0; pixelByte < pixelsToCopy * 2; pixelByte += 2)
		{
			memcpy(&buffer[pixelByte], &value, 2);
		}

		// Cycle over the pixel buffer up to the closest power of 2 to the the buffer size and copy large
		// byte chunks into the buffer to help minimise individual write calls.
		for (index = 0; index < m_buffChunkSize; index += pixelsToCopy)
		{
			memcpy(&(m_buffer[index]), &buffer, pixelsToCopy * 2);
		}

		// Do a standard / sloooow copy into the buffer for the remainder.
		for (index = m_buffChunkSize; index < m_buffSize; index++)
		{
			memcpy(&(m_buffer[index]), &value, 2); // Copy the 4 bytes of the clear colour into our buffer.
		}
	}
	
}; // End namespace SWR.
