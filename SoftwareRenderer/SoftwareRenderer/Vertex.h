#pragma once

#ifndef VERTEX_H
#define VERTEX_H

//****************************************************************************
//**
//**    Vertex.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 06/2010
//**
//****************************************************************************

#include <Windows.h>

#include "DataTypes.h"
#include "Colour.h"
#include "Vector3.h"


namespace SWR
{
	struct Vertex
	{
		Real x,y,z;
		Colour32 colour;
		float u,v;
		Real xNormal, yNormal, zNormal;

		Vertex(){}
		~Vertex(){}

		Vertex(const Vertex& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			colour = v.colour;
			u = v.u;
			this->v = v.v;
			xNormal = v.xNormal;
			yNormal = v.yNormal;
			zNormal = v.zNormal;
		}

		Vertex(float xIn, float yIn, float zIn, Colour32 colourIn, float uIn, float vIn, float xnIn, float ynIn, float znIn)
		{
			x = xIn;
			y = yIn;
			z = zIn;
			colour = colourIn;
			u = uIn;
			this->v = vIn;
			xNormal = xnIn;
			yNormal = ynIn;
			zNormal = znIn;
		}

		inline void operator=(const Vertex& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			colour = v.colour;
			u = v.u;
			this->v = v.v;
			xNormal = v.xNormal;
			yNormal = v.yNormal;
			zNormal = v.zNormal;
		}

		inline void ToVec3(Vector3 &vec)
		{
			vec.x = x;
			vec.y = y;
			vec.z = z;
		}

		inline void FromVec3( const Vector3 &vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}

		inline Vector3 GetNormal()
		{
			return Vector3(xNormal, yNormal, zNormal);
		}
	};

	struct Triangle
	{
		Vertex* verts;
	};

	// ------------------------------------------------------------------------
	//								VertexBuffer
	// ------------------------------------------------------------------------
	// Desc:
	// 
	// ------------------------------------------------------------------------
	class VertexBuffer
	{
	private:

		Vertex* m_vertices; // A byte buffer.
		U16 m_numOfVerts;
		
		friend SWR_ERR CreateVertexBuffer(void* verts, U16 totalVerts, VertexBuffer* &out_buffer);
	protected:
	public:

		VertexBuffer()
			: m_vertices(NULL)
			, m_numOfVerts(0)
		{
		}

		~VertexBuffer()
		{
			if (m_vertices != NULL)
			{
				delete [] m_vertices;
				m_vertices = NULL;
			}
		}

		Vertex* GetVertices();

		U16 GetTotalVerts() const;

		// Returns the size in BYTES of the total vertices stored by the buffer.
		size_t Size();
	};

	SWR_ERR CreateVertexBuffer(void* verts, U16 totalVerts, VertexBuffer* &out_buffer);

	// Model generation functions.
	SWR_ERR LoadModel(void* verts, void* indices, const char* declarationFile);
	SWR_ERR LoadVertsFromFile(Vertex* &verts, const char* filename, int totalVerts);
	SWR_ERR LoadIndicesFromFile(U16* &indices, const char* filename, int totalIndices);
	
}; // End namespace SWR.

#endif // #ifndef VERTEX_H