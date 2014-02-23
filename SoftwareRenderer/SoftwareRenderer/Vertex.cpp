//****************************************************************************
//**
//**    VertexBuffer.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>
#include <iostream>

#include "Vertex.h"
#include "Logger.h"

#include "MemoryLeak.h"

namespace SWR
{
	Vertex* VertexBuffer::GetVertices()
	{
		return (Vertex*)m_vertices;
	}

	U16 VertexBuffer::GetTotalVerts() const
	{
		return m_numOfVerts;
	}

	size_t VertexBuffer::Size()
	{
		return m_numOfVerts * sizeof(Vertex);
	}


	SWR_ERR CreateVertexBuffer(void* verts, U16 totalVerts, VertexBuffer* &out_buffer)
	{
		out_buffer = new VertexBuffer();
		out_buffer->m_numOfVerts = totalVerts;

		size_t size = totalVerts * sizeof(Vertex);

		// Allocate the new buffer.
		out_buffer->m_vertices = new Vertex[totalVerts];

		if (out_buffer->m_vertices == NULL)
			return SWR_FAIL;

		memcpy(out_buffer->m_vertices, verts, size);
		return SWR_OK;

	}

	SWR_ERR LoadModel(void* verts, void* indices, const char* declarationFile)
	{
		return SWR_FAIL;
	}

	SWR_ERR LoadVertsFromFile(Vertex* &verts, const char* filename, int totalVerts)
	{
		// Try to open the file.
		//verts = malloc;
		FILE* file = fopen(filename, "rb");
		if (file != NULL)
		{
			// Allocate the vertices and get them from the binary.
			verts = new Vertex[totalVerts];
			size_t totalRead = sizeof(Vertex); 
			totalRead *= totalVerts;

			size_t size = fread(verts,1 , totalRead, file);
			if (size == 0)
			{
				return SWR_FAIL;
			}
		}
		else
		{
			return SWR_FAIL;
		}
		LOG("Model file loaded.", LOG_Standard);
		return SWR_OK;
	}
	
	SWR_ERR LoadIndicesFromFile(U16* &indices, const char* filename, int totalIndices)
	{
		// Try to open the file.
		//verts = malloc;
		FILE* file = fopen(filename, "rb");
		if (file != NULL)
		{
			// Allocate the vertices and get them from the binary.
			indices = new U16[totalIndices];
			size_t totalRead = sizeof(U16); 
			totalRead *= totalIndices;

			size_t size = fread(indices,1 , totalRead, file);
			if (size == 0)
			{
				return SWR_FAIL;
			}
		}
		else
		{
			return SWR_FAIL;
		}
		LOG("Index file loaded.", LOG_Standard);
		return SWR_OK;
	}


}; // End namespace SWR.