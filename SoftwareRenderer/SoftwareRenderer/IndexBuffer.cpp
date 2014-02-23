//****************************************************************************
//**
//**    IndexBuffer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>

#include "IndexBuffer.h"

#include "MemoryLeak.h"

namespace SWR
{

	U16* IndexBuffer::GetBuffer()
	{
		return m_indices;
	}

	U16 IndexBuffer::GetTotalIndices() const
	{
		return m_totalIndices;
	}

	SWR_ERR CreateIndexBuffer(U16* indices, U16 totalIndices, IndexBuffer* &out_buffer)
	{
		if (indices != NULL && totalIndices > 0)
		{
			out_buffer = new IndexBuffer();
			out_buffer->m_totalIndices = totalIndices;
			out_buffer->m_indices = new U16[totalIndices];

			// Copy the existing buffer across.
			memcpy(out_buffer->m_indices, indices, totalIndices * sizeof(U16));

			return SWR_OK;
		}

		// Invalid index buffer on fall-through.
		return SWR_FAIL;
	}
	
}; // End namespace SWR.
