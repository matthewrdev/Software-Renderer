#pragma once

#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

//****************************************************************************
//**
//**    IndexBuffer.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#ifndef NULL
#define NULL 0
#endif // #ifndef NULL

#include "DataTypes.h"

namespace SWR
{
	// ------------------------------------------------------------------------
	//								IndexBuffer
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class IndexBuffer
	{
	private:
		U16* m_indices;
		U16 m_totalIndices;

		friend SWR_ERR CreateIndexBuffer(U16* indices, U16 totalIndices, IndexBuffer* &out_buffer);
	protected:
	public:
		IndexBuffer()
			: m_indices(NULL)
			, m_totalIndices(0)
		{	}

		~IndexBuffer()
		{
			if (m_indices != NULL)
			{
				delete [] m_indices;
				m_indices = NULL;
			}
		}

		U16* GetBuffer();
		U16 GetTotalIndices() const;
	};

	SWR_ERR CreateIndexBuffer(U16* indices, U16 totalIndices, IndexBuffer* &out_buffer);
	
}; // End namespace SWR.

#endif // #ifndef INDEX_BUFFER_H